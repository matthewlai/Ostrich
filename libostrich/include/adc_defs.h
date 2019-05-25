/*
 * This file is part of the libostrich project.
 *
 * Copyright (C) 2019 Matthew Lai <m@matthewlai.ca>
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

#ifndef __ADC_DEFS_H__
#define __ADC_DEFS_H__

#include <cstdint>
#include <utility>

#include <libopencm3/stm32/gpio.h>

#include "gpio_defs.h"

namespace Ostrich {

enum class ChannelType {
  kGPIO, kTempVbatt, kVrefint, kNC
};

struct ChannelInfo {
  ChannelType channel_type;
  GPIOPortPin port_pin;
};

constexpr std::array<ChannelInfo, 19> kADCChannelInfos[3] = {
  {{
    // ADC1
    {ChannelType::kGPIO, PIN_A0},  // Channel 0
    {ChannelType::kGPIO, PIN_A1},  // Channel 1
    {ChannelType::kGPIO, PIN_A2},  // Channel 2
    {ChannelType::kGPIO, PIN_A3},  // Channel 3
    {ChannelType::kGPIO, PIN_A4},  // Channel 4
    {ChannelType::kGPIO, PIN_A5},  // Channel 5
    {ChannelType::kGPIO, PIN_A6},  // Channel 6
    {ChannelType::kGPIO, PIN_A7},  // Channel 7
    {ChannelType::kGPIO, PIN_B0},  // Channel 8
    {ChannelType::kGPIO, PIN_B1},  // Channel 9
    {ChannelType::kGPIO, PIN_C0},  // Channel 10
    {ChannelType::kGPIO, PIN_C1},  // Channel 11
    {ChannelType::kGPIO, PIN_C2},  // Channel 12
    {ChannelType::kGPIO, PIN_C3},  // Channel 13
    {ChannelType::kGPIO, PIN_C4},  // Channel 14
    {ChannelType::kGPIO, PIN_C5},  // Channel 15
    {ChannelType::kNC, PIN_INVALID},  // Channel 16
    {ChannelType::kVrefint, PIN_INVALID},  // Channel 17
    {ChannelType::kTempVbatt, PIN_INVALID},  // Channel 18
  }},
  {{
    // ADC2
    {ChannelType::kGPIO, PIN_A0},  // Channel 0
    {ChannelType::kGPIO, PIN_A1},  // Channel 1
    {ChannelType::kGPIO, PIN_A2},  // Channel 2
    {ChannelType::kGPIO, PIN_A3},  // Channel 3
    {ChannelType::kGPIO, PIN_A4},  // Channel 4
    {ChannelType::kGPIO, PIN_A5},  // Channel 5
    {ChannelType::kGPIO, PIN_A6},  // Channel 6
    {ChannelType::kGPIO, PIN_A7},  // Channel 7
    {ChannelType::kGPIO, PIN_B0},  // Channel 8
    {ChannelType::kGPIO, PIN_B1},  // Channel 9
    {ChannelType::kGPIO, PIN_C0},  // Channel 10
    {ChannelType::kGPIO, PIN_C1},  // Channel 11
    {ChannelType::kGPIO, PIN_C2},  // Channel 12
    {ChannelType::kGPIO, PIN_C3},  // Channel 13
    {ChannelType::kGPIO, PIN_C4},  // Channel 14
    {ChannelType::kGPIO, PIN_C5},  // Channel 15
    {ChannelType::kNC, PIN_INVALID},  // Channel 16
    {ChannelType::kNC, PIN_INVALID},  // Channel 17
    {ChannelType::kNC, PIN_INVALID},  // Channel 18
  }},
  {{
    // ADC3
    {ChannelType::kGPIO, PIN_A0},  // Channel 0
    {ChannelType::kGPIO, PIN_A1},  // Channel 1
    {ChannelType::kGPIO, PIN_A2},  // Channel 2
    {ChannelType::kGPIO, PIN_A3},  // Channel 3
    {ChannelType::kGPIO, PIN_F6},  // Channel 4
    {ChannelType::kGPIO, PIN_F7},  // Channel 5
    {ChannelType::kGPIO, PIN_F8},  // Channel 6
    {ChannelType::kGPIO, PIN_F9},  // Channel 7
    {ChannelType::kGPIO, PIN_F10},  // Channel 8
    {ChannelType::kGPIO, PIN_F3},  // Channel 9
    {ChannelType::kGPIO, PIN_C0},  // Channel 10
    {ChannelType::kGPIO, PIN_C1},  // Channel 11
    {ChannelType::kGPIO, PIN_C2},  // Channel 12
    {ChannelType::kGPIO, PIN_C3},  // Channel 13
    {ChannelType::kGPIO, PIN_F4},  // Channel 14
    {ChannelType::kGPIO, PIN_F5},  // Channel 15
    {ChannelType::kNC, PIN_INVALID},  // Channel 16
    {ChannelType::kNC, PIN_INVALID},  // Channel 17
    {ChannelType::kNC, PIN_INVALID},  // Channel 18
  }}
};

struct SamplingTimeSetting {
  uint32_t setting;
  uint32_t cycles;
};

constexpr std::array<SamplingTimeSetting, 8> kSamplingTimes = {{
  {ADC_SMPR_SMP_3CYC, 3},
  {ADC_SMPR_SMP_15CYC, 15},
  {ADC_SMPR_SMP_28CYC, 28},
  {ADC_SMPR_SMP_56CYC, 56},
  {ADC_SMPR_SMP_84CYC, 84},
  {ADC_SMPR_SMP_112CYC, 112},
  {ADC_SMPR_SMP_144CYC, 144},
  {ADC_SMPR_SMP_480CYC, 480},
}};

}; // namespace Ostrich

#endif // __ADC_DEFS_H__
