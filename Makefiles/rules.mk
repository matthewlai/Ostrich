##
## This file is part of the Ostrich project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
## Copyright (C) 2017 Matthew Lai <m@matthewlai.ca>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

LIBNAME         = opencm3_stm32f7
OSTRICH_LIBNAME	= ostrich
DEFS            += -DSTM32F7

FP_FLAGS        ?= -mfloat-abi=hard -mfpu=fpv5-d16
ARCH_FLAGS      = -mthumb -mcpu=cortex-m7 $(FP_FLAGS)

###############################################################################
# Executables

PREFIX		?= arm-none-eabi

CC		:= $(PREFIX)-gcc
CXX		:= $(PREFIX)-g++
LD		:= $(PREFIX)-g++
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
GDB		?= $(PREFIX)-gdb
OPT		?= -O2
CSTD		?= -std=c99
CXXSTD		?= -std=gnu++14


###############################################################################
# Source files
SRCS_BASE	:= $(basename $(SRCS))
OBJS		:= $(SRCS_BASE:src/%=obj/%.o)

INCLUDE 	+= $(OSTRICH_PATH)/libopencm3/include
INCLUDE         += $(OSTRICH_PATH)/libostrich/include
INCLUDES	:= $(INCLUDE:%=-I%)

OPENCM3_DIR 	:= $(OSTRICH_PATH)/libopencm3

# Path to the static library for our target
LIBPATH		= $(OSTRICH_PATH)/libopencm3/lib/lib$(LIBNAME).a
OSTRICH_LIBPATH = $(OSTRICH_PATH)/libostrich/lib$(OSTRICH_LIBNAME).a

ifneq ($(MAKECMDGOALS), clean)
ifeq ($(wildcard $(LIBPATH)),)
$(info lib$(LIBNAME).a not found. Building libopencm3. \
	This will take a while, but only has to be done once.)
# Make sure libraries are up to date
$(shell cd $(OPENCM3_DIR) && make 1>&2)
endif

ifeq ($(wildcard $(OSTRICH_LIBPATH)),)
ifneq ($(MODE),ostrich)
$(info lib$(OSTRICH_LIBNAME).a not found. Building libostrich.)
# Make sure libraries are up to date
$(shell cd $(OSTRICH_PATH)/libostrich && make 1>&2)
endif
endif

ifneq ($(MODE),ostrich)
LDLIBS		+= -L$(OSTRICH_PATH)/libostrich -l$(OSTRICH_LIBNAME)
endif

endif

ifeq ($(V),1)
$(info Using $(OPENCM3_DIR) path to library)
endif

ifneq ($(MAKECMDGOALS), clean)
include $(OPENCM3_DIR)/mk/genlink-config.mk
endif

###############################################################################
# C flags

TGT_CFLAGS	+= $(OPT) $(CSTD) -g
TGT_CFLAGS	+= $(ARCH_FLAGS)
TGT_CFLAGS	+= $(INCLUDES)
TGT_CFLAGS	+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS	+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C++ flags

TGT_CXXFLAGS	+= $(OPT) $(CXXSTD) -g
TGT_CXXFLAGS	+= $(ARCH_FLAGS)
TGT_CXXFLAGS	+= $(INCLUDES)
TGT_CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls
TGT_CXXFLAGS	+= -fno-threadsafe-statics -fno-use-cxa-atexit
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

TGT_CPPFLAGS	+= -MD
TGT_CPPFLAGS	+= -Wall -Wundef
TGT_CPPFLAGS	+= $(DEFS)

###############################################################################
# Linker flags

TGT_LDFLAGS		+= --static -nostartfiles --specs=nano.specs --specs=nosys.specs 
TGT_LDFLAGS		+= -T$(LDSCRIPT)
TGT_LDFLAGS		+= $(ARCH_FLAGS)
TGT_LDFLAGS		+= -Wl,-Map=$(*).map
TGT_LDFLAGS		+= -Wl,--gc-sections
TGT_LDFLAGS		+= -Wl,--undefined=InitOstrich -Wl,--undefined=otg_fs_isr
ifeq ($(V),99)
TGT_LDFLAGS		+= -Wl,--print-gc-sections
endif

###############################################################################
# Used libraries

LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

###############################################################################
###############################################################################
###############################################################################

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

ifeq ($(MODE),ostrich)
all: library clean_intermediate
else
all: bin clean_intermediate
endif

elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list
library: lib$(LIBRARY).a

images: $(BINARY).images
flash: $(BINARY).flash

$(LDSCRIPT): $(OPENCM3_DIR)/ld/linker.ld.S
	@#printf "  GENLNK  $(DEVICE)\n"
	$(Q)$(CPP) $(ARCH_FLAGS) $(shell $(OPENCM3_DIR)/scripts/genlink.py $(DEVICES_DATA) $(DEVICE) DEFS) -P -E $< > $@

# Define a helper macro for debugging make errors online
# you can type "make print-OPENCM3_DIR" and it will show you
# how that ended up being resolved by all of the included
# makefiles.
print-%:
	@echo $*=$($*)

%.images: %.bin %.hex %.srec %.list %.map
	@#printf "*** $* images generated ***\n"

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@#printf "  OBJCOPY $(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@#printf "  OBJDUMP $(*).list\n"
	$(Q)$(OBJDUMP) -S $(*).elf > $(*).list

%.elf %.map: $(OBJS) $(LDSCRIPT)
	@#printf "  LD      $(*).elf\n"
	$(Q)$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(*).elf

%.a: $(OBJS)
	@#printf "  AR      $(*).a"
	$(Q)$(AR) rcs $@ $(OBJS)

obj/%.o: src/%.c
	@#printf "  CC      $(*).c\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

obj/%.o: src/%.cxx
	@#printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

obj/%.o: src/%.cpp
	@#printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $@ -c $<

clean:
	@#printf "  CLEAN\n"
	$(Q)$(RM) obj/*.o obj/*.d *.a *.elf *.bin *.hex *.srec *.list *.map generated.* ${OBJS} ${OBJS:%.o:%.d}

clean_intermediate:
	@#printf "  CLEAN_INTERMEDIATE\n"
	$(Q)$(RM) *.hex *.srec *.list *.map generated.*

.PHONY: images clean clean_intermediate elf bin hex srec list

-include $(OBJS:.o=.d)
