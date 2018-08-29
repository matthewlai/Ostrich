##
## This file is part of the Ostrich project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
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

LIBRARY = ostrich

# .c, .cpp, and .cxx files
SRCS = $(wildcard src/*.c) $(wildcard src/*.cpp) $(wildcard src/*.cxx) $(wildcard src/usb/*.cpp)

# Directories containing header files
INCLUDE = include

# Chip part number (this is mostly unused for building a library, but defines
# a few useful constants). We can use any stm32f7 part number here.
DEVICE = stm32f767zit6u

ifeq ($(strip $(OSTRICH_PATH)),)
$(error OSTRICH_PATH undefined!)
endif

MODE = ostrich
include $(OSTRICH_PATH)/Makefiles/Makefile.stm32f7
