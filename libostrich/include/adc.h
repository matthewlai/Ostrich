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

#ifndef __ADC_H____
#define __ADC_H__

#include <array>
#include <cstdint>

#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include "adc_defs.h"
#include "gpio.h"
#include "ostrich.h"
#include "systick.h"
#include "util.h"

namespace Ostrich {

constexpr int kNumADCs = 3;
constexpr int kNumChannels = 19;
constexpr int kADCFullScale = 4096;  // Only supporting 12-bit for now.

struct ADCInfo {
  uint32_t adc_base;
  rcc_periph_clken adc_rcc;
  const char* str_name;
};

constexpr std::array<ADCInfo, kNumADCs> kADCInfos{{
  {ADC1, RCC_ADC1, "ADC1"},
  {ADC2, RCC_ADC2, "ADC2"},
  {ADC3, RCC_ADC3, "ADC3"}
}};

constexpr int GetIndex(uint32_t adc) {
  switch (adc) {
    case ADC1:
      return 0;
    case ADC2:
      return 1;
    case ADC3:
      return 2;
    default:
      return -1;
  }
}

constexpr const ADCInfo& GetInfo(uint32_t adc) {
  return kADCInfos[GetIndex(adc)];
}

class ADCManager : public NonCopyable {
 public:
  static ADCManager& GetInstance() {
    static ADCManager instance;
    return instance;
  }

  bool AllocateADC(uint32_t adc) {
    if (in_use_[GetIndex(adc)]) {
      return false;
    } else {
      in_use_[GetIndex(adc)] = true;
      rcc_periph_clock_enable(GetInfo(adc).adc_rcc);
      return true;
    }
  }

  void DeallocateADC(uint32_t adc) {
    in_use_[GetIndex(adc)] = false;
    rcc_periph_clock_disable(GetInfo(adc).adc_rcc);
  }

  uint64_t ADCClock() const { return adc_clock_; }

  void SetADCPreDivider();

 private:
  ADCManager();

  std::array<bool, kNumADCs> in_use_;
  uint64_t adc_clock_;
};

template <uint32_t kADC>
class ADCBase : public NonCopyable {
 public:
  ADCBase() : adc_info_(GetInfo(kADC)) {
    AssertTrue(ADCManager::GetInstance().AllocateADC(kADC),
               std::string(adc_info_.str_name) + " already in use");
  }

  ~ADCBase() {
    ADCManager::GetInstance().DeallocateADC(kADC);
  }

 protected:
  const ADCInfo& adc_info_;
};

template <uint32_t kADC>
class SingleConversionADC : public ADCBase<kADC> {
 public:
  SingleConversionADC();
  ~SingleConversionADC();

  template <uint8_t kChannel>
  class ChannelSampler {
   public:
    ChannelSampler(SingleConversionADC* adc, bool is_vbatt) : adc_(adc) {
      adc->EnsureChannelSetup(kChannel, is_vbatt);
    }

    void SetSamplingTime(uint64_t sampling_time_ns) {
      adc_->SetSamplingTime(kChannel, sampling_time_ns);
    }

    uint16_t ReadU16() { return adc_->ReadChannel<kChannel>(); }

    float ReadNormalized() {
      return static_cast<float>(adc_->ReadChannel<kChannel>()) / 4096;
    }

   private:
    SingleConversionADC* adc_;
  };

  template <uint8_t kChannel>
  ChannelSampler<kChannel> GetGPIOInput() {
    return ChannelSampler<kChannel>(this, /*is_vbatt=*/false);
  }

 private:
  void EnsureChannelSetup(int channel, bool is_vbatt);
  void SetSamplingTime(uint32_t channel, uint64_t sampling_time_ns);

  template <uint8_t kChannel>
  uint16_t ReadChannel() {
    uint8_t channel = kChannel;
    adc_set_regular_sequence(kADC, 1, &channel);
    adc_start_conversion_regular(kADC);
    while (!adc_eoc(kADC)) {
      WaitForInterrupt();
    }
    return adc_read_regular(kADC);
  }

  const ADCInfo& adc_info_;

  // These are connected to the internal ADC1 channels - Vref, Vsense (temp),
  // and Vbatt. Either Vref + Vsense, or Vbat can be on at a time.
  bool vbatt_on_;
  bool vref_vsense_on_;
};

}  // namespace Ostrich

#endif // __ADC_H__