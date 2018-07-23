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

#ifndef __OSTRICH_H__
#define __OSTRICH_H__

#include "gpio.h"

#include <cstdint>

#include <libopencm3/stm32/rcc.h>

namespace Ostrich {

extern uint32_t g_ahb_freq;
extern uint32_t g_apb1_freq;
extern uint32_t g_apb2_freq;

struct InitInfo {
  // Clock settings. See libopencm3/lib/stm32/*/rcc.c.
  rcc_clock_scale clock_scale;

  // Use high speed external oscillator.
  bool use_hse; 

  // High speed external oscillator frequency. Unused if use_hse is not set.
  uint8_t hse_mhz;

  // Timing resolution in nanoseconds. Reasonable values depend on AHB clock
  // frequency (usually the same as CPU clock frequency). It should be at least
  // hundreds of CPU cycles. For example, at 200 MHz, cycle time is 5 ns, and
  // period of 1 us would be the upper-bound of reasonable (otherwise the CPU
  // will spend a significant portion of the time servicing systick interrupts).
  uint64_t systick_period_ns;
};

inline void Init(const InitInfo& ii) {
  if (ii.use_hse) {
    rcc_clock_setup_hse(&ii.clock_scale, ii.hse_mhz);
  } else {
    rcc_clock_setup_hsi(&ii.clock_scale);
  }

  g_ahb_freq = ii.clock_scale.ahb_frequency;
  g_apb1_freq = ii.clock_scale.apb1_frequency;
  g_apb2_freq = ii.clock_scale.apb2_frequency;
}

}; // namespace Ostrich

#endif // __OSTRICH_H__
