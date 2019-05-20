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

#include "ostrich.h"

#include <libopencm3/cm3/systick.h>

#include "systick.h"

namespace Ostrich {

uint32_t g_ahb_freq;
uint32_t g_apb1_freq;
uint32_t g_apb2_freq;
uint32_t g_systick_period;
uint32_t g_vdd_mv;

volatile uint32_t g_systick_reloads_high;
volatile uint32_t g_systick_reloads_low;

ErrorHandler g_error_handler;
LoggingHandler g_logging_handler;

// Default error handler locks up the system.
ErrorHandler error_handler = [](const std::string&) { LockUp(); };

void Init() {
  auto board_config = MakeBoardConfig();

  if (board_config.use_hse) {
    rcc_clock_setup_hse(&board_config.clock_scale, board_config.hse_mhz);
  } else {
    rcc_clock_setup_hsi(&board_config.clock_scale);
  }

  g_ahb_freq = board_config.clock_scale.ahb_frequency;
  g_apb1_freq = board_config.clock_scale.apb1_frequency;
  g_apb2_freq = board_config.clock_scale.apb2_frequency;

  g_systick_period = board_config.systick_period_clocks;

  g_vdd_mv = board_config.vdd_voltage_mV;

  InitSystick();
}
}; // namespace Ostrich

void InitOstrich() {
  Ostrich::Init();
}

// This is needed to keep GCC happy when some C++ classes are used.
void* __dso_handle;
