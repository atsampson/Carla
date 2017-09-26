#!/usr/bin/make -f
# Makefile for native-plugins #
# --------------------------- #
# Created by falkTX
#

CWD=..
MODULENAME=native-plugins
include ../modules/Makefile.mk

# TODO
# rm ZYN_UI_FILES_H
# each plugin with one makefile?
# UIs in bin dir
# adjust main makefile install
# build object files in separate dirs, so generic c++ rule on this file is possible
# add '-I$(CWD)/modules/distrho' to sub makefile?
# add '-DWITH_EXT_PLUGINS' to sub make

# ----------------------------------------------------------------------------------------------------------------------

BUILD_C_FLAGS   += -I..
BUILD_CXX_FLAGS += -I.. -isystem $(CWD)/modules

# ----------------------------------------------------------------------------------------------------------------------
# Set targets

TARGETS = $(MODULEDIR)/$(MODULENAME).a

# ----------------------------------------------------------------------------------------------------------------------
# Set objects

OBJS = \
	$(OBJDIR)/_all.c.o \
	$(OBJDIR)/_data.cpp.o

# ----------------------------------------------------------------------------------------------------------------------
# Simple plugins

OBJS += \
	$(OBJDIR)/bypass.c.o \
	$(OBJDIR)/lfo.c.o \
	$(OBJDIR)/midi-channel-filter.c.o \
	$(OBJDIR)/midi-gain.c.o \
	$(OBJDIR)/midi-join.c.o \
	$(OBJDIR)/midi-split.c.o \
	$(OBJDIR)/midi-through.c.o \
	$(OBJDIR)/midi-transpose.c.o

OBJS += \
	$(OBJDIR)/audio-file.cpp.o \
	$(OBJDIR)/bigmeter.cpp.o \
	$(OBJDIR)/midi-file.cpp.o \
	$(OBJDIR)/midi-pattern.cpp.o \
	$(OBJDIR)/notes.cpp.o

# ----------------------------------------------------------------------------------------------------------------------
# Include external plugins, if present

ifeq ($(HAVE_EXTERNAL_PLUGINS),true)
include external/Makefile.mk
endif

# ----------------------------------------------------------------------------------------------------------------------

all: $(TARGETS)

# ----------------------------------------------------------------------------------------------------------------------

clean:
	rm -f $(OBJDIR)/*.o $(MODULEDIR)/$(MODULENAME)*.a $(ZYN_UI_FILES_H) $(ZYN_UI_FILES_CPP) $(TARGETS)

debug:
	$(MAKE) DEBUG=true

# ----------------------------------------------------------------------------------------------------------------------

$(MODULEDIR)/$(MODULENAME).a: $(OBJS)
	-@mkdir -p $(MODULEDIR)
	@echo "Creating $(MODULENAME).a"
	@rm -f $@
	@$(AR) crs $@ $^

# ----------------------------------------------------------------------------------------------------------------------

$(OBJDIR)/%.c.o: %.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CC) $< $(BUILD_C_FLAGS) -c -o $@

$(OBJDIR)/%.cpp.o: %.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling $<"
	@$(CXX) $< $(BUILD_CXX_FLAGS) -c -o $@

# ----------------------------------------------------------------------------------------------------------------------

-include $(OBJS:%.o=%.d)

# ----------------------------------------------------------------------------------------------------------------------
