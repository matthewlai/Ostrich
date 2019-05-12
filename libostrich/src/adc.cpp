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

#include "adc.h"
#include "gpio.h"

namespace Ostrich {

ADCManager::ADCManager() {
  for (int i = 0; i < kNumADCs; ++i) {
    in_use_[i] = false;
  }

  SetADCPreDivider();
}

void ADCManager::SetADCPreDivider() {
  // We have 4 dividers to choose from - 2, 4, 6, 8.
  // We pick the lowest divider that gives us an ADC clock within requirements:
  // Vdda 1.7 to 2.4V => 0.6 - 18 MHz
  // Vdda 2.4 to 3.6V => 0.6 - 36 MHz

  uint64_t max_adc_clock = 18000000;
  if (g_vdd_mv > 2400) {
    max_adc_clock = 36000000;
  }

  if ((g_apb2_freq / 2) <= max_adc_clock) {
    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY2);
    adc_clock_ = g_apb2_freq / 2;
  } else if ((g_apb2_freq / 4) <= max_adc_clock) {
    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY4);
    adc_clock_ = g_apb2_freq / 4;
  } else if ((g_apb2_freq / 6) <= max_adc_clock) {
    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY6);
    adc_clock_ = g_apb2_freq / 6;
  } else if ((g_apb2_freq / 8) <= max_adc_clock) {
    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY8);
    adc_clock_ = g_apb2_freq / 8;
  } else {
    HandleError("No valid ADC divider found?");
  }
}

template <uint32_t kADC>
SingleConversionADC<kADC>::SingleConversionADC()
    : adc_info_(GetInfo(kADC)), vbatt_on_(false), vref_vsense_on_(false) {
  adc_power_off(kADC);
  adc_disable_scan_mode(kADC);
  adc_set_single_conversion_mode(kADC);
  adc_enable_eoc_interrupt(kADC);

  // Set sampling time to min by default.
  adc_set_sample_time_on_all_channels(kADC, ADC_SMPR_SMP_3CYC);

  adc_power_on(kADC);
}

template <uint32_t kADC>
SingleConversionADC<kADC>::~SingleConversionADC() {
}

template <uint32_t kADC>
void SingleConversionADC<kADC>::EnsureChannelSetup(int channel, bool is_vbatt) {
  const ChannelInfo& ci = kADCChannelInfos[GetIndex(kADC)][channel];
  if (ci.channel_type == ChannelType::kGPIO) {
    GPIOManager::GetInstance().AllocateAnalogPin(ci.port_pin);
  } else if (ci.channel_type == ChannelType::kVrefint) {
    if (!vref_vsense_on_) {
      adc_disable_vbat_sensor();
      adc_enable_temperature_sensor();
      vref_vsense_on_ = true;
      vbatt_on_ = false;

      // 10us startup time.
      auto end_time = GetTimeMicroseconds() + 10;
      while (GetTimeMicroseconds() < end_time) {}

      // 10us sampling time.
      SetSamplingTime(channel, 10000);
    }
  } else if (ci.channel_type == ChannelType::kTempVbatt) {
    if (is_vbatt) {
      if (!vbatt_on_) {
        adc_disable_temperature_sensor();
        adc_enable_vbat_sensor();
        vref_vsense_on_ = false;
        vbatt_on_ = true;

        // 10us startup time.
        auto end_time = GetTimeMicroseconds() + 10;
        while (GetTimeMicroseconds() < end_time) {}

        // 10us sampling time.
        SetSamplingTime(channel, 10000);
      }
    } else {
      if (!vref_vsense_on_) {
        adc_disable_vbat_sensor();
        adc_enable_temperature_sensor();
        vref_vsense_on_ = true;
        vbatt_on_ = false;

        // 10us startup time.
        auto end_time = GetTimeMicroseconds() + 10;
        while (GetTimeMicroseconds() < end_time) {}

        // 10us sampling time.
        SetSamplingTime(channel, 10000);
      }
    }
  }
}

template <uint32_t kADC>
void SingleConversionADC<kADC>::SetSamplingTime(
    uint32_t channel, uint64_t sampling_time_ns) {
  uint64_t cycle_time_ns =
      1000000000ULL / ADCManager::GetInstance().ADCClock();
  for (const auto& option : kSamplingTimes) {
    if (cycle_time_ns * option.cycles >= sampling_time_ns) {
      adc_set_sample_time(kADC, channel, option.setting);
      return;
    }
  }

  // Set longest time if we got to the end (using higher sampling time would
  // require changing ADCPre, which affects all ADCs).
  adc_set_sample_time(kADC, channel, kSamplingTimes.back().setting);
}

template class SingleConversionADC<ADC1>;
template class SingleConversionADC<ADC2>;
template class SingleConversionADC<ADC3>;

}; // namespace Ostrich
