#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Carla bridge for LV2 modguis
# Copyright (C) 2015 Filipe Coelho <falktx@falktx.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# For a full copy of the GNU General Public License see the doc/GPL.txt file.

# ------------------------------------------------------------------------------------------------------------
# Imports (Config)

from carla_config import *

# ------------------------------------------------------------------------------------------------------------
# Imports (Global)

if config_UseQt5:
    from PyQt5.QtCore import pyqtSlot, QPoint, QThread, QUrl
    from PyQt5.QtWidgets import QMainWindow
    from PyQt5.QtWebKitWidgets import QWebElement, QWebSettings, QWebView
else:
    from PyQt4.QtCore import pyqtSlot, QPoint, QThread, QUrl
    from PyQt4.QtGui import QMainWindow
    from PyQt4.QtWebKit import QWebElement, QWebSettings, QWebView

# ------------------------------------------------------------------------------------------------------------
# Imports (Custom)

from carla_app import *
from carla_utils import *

# ------------------------------------------------------------------------------------------------------------
# Generate a random port number between 9000 and 18000

from random import random

PORTn = 8998 + int(random()*9000)

# ------------------------------------------------------------------------------------------------------------
# Set up environment for the webserver

PORT     = str(PORTn)
ROOT     = "/usr/share"
ROOT     = "/home/falktx/Personal/FOSS/Git-mine/mod-app/source/modules"
DATA_DIR = os.path.expanduser("~/.local/share/mod-data/")

os.environ['MOD_DEV_HOST'] = "1"
os.environ['MOD_DEV_HMI']  = "1"
os.environ['MOD_DESKTOP']  = "1"
os.environ['MOD_LOG']      = "0"

os.environ['MOD_DATA_DIR']           = DATA_DIR
os.environ['MOD_HTML_DIR']           = os.path.join(ROOT, "mod-ui", "html")
os.environ['MOD_PLUGIN_LIBRARY_DIR'] = os.path.join(DATA_DIR, 'lib')

os.environ['MOD_PHANTOM_BINARY']        = "/usr/bin/phantomjs"
os.environ['MOD_SCREENSHOT_JS']         = os.path.join(ROOT, "mod-ui", "screenshot.js")
os.environ['MOD_DEVICE_WEBSERVER_PORT'] = PORT

# FIXME
os.environ['MOD_DEFAULT_JACK_BUFSIZE']  = "0"

#sys.path = [os.path.join(ROOT, "mod-ui")] + sys.path

# ------------------------------------------------------------------------------------------------------------
# Imports (MOD)

from mod import webserver
from mod.lv2 import PluginSerializer
from mod.session import SESSION

# Dummy monitor var, we don't need it
SESSION.monitor_server = "skip"

# ------------------------------------------------------------------------------------------------------------
# WebServer Thread

class WebServerThread(QThread):
    def __init__(self, parent=None):
        QThread.__init__(self, parent)

    def run(self):
        webserver.run()

    def stopWait(self):
        webserver.stop()
        return self.wait(5000)

# ------------------------------------------------------------------------------------------------------------
# Host Window

class HostWindow(QMainWindow):
    # signals
    SIGTERM = pyqtSignal()
    SIGUSR1 = pyqtSignal()

    # --------------------------------------------------------------------------------------------------------

    def __init__(self):
        QMainWindow.__init__(self)
        gCarla.gui = self

        URI = sys.argv[1]

        # ----------------------------------------------------------------------------------------------------
        # Internal stuff

        self.fCurrentFrame = None
        self.fDocElemement = None
        self.fCanSetValues = False
        self.fNeedsShow    = False
        self.fSizeSetup    = False
        self.fQuitReceived = False
        self.fWasRepainted = False

        self.fPlugin      = PluginSerializer(URI)
        self.fPorts       = self.fPlugin.data['ports']
        self.fPortSymbols = {}
        self.fPortValues  = {}

        for port in self.fPorts['control']['input'] + self.fPorts['control']['output']:
            self.fPortSymbols[port['index']] = port['symbol']
            self.fPortValues [port['index']] = port['default']

        # ----------------------------------------------------------------------------------------------------
        # Init pipe

        if len(sys.argv) == 7:
            self.fPipeClient = gCarla.utils.pipe_client_new(lambda s,msg: self.msgCallback(msg))
        else:
            self.fPipeClient = None

        # ----------------------------------------------------------------------------------------------------
        # Init Web server

        self.fWebServerThread = WebServerThread(self)
        self.fWebServerThread.start()

        # ----------------------------------------------------------------------------------------------------
        # Set up GUI

        self.fWebview = QWebView(self)
        self.setCentralWidget(self.fWebview)
        self.setContentsMargins(0, 0, 0, 0)

        mainFrame = self.fWebview.page().mainFrame()
        mainFrame.setScrollBarPolicy(Qt.Horizontal, Qt.ScrollBarAlwaysOff)
        mainFrame.setScrollBarPolicy(Qt.Vertical, Qt.ScrollBarAlwaysOff)

        settings = self.fWebview.settings()
        settings.setAttribute(QWebSettings.DeveloperExtrasEnabled, True)

        self.fWebview.loadFinished.connect(self.slot_webviewLoadFinished)

        url = "http://127.0.0.1:%s/icon.html?v=0#%s" % (PORT, URI)
        self.fWebview.load(QUrl(url))

        # ----------------------------------------------------------------------------------------------------
        # Connect actions to functions

        self.SIGTERM.connect(self.slot_handleSIGTERM)

        # ----------------------------------------------------------------------------------------------------
        # Final setup

        self.fIdleTimer = self.startTimer(30)

        if self.fPipeClient is None:
            # testing, show UI only
            self.setWindowTitle("TestUI")
            self.fNeedsShow = True

    # --------------------------------------------------------------------------------------------------------

    def closeExternalUI(self):
        self.fWebServerThread.stopWait()

        if self.fPipeClient is None:
            return

        if not self.fQuitReceived:
            self.send(["exiting"])

        gCarla.utils.pipe_client_destroy(self.fPipeClient)
        self.fPipeClient = None

    def idleStuff(self):
        if self.fPipeClient is not None:
            gCarla.utils.pipe_client_idle(self.fPipeClient)
            self.checkForRepaintChanges()

        if self.fSizeSetup:
            return
        if self.fDocElemement is None or self.fDocElemement.isNull():
            return

        pedal = self.fDocElemement.findFirst(".mod-pedal")

        if pedal.isNull():
            return

        size = pedal.geometry().size()

        if size.width() <= 10 or size.height() <= 10:
            return

        self.fSizeSetup    = True
        self.fDocElemement = None

        self.setFixedSize(size)
        self.fCurrentFrame.setScrollPosition(QPoint(15, 0))

        # set initial values
        for index in self.fPortValues.keys():
            symbol = self.fPortSymbols[index]
            value  = self.fPortValues[index]
            self.fCurrentFrame.evaluateJavaScript("icongui.setPortValue('%s', %f)" % (symbol, value))

        if self.fNeedsShow:
            self.show()

        self.fCanSetValues = True

    def checkForRepaintChanges(self):
        if not self.fWasRepainted:
            return

        self.fWasRepainted = False

        if not self.fCanSetValues:
            return

        for index in self.fPortValues.keys():
            symbol   = self.fPortSymbols[index]
            oldValue = self.fPortValues[index]
            newValue = self.fCurrentFrame.evaluateJavaScript("icongui.getPortValue('%s')" % (symbol,))

            if oldValue != newValue:
                self.fPortValues[index] = newValue
                self.send(["control", index, newValue])

    # --------------------------------------------------------------------------------------------------------

    @pyqtSlot(bool)
    def slot_webviewLoadFinished(self, ok):
        page = self.fWebview.page()
        page.repaintRequested.connect(self.slot_repaintRequested)

        self.fCurrentFrame = page.currentFrame()
        self.fDocElemement = self.fCurrentFrame.documentElement()

    def slot_repaintRequested(self):
        if self.fCanSetValues:
            self.fWasRepainted = True

    # --------------------------------------------------------------------------------------------------------
    # Callback

    def msgCallback(self, msg):
        msg = charPtrToString(msg)

        if msg == "control":
            index = int(self.readlineblock())
            value = float(self.readlineblock())
            self.dspParameterChanged(index, value)

        elif msg == "program":
            index = int(self.readlineblock())
            self.dspProgramChanged(index)

        elif msg == "midiprogram":
            bank    = int(self.readlineblock())
            program = float(self.readlineblock())
            self.dspMidiProgramChanged(bank, program)

        elif msg == "configure":
            key   = self.readlineblock()
            value = self.readlineblock()
            self.dspStateChanged(key, value)

        elif msg == "note":
            onOff    = bool(self.readlineblock() == "true")
            channel  = int(self.readlineblock())
            note     = int(self.readlineblock())
            velocity = int(self.readlineblock())
            self.dspNoteReceived(onOff, channel, note, velocity)

        elif msg == "atom":
            index      = int(self.readlineblock())
            size       = int(self.readlineblock())
            base64atom = self.readlineblock()
            # nothing to do yet

        elif msg == "urid":
            urid = int(self.readlineblock())
            uri  = self.readlineblock()
            # nothing to do yet

        elif msg == "uiOptions":
            sampleRate     = float(self.readlineblock())
            useTheme       = bool(self.readlineblock() == "true")
            useThemeColors = bool(self.readlineblock() == "true")
            windowTitle    = self.readlineblock()
            transWindowId  = int(self.readlineblock())
            self.uiTitleChanged(windowTitle)

        elif msg == "show":
            self.uiShow()

        elif msg == "focus":
            self.uiFocus()

        elif msg == "hide":
            self.uiHide()

        elif msg == "quit":
            self.fQuitReceived = True
            self.uiQuit()

        elif msg == "uiTitle":
            uiTitle = self.readlineblock()
            self.uiTitleChanged(uiTitle)

        else:
            print("unknown message: \"" + msg + "\"")

    # --------------------------------------------------------------------------------------------------------

    def dspParameterChanged(self, index, value):
        self.fPortValues[index] = value

        if self.fCurrentFrame is not None:
            symbol = self.fPortSymbols[index]
            self.fCurrentFrame.evaluateJavaScript("icongui.setPortValue('%s', %f)" % (symbol, value))

    def dspProgramChanged(self, index):
        return

    def dspMidiProgramChanged(self, bank, program):
        return

    def dspStateChanged(self, key, value):
        return

    def dspNoteReceived(self, onOff, channel, note, velocity):
        return

    # --------------------------------------------------------------------------------------------------------

    def uiShow(self):
        if self.fSizeSetup:
            self.show()
        else:
            self.fNeedsShow = True

    def uiFocus(self):
        if not self.fSizeSetup:
            return

        self.setWindowState((self.windowState() & ~Qt.WindowMinimized) | Qt.WindowActive)
        self.show()

        self.raise_()
        self.activateWindow()

    def uiHide(self):
        self.hide()

    def uiQuit(self):
        self.closeExternalUI()
        self.close()
        app.quit()

    def uiTitleChanged(self, uiTitle):
        self.setWindowTitle(uiTitle)

    # --------------------------------------------------------------------------------------------------------
    # Qt events

    def closeEvent(self, event):
        self.closeExternalUI()
        QMainWindow.closeEvent(self, event)

    def timerEvent(self, event):
        if event.timerId() == self.fIdleTimer:
            self.idleStuff()

        QMainWindow.timerEvent(self, event)

    # --------------------------------------------------------------------------------------------------------

    @pyqtSlot()
    def slot_handleSIGTERM(self):
        print("Got SIGTERM -> Closing now")
        self.close()

    # --------------------------------------------------------------------------------------------------------
    # Internal stuff

    def readlineblock(self):
        if self.fPipeClient is None:
            return ""

        return gCarla.utils.pipe_client_readlineblock(self.fPipeClient, 5000)

    def send(self, lines):
        if self.fPipeClient is None or len(lines) == 0:
            return

        gCarla.utils.pipe_client_lock(self.fPipeClient)

        # this must never fail, we need to unlock at the end
        try:
            for line in lines:
                if line is None:
                    line2 = "(null)"
                elif isinstance(line, str):
                    line2 = line.replace("\n", "\r")
                elif isinstance(line, bool):
                    line2 = "true" if line else "false"
                elif isinstance(line, int):
                    line2 = "%i" % line
                elif isinstance(line, float):
                    line2 = "%.10f" % line
                else:
                    print("unknown data type to send:", type(line))
                    return

                gCarla.utils.pipe_client_write_msg(self.fPipeClient, line2 + "\n")
        except:
            pass

        gCarla.utils.pipe_client_flush_and_unlock(self.fPipeClient)

# ------------------------------------------------------------------------------------------------------------
# Main

if __name__ == '__main__':
    # -------------------------------------------------------------
    # Read CLI args

    if len(sys.argv) < 3:
        print("usage: %s <plugin-uri> <ui-uri>" % sys.argv[0])
        sys.exit(1)

    libPrefix = os.getenv("CARLA_LIB_PREFIX")

    # -------------------------------------------------------------
    # App initialization

    app = CarlaApplication("Carla2-MODGUI", libPrefix)

    # -------------------------------------------------------------
    # Init utils

    pathBinaries, pathResources = getPaths(libPrefix)

    utilsname = "libcarla_utils.%s" % (DLL_EXTENSION)

    gCarla.utils = CarlaUtils(os.path.join(pathBinaries, utilsname))
    gCarla.utils.set_process_name("carla-bridge-lv2-modgui")

    # -------------------------------------------------------------
    # Set-up custom signal handling

    setUpSignals()

    # -------------------------------------------------------------
    # Create GUI

    gui = HostWindow()

    # --------------------------------------------------------------------------------------------------------
    # App-Loop

    app.exit_exec()

# ------------------------------------------------------------------------------------------------------------
