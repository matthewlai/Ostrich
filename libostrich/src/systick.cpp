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

#include "systick.h"

#include <sys/time.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "ostrich.h"

namespace Ostrich {

void InitSystick() {
  systick_set_reload(g_systick_period);
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_clear();
  systick_counter_enable();
  systick_interrupt_enable();
}

uint64_t GetTimeClocks() {
  uint64_t num_reloads;
  uint32_t systick_val;

  // Clear the countflag first, so we can check if an interrupt happened
  // while we are doing the math here.
  systick_get_countflag();

  do {
    // Read high before low, because the ISR writes high before low.
    uint32_t num_reloads_high;
    uint32_t num_reloads_low;

    do {
      num_reloads_high = g_systick_reloads_high;
      num_reloads_low = g_systick_reloads_low;

      // Make sure reloads high hasn't changed.
    } while (num_reloads_high != g_systick_reloads_high);

  	num_reloads = num_reloads_high;
    num_reloads <<= 32;
    num_reloads |= num_reloads_low;
  	systick_val = systick_get_value();

    // If an interrupt did happen, we do everything all over again.
  } while (systick_get_countflag());

  uint64_t clocks = num_reloads * g_systick_period + (g_systick_period - systick_val);
  return clocks;
}

uint64_t GetTimeMilliseconds() {
  return GetTimeClocks() * 1000 / g_ahb_freq;
}

}; // namespace Ostrich

extern "C" {
void sys_tick_handler(void) {
  if (Ostrich::g_systick_reloads_low == 0xFFFFFFFF) {
    ++Ostrich::g_systick_reloads_high;
    Ostrich::g_systick_reloads_low = 0;
  } else {
    ++Ostrich::g_systick_reloads_low;
  }
}

// Use GetTimeClocks to implement gettimeofday
int _gettimeofday(struct timeval* tp, void* tzp)
{
  (void)tzp;
  uint64_t now = Ostrich::GetTimeClocks() * 1000000 / Ostrich::g_ahb_freq;
  tp->tv_sec = now / 1000000;
  tp->tv_usec = now - (tp->tv_sec * 1000000);
  return 0;
}
}


