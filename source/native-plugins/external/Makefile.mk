#!/usr/bin/make -f
# Makefile for native-plugins #
# --------------------------- #
# Created by falkTX
#

# ----------------------------------------------------------------------------------------------------------------------
# Flags for Zita UIs

ifeq ($(EXPERIMENTAL_PLUGINS),true)
ZITA_DSP_CXX_FLAGS  = $(BUILD_CXX_FLAGS) -Wno-unused-parameter
ZITA_DSP_CXX_FLAGS += $(shell pkg-config --cflags fftw3f)
ZITA_UI_CXX_FLAGS   = $(BUILD_CXX_FLAGS) -Wno-ignored-qualifiers -Wno-unused-parameter -Wno-unused-result
ZITA_UI_CXX_FLAGS  += $(shell pkg-config --cflags cairo libpng12 freetype2 x11 xft zlib)
ZITA_UI_LINK_FLAGS  = $(LINK_FLAGS) -lclxclient -lclthreads
ZITA_UI_LINK_FLAGS += $(shell pkg-config --libs cairo libpng12 freetype2 x11 xft zlib)
ZITA_UI_LINK_FLAGS += -ldl -lpthread -lrt
endif # EXPERIMENTAL_PLUGINS

# ----------------------------------------------------------------------------------------------------------------------
# Flags for ZynAddSubFX

ifeq ($(HAVE_ZYN_DEPS),true)
ZYN_CXX_FLAGS  = $(BUILD_CXX_FLAGS) -Izynaddsubfx -Izynaddsubfx/rtosc
ZYN_CXX_FLAGS += -Wno-misleading-indentation -Wno-shift-negative-value -fpermissive
ZYN_CXX_FLAGS += $(shell pkg-config --cflags fftw3 mxml zlib)
ZYN_LD_FLAGS   = $(LINK_FLAGS)
ZYN_LD_FLAGS  += $(shell pkg-config --libs liblo)
ZYN_LD_FLAGS  += -ldl -lpthread
ifneq ($(WIN32),true)
ZYN_CXX_FLAGS += -DHAVE_ASYNC
endif
ifeq ($(HAVE_X11),true)
ZYN_CXX_FLAGS += $(shell pkg-config --cflags x11)
ZYN_LD_FLAGS  += $(shell pkg-config --libs x11)
endif
ifeq ($(HAVE_ZYN_UI_DEPS),true)
ifeq ($(HAVE_NTK),true)
FLUID          = ntk-fluid
ZYN_CXX_FLAGS += -DNTK_GUI
ZYN_CXX_FLAGS += $(shell pkg-config --cflags ntk_images ntk)
ZYN_LD_FLAGS  += $(shell pkg-config --libs ntk_images ntk)
else # HAVE_NTK
FLUID          = fluid
ZYN_CXX_FLAGS += -DFLTK_GUI
ZYN_CXX_FLAGS += $(shell fltk-config --use-images --cxxflags)
ZYN_LD_FLAGS  += $(shell fltk-config --use-images --ldflags)
endif # HAVE_NTK
ifeq ($(LINUX),true)
ZYN_LD_FLAGS  += -lrt
else
ZYN_LD_FLAGS  += $(MODULEDIR)/juce_core.a
endif
else  # HAVE_ZYN_UI_DEPS
ZYN_CXX_FLAGS += -DNO_UI
endif # HAVE_ZYN_UI_DEPS
endif # HAVE_ZYN_DEPS

# ----------------------------------------------------------------------------------------------------------------------
# DISTRHO plugins

OBJS += \
	$(OBJDIR)/distrho-3bandeq.cpp.o \
	$(OBJDIR)/distrho-3bandsplitter.cpp.o \
	$(OBJDIR)/distrho-kars.cpp.o \
	$(OBJDIR)/distrho-nekobi.cpp.o \
	$(OBJDIR)/distrho-pingpongpan.cpp.o

ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_PROJECTM),true)
OBJS += $(OBJDIR)/distrho-prom.cpp.o
endif
endif

# ----------------------------------------------------------------------------------------------------------------------
# DISTRHO plugins (Juice)

OBJS += \
	$(OBJDIR)/distrho-vectorjuice.cpp.o \
	$(OBJDIR)/distrho-wobblejuice.cpp.o

# ----------------------------------------------------------------------------------------------------------------------
# ZynAddSubFX

ifeq ($(HAVE_ZYN_DEPS),true)
OBJS += \
	$(OBJDIR)/zynaddsubfx-fx.cpp.o \
	$(OBJDIR)/zynaddsubfx-synth.cpp.o \
	$(OBJDIR)/zynaddsubfx-src.cpp.o

ifeq ($(HAVE_ZYN_UI_DEPS),true)
TARGETS += resources/zynaddsubfx-ui$(APP_EXT)

ZYN_UI_FILES_CPP = \
	zynaddsubfx/UI/ADnoteUI.cpp \
	zynaddsubfx/UI/BankUI.cpp \
	zynaddsubfx/UI/ConfigUI.cpp \
	zynaddsubfx/UI/EffUI.cpp \
	zynaddsubfx/UI/EnvelopeUI.cpp \
	zynaddsubfx/UI/FilterUI.cpp \
	zynaddsubfx/UI/LFOUI.cpp \
	zynaddsubfx/UI/MasterUI.cpp \
	zynaddsubfx/UI/MicrotonalUI.cpp \
	zynaddsubfx/UI/OscilGenUI.cpp \
	zynaddsubfx/UI/PADnoteUI.cpp \
	zynaddsubfx/UI/PartUI.cpp \
	zynaddsubfx/UI/PresetsUI.cpp \
	zynaddsubfx/UI/ResonanceUI.cpp \
	zynaddsubfx/UI/SUBnoteUI.cpp \
	zynaddsubfx/UI/VirKeyboard.cpp

ZYN_UI_FILES_H = \
	zynaddsubfx/UI/ADnoteUI.h \
	zynaddsubfx/UI/BankUI.h \
	zynaddsubfx/UI/ConfigUI.h \
	zynaddsubfx/UI/EffUI.h \
	zynaddsubfx/UI/EnvelopeUI.h \
	zynaddsubfx/UI/FilterUI.h \
	zynaddsubfx/UI/LFOUI.h \
	zynaddsubfx/UI/MasterUI.h \
	zynaddsubfx/UI/MicrotonalUI.h \
	zynaddsubfx/UI/OscilGenUI.h \
	zynaddsubfx/UI/PADnoteUI.h \
	zynaddsubfx/UI/PartUI.h \
	zynaddsubfx/UI/PresetsUI.h \
	zynaddsubfx/UI/ResonanceUI.h \
	zynaddsubfx/UI/SUBnoteUI.h \
	zynaddsubfx/UI/VirKeyboard.h
endif
endif

# ----------------------------------------------------------------------------------------------------------------------
# Experimental plugins

ifeq ($(EXPERIMENTAL_PLUGINS),true)
OBJS += \
	$(OBJDIR)/zita-at1.cpp.o \
	$(OBJDIR)/zita-bls1.cpp.o \
	$(OBJDIR)/zita-rev1.cpp.o

TARGETS += \
	bin/at1-ui$(APP_EXT) \
	bin/bls1-ui$(APP_EXT) \
	bin/rev1-ui$(APP_EXT)
endif

# ----------------------------------------------------------------------------------------------------------------------

bin/at1-ui$(APP_EXT): $(OBJDIR)/zita-at1-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking at1-ui"
	@$(CXX) $^ $(ZITA_UI_LINK_FLAGS) -o $@

bin/bls1-ui$(APP_EXT): $(OBJDIR)/zita-bls1-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking bls1-ui"
	@$(CXX) $^ $(ZITA_UI_LINK_FLAGS) -o $@

bin/rev1-ui$(APP_EXT): $(OBJDIR)/zita-rev1-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking rev1-ui"
	@$(CXX) $^ $(ZITA_UI_LINK_FLAGS) -o $@

bin/zynaddsubfx-ui$(APP_EXT): $(OBJDIR)/zynaddsubfx-ui.cpp.o
	-@mkdir -p $(OBJDIR)
	@echo "Linking zynaddsubfx-ui"
	@$(CXX) $^ $(ZYN_LD_FLAGS) -o $@

# ----------------------------------------------------------------------------------------------------------------------

zynaddsubfx/UI/%.cpp: zynaddsubfx/UI/%.fl
	@echo "Generating $@|h"
	@$(FLUID) -c -o zynaddsubfx/UI/$*.cpp -h zynaddsubfx/UI/$*.h $<

zynaddsubfx/UI/%.h: zynaddsubfx/UI/%.fl
	@echo "Generating $@|cpp"
	@$(FLUID) -c -o zynaddsubfx/UI/$*.cpp -h zynaddsubfx/UI/$*.h $<

# ----------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/distrho-3bandeq.cpp.o: distrho-3bandeq.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_3BandEQ -Idistrho-3bandeq -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-3bandsplitter.cpp.o: distrho-3bandsplitter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_3BandSplitter -Idistrho-3bandsplitter -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-kars.cpp.o: distrho-kars.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_Kars -Idistrho-kars -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-nekobi.cpp.o: distrho-nekobi.cpp
	-@mkdir -p $(OBJDIR)
	# FIXME - fix nekobi strict warnings
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_Nekobi -Idistrho-nekobi -I$(CWD)/modules/dgl -w -c -o $@

$(OBJDIR)/distrho-pingpongpan.cpp.o: distrho-pingpongpan.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_PingPongPan -Idistrho-pingpongpan -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-prom.cpp.o: distrho-prom.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) $(PROJECTM_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_ProM -Idistrho-prom -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

# ----------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/distrho-vectorjuice.cpp.o: distrho-vectorjuice.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_VectorJuice -Idistrho-vectorjuice -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

$(OBJDIR)/distrho-wobblejuice.cpp.o: distrho-wobblejuice.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -DDISTRHO_NAMESPACE=DISTRHO_WobbleJuice -Idistrho-wobblejuice -I$(CWD)/modules/dgl -Wno-effc++ -c -o $@

# ----------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/zynaddsubfx-fx.cpp.o: zynaddsubfx-fx.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -c -o $@

$(OBJDIR)/zynaddsubfx-synth.cpp.o: zynaddsubfx-synth.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -Wno-unused-parameter -c -o $@

$(OBJDIR)/zynaddsubfx-src.cpp.o: zynaddsubfx-src.cpp $(ZYN_UI_FILES_H)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -Wno-unused-parameter -Wno-unused-variable -c -o $@

$(OBJDIR)/zynaddsubfx-ui.cpp.o: zynaddsubfx-ui.cpp $(ZYN_UI_FILES_H) $(ZYN_UI_FILES_CPP)
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZYN_CXX_FLAGS) -Wno-unused-parameter -Wno-unused-variable -c -o $@

# ----------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/zita-%-ui.cpp.o: zita-%-ui.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZITA_UI_CXX_FLAGS) -c -o $@

$(OBJDIR)/zita-%.cpp.o: zita-%.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(ZITA_DSP_CXX_FLAGS) -c -o $@

# ----------------------------------------------------------------------------------------------------------------------

-include $(OBJDIR)/zita-at1-ui.cpp.d
-include $(OBJDIR)/zita-bls1-ui.cpp.d
-include $(OBJDIR)/zita-rev1-ui.cpp.d
-include $(OBJDIR)/zynaddsubfx-ui.cpp.d

# ----------------------------------------------------------------------------------------------------------------------
