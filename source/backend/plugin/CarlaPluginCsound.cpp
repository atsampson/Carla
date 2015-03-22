/*
 * Carla CSound Plugin
 * Copyright (C) 2015 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "CarlaPluginInternal.hpp"
#include "CarlaEngine.hpp"

#ifdef HAVE_CSOUND

#include <csound.hpp>

CARLA_BACKEND_START_NAMESPACE

// -----------------------------------------------------

static bool sInitialized = false;

class CarlaPluginCsound : public CarlaPlugin
{
public:
    CarlaPluginCsound(CarlaEngine* const engine, const uint id) noexcept
        : CarlaPlugin(engine, id),
          fCsound(nullptr),
          leakDetector_CarlaPluginCsound()
    {
        carla_debug("CarlaPluginCsound::CarlaPluginCsound(%p, %i)", engine, id);

        if (! sInitialized)
        {
            sInitialized = true;
            CARLA_SAFE_ASSERT(csoundInitialize(CSOUNDINIT_NO_SIGNAL_HANDLER|CSOUNDINIT_NO_ATEXIT) == 0);
        }

        fCsound = csoundCreate(this);
        CARLA_SAFE_ASSERT_RETURN(fCsound != nullptr,);

        csoundSetHostImplementedMIDIIO(fCsound, 1);

        csoundSetExternalMidiInOpenCallback(fCsound, nullptr);
        csoundSetExternalMidiReadCallback(fCsound, nullptr);
        csoundSetExternalMidiInCloseCallback(fCsound, nullptr);

        csoundSetExternalMidiOutOpenCallback(fCsound, nullptr);
        csoundSetExternalMidiWriteCallback(fCsound, nullptr);
        csoundSetExternalMidiOutCloseCallback(fCsound, nullptr);
    }

    ~CarlaPluginCsound() noexcept override
    {
        carla_debug("CarlaPluginCsound::~CarlaPluginCsound()");

        pData->singleMutex.lock();
        pData->masterMutex.lock();

        if (pData->client != nullptr && pData->client->isActive())
            pData->client->deactivate();

        if (pData->active)
        {
            deactivate();
            pData->active = false;
        }

        if (fCsound != nullptr)
        {
            csoundDestroy(fCsound);
            fCsound = nullptr;
        }

        clearBuffers();
    }

    // -------------------------------------------------------------------
    // Information (base)

    PluginType getType() const noexcept override
    {
        return PLUGIN_CSOUND;
    }

    // -------------------------------------------------------------------
    // Information (count)

    // nothing

    // -------------------------------------------------------------------
    // Information (current data)

    // nothing

    // -------------------------------------------------------------------
    // Information (per-plugin data)

    uint getOptionsAvailable() const noexcept override
    {
        uint options = 0x0;

        options |= PLUGIN_OPTION_MAP_PROGRAM_CHANGES;
        options |= PLUGIN_OPTION_SEND_CONTROL_CHANGES;
        options |= PLUGIN_OPTION_SEND_CHANNEL_PRESSURE;
        options |= PLUGIN_OPTION_SEND_PITCHBEND;
        options |= PLUGIN_OPTION_SEND_ALL_SOUND_OFF;

        return options;
    }

    //float getParameterValue(const uint32_t parameterId) const noexcept override

#if 0
    void getLabel(char* const strBuf) const noexcept override
    {
        CARLA_SAFE_ASSERT_RETURN(fDescriptor        != nullptr, nullStrBuf(strBuf));
        CARLA_SAFE_ASSERT_RETURN(fDescriptor->Label != nullptr, nullStrBuf(strBuf));

        std::strncpy(strBuf, fDescriptor->Label, STR_MAX);
    }

    void getMaker(char* const strBuf) const noexcept override
    {
        CARLA_SAFE_ASSERT_RETURN(fDescriptor        != nullptr, nullStrBuf(strBuf));
        CARLA_SAFE_ASSERT_RETURN(fDescriptor->Maker != nullptr, nullStrBuf(strBuf));

        if (fRdfDescriptor != nullptr && fRdfDescriptor->Creator != nullptr)
        {
            std::strncpy(strBuf, fRdfDescriptor->Creator, STR_MAX);
            return;
        }

        std::strncpy(strBuf, fDescriptor->Maker, STR_MAX);
    }

    void getCopyright(char* const strBuf) const noexcept override
    {
        CARLA_SAFE_ASSERT_RETURN(fDescriptor            != nullptr, nullStrBuf(strBuf));
        CARLA_SAFE_ASSERT_RETURN(fDescriptor->Copyright != nullptr, nullStrBuf(strBuf));

        std::strncpy(strBuf, fDescriptor->Copyright, STR_MAX);
    }

    void getRealName(char* const strBuf) const noexcept override
    {
        CARLA_SAFE_ASSERT_RETURN(fDescriptor       != nullptr, nullStrBuf(strBuf));
        CARLA_SAFE_ASSERT_RETURN(fDescriptor->Name != nullptr, nullStrBuf(strBuf));

        if (fRdfDescriptor != nullptr && fRdfDescriptor->Title != nullptr)
        {
            std::strncpy(strBuf, fRdfDescriptor->Title, STR_MAX);
            return;
        }

        std::strncpy(strBuf, fDescriptor->Name, STR_MAX);
    }
#endif

    //void getParameterName(const uint32_t parameterId, char* const strBuf) const noexcept override

    // -------------------------------------------------------------------
    // Set data (state)

    // nothing

    // -------------------------------------------------------------------
    // Set data (internal stuff)

    // nothing

    // -------------------------------------------------------------------
    // Set data (plugin-specific stuff)

    //void setParameterValue(const uint32_t parameterId, const float value, const bool sendGui, const bool sendOsc, const bool sendCallback) noexcept override

    // -------------------------------------------------------------------
    // Misc

    // nothing

    // -------------------------------------------------------------------
    // Plugin state

    void reload() override
    {
        CARLA_SAFE_ASSERT_RETURN(pData->engine != nullptr,);
        //CARLA_SAFE_ASSERT_RETURN(fDescriptor != nullptr,);
        carla_debug("CarlaPluginCsound::reload() - start");

        const EngineProcessMode processMode(pData->engine->getProccessMode());

        // Safely disable plugin for reload
        const ScopedDisabler sd(this);

        if (pData->active)
            deactivate();

        clearBuffers();

        uint32_t aIns, aOuts, params;
        aIns = aOuts = params = 0;

        bool needsCtrlIn, needsCtrlOut;
        needsCtrlIn = needsCtrlOut = false;

        // TODO - get params

        if (aIns > 0)
        {
            pData->audioIn.createNew(aIns);
        }

        if (aOuts > 0)
        {
            pData->audioOut.createNew(aOuts);
            needsCtrlIn = true;
        }

        if (params > 0)
        {
            pData->param.createNew(params, true);

            //fParamBuffers = new float[params];
            //FloatVectorOperations::clear(fParamBuffers, static_cast<int>(params));
        }

        const uint portNameSize(pData->engine->getMaxPortNameSize());
        CarlaString portName;

        if (needsCtrlIn)
        {
            portName.clear();

            if (processMode == ENGINE_PROCESS_MODE_SINGLE_CLIENT)
            {
                portName  = pData->name;
                portName += ":";
            }

            portName += "events-in";
            portName.truncate(portNameSize);

            pData->event.portIn = (CarlaEngineEventPort*)pData->client->addPort(kEnginePortTypeEvent, portName, true);
        }

        if (needsCtrlOut)
        {
            portName.clear();

            if (processMode == ENGINE_PROCESS_MODE_SINGLE_CLIENT)
            {
                portName  = pData->name;
                portName += ":";
            }

            portName += "events-out";
            portName.truncate(portNameSize);

            pData->event.portOut = (CarlaEngineEventPort*)pData->client->addPort(kEnginePortTypeEvent, portName, false);
        }

        // plugin hints
        pData->hints = 0x0;

#ifndef BUILD_BRIDGE
        if (aOuts > 0 && (aIns == aOuts || aIns == 1))
            pData->hints |= PLUGIN_CAN_DRYWET;

        if (aOuts > 0)
            pData->hints |= PLUGIN_CAN_VOLUME;

        if (aOuts >= 2 && aOuts % 2 == 0)
            pData->hints |= PLUGIN_CAN_BALANCE;
#endif

        // extra plugin hints
        pData->extraHints  = 0x0;
        pData->extraHints |= PLUGIN_EXTRA_HINT_CAN_RUN_RACK;

        bufferSizeChanged(pData->engine->getBufferSize());

        if (pData->active)
            activate();

        carla_debug("CarlaPluginCsound::reload() - end");
    }

    // -------------------------------------------------------------------
    // Plugin processing

    void activate() noexcept override
    {
    }

    void deactivate() noexcept override
    {
    }

    void process(const float** const audioIn, float** const audioOut, const float** const, float** const, const uint32_t frames) override
    {
        // --------------------------------------------------------------------------------------------------------
        // Check if active

        if (! pData->active)
        {
            // disable any output sound
            for (uint32_t i=0; i < pData->audioOut.count; ++i)
                FloatVectorOperations::clear(audioOut[i], static_cast<int>(frames));
            return;
        }
    }

    //void bufferSizeChanged(const uint32_t newBufferSize) override

    //void sampleRateChanged(const double newSampleRate) override

    // -------------------------------------------------------------------
    // Plugin buffers

    //void clearBuffers() noexcept override

    // -------------------------------------------------------------------

    void* getNativeHandle() const noexcept override
    {
        return nullptr; // fHandle;
    }

//     const void* getNativeDescriptor() const noexcept override
//     {
//         return fDescriptor;
//     }
//
//     const void* getExtraStuff() const noexcept override
//     {
//         return fRdfDescriptor;
//     }

    // -------------------------------------------------------------------

    bool init(const char* const filename, const char* const name, const char* const label)
    {
        CARLA_SAFE_ASSERT_RETURN(pData->engine != nullptr, false);

        // ---------------------------------------------------------------
        // first checks

        if (pData->client != nullptr)
        {
            pData->engine->setLastError("Plugin client is already registered");
            return false;
        }

        if (fCsound == nullptr)
        {
            pData->engine->setLastError("Csound failed to initialize");
            return false;
        }

        if (filename == nullptr || filename[0] == '\0')
        {
            pData->engine->setLastError("null filename");
            return false;
        }

        // ---------------------------------------------------------------
        // Get info

        pData->filename = carla_strdup(filename);

        if (name != nullptr && name[0] != '\0')
            pData->name = pData->engine->getUniquePluginName(name);
        else if (label != nullptr && label[0] != '\0')
            pData->name = pData->engine->getUniquePluginName(label);
        else
            pData->name = pData->engine->getUniquePluginName("csound");

        // ---------------------------------------------------------------
        // register client

        pData->client = pData->engine->addClient(this);

        if (pData->client == nullptr || ! pData->client->isOk())
        {
            pData->engine->setLastError("Failed to register plugin client");
            return false;
        }

        // ---------------------------------------------------------------
        // set default options

        pData->options  = 0x0;
        pData->options |= PLUGIN_OPTION_SEND_CONTROL_CHANGES;
        pData->options |= PLUGIN_OPTION_SEND_CHANNEL_PRESSURE;
        pData->options |= PLUGIN_OPTION_SEND_NOTE_AFTERTOUCH;
        pData->options |= PLUGIN_OPTION_SEND_PITCHBEND;
        pData->options |= PLUGIN_OPTION_SEND_ALL_SOUND_OFF;

        return true;
    }

private:
    CSOUND* fCsound;

    CARLA_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CarlaPluginCsound)
};

CARLA_BACKEND_END_NAMESPACE

#endif // HAVE_CSOUND

CARLA_BACKEND_START_NAMESPACE

// -------------------------------------------------------------------------------------------------------------------

CarlaPlugin* CarlaPlugin::newCsound(const Initializer& init)
{
    carla_debug("CarlaPlugin::newCsound({%p, \"%s\", \"%s\", \"%s\", " P_INT64 ", %x})", init.engine, init.filename, init.name, init.label, init.uniqueId, init.options);

#ifdef HAVE_CSOUND
    CarlaPluginCsound* const plugin(new CarlaPluginCsound(init.engine, init.id));

    if (! plugin->init(init.filename, init.name, init.label))
    {
        delete plugin;
        return nullptr;
    }

    return plugin;
#else
    init.engine->setLastError("Csound support not available");
    return nullptr;
#endif
}

// -------------------------------------------------------------------------------------------------------------------

CARLA_BACKEND_END_NAMESPACE
