/*
 * This file is part of the libostrich project.
 *
 * Copyright (C) 2018 Matthew Lai <m@matthewlai.ca>
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
#include <libopencm3/cm3/nvic.h>

namespace Ostrich {

extern uint32_t g_ahb_freq;
extern uint32_t g_apb1_freq;
extern uint32_t g_apb2_freq;
extern uint32_t g_systick_period;

extern volatile uint32_t g_systick_reloads_high;
extern volatile uint32_t g_systick_reloads_low;

struct BoardConfig {
  // Clock settings. See libopencm3/lib/stm32/*/rcc.c.
  rcc_clock_scale clock_scale;

  // Use high speed external oscillator.
  bool use_hse; 

  // High speed external oscillator frequency. Unused if use_hse is not set.
  uint8_t hse_mhz;

  // How often we do housekeeping in AHB (usually the same as CPU) clock cycles.
  // Reasonable values are in the hundreds, so the CPU doesn't spend excessive
  // amount of time servicing systick interrupts.
  uint64_t systick_period_clocks;
};

// This should be defined by the user.
BoardConfig MakeBoardConfig();

inline void WaitForInterrupt() {
  __asm__("wfi");
}

}; // namespace Ostrich

extern "C" {
// Here we have to make sure InitOstrich is called before main, and before any
// other Ostrich objects are static initialized. We use GCC's extensions for
// this, but this is complicated by the fact that libostrich is a static
// library, and if the user doesn't refer to this symbol, it will be
// thrown away by the linker!
// We use the linker flag "-Wl,--undefined=InitOstrich" to work around that.
void InitOstrich() __attribute__((constructor(1000)));
}

#endif // __OSTRICH_H__
