/*
 * This file is part of the libostrich project.
 *
 * Copyright (C) 2017 Matthew Lai <m@matthewlai.ca>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "ostrich.h"

#include <libopencm3/stm32/rcc.h>

inline void SetupClocks() {
  Ostrich::ClockInfo ci;

  ci.clock_scale = rcc_3v3[RCC_CLOCK_3V3_216MHZ];
  ci.hse_mhz = 8;
  ci.external = true;
  
  Ostrich::SetupClocks(ci);
}

#endif // __BOARD_CONFIG_H__
