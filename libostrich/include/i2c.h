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

#ifndef __I2C_H__
#define __I2C_H__

#include <functional>
#include <vector>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/i2c.h>

#include "gpio.h"
#include "ostrich.h"
#include "util.h"

namespace Ostrich {

constexpr int kNumI2Cs = 4;

struct I2CInfo {
  struct PinOption {
    GPIOPortPin port_pin;
    int af;
  };

  uint32_t i2c_base;
  rcc_periph_clken i2c_rcc;
  uint8_t irq;
  const char* str_name;

  std::vector<PinOption> sda_options;
  std::vector<PinOption> scl_options;
};

extern std::array<const I2CInfo, kNumI2Cs> kI2CInfo;

constexpr int I2CToIndex(uint32_t i2c) {
  switch (i2c) {
    case I2C1: return 0;
    case I2C2: return 1;
    case I2C3: return 2;
    case I2C4: return 3;
    default: return -1;
  }
}

inline const I2CInfo& GetI2CInfo(uint32_t i2c) {
  return kI2CInfo[I2CToIndex(i2c)];
}

constexpr int kNumI2CSpeeds = 4;

enum class I2CSpeed {
  Speed10kHz = 0,
  Speed100kHz = 1,
  Speed400kHz = 2,
  Speed1MHz = 3
};

// Time delays for speed options are taken from RM0410, table -
// "Examples of timings settings for fI2CCLK = 48 MHz"
// On the F7 APB1 clock (=I2CCLK in our case) can can go 54 MHz, so the values
// may be a bit off.
// If the actual APB1 clock is lower, we use the I2CCLK prescalar to keep the
// I2C clock more or less constant, but some of these parameters will be too
// conservative because they depend on the unscaled clock, too. They should be
// safe, though.
struct I2CSpeedParams {
  uint32_t presc_target;
  uint8_t scll;
  uint8_t sclh;
  uint8_t sdadel;
  uint8_t scldel;
};

constexpr std::array<I2CSpeedParams, kNumI2CSpeeds> kI2CSpeedSettings = {{
  // 10 kHz
  {4000000, 0xc7, 0xc3, 0x2, 0x4},

  // 100 kHz
  {4000000, 0x13, 0xf, 0x2, 0x4},

  // 400 kHz
  {8000000, 0x9, 0x3, 0x3, 0x3},

  // 1 MHz
  {8000000, 0x3, 0x1, 0x0, 0x1},
}};


class I2CManager : public NonCopyable {
 public:
  static I2CManager& GetInstance() {
    static I2CManager instance;
    return instance;
  }

  using Callback = std::function<void()>;

  void AllocateI2C(uint32_t i2c) {
    int index = I2CToIndex(i2c);
    if (in_use_[index]) {
      HandleError(std::string("I2C already in use"));
    } 

    in_use_[index] = true;
  }

  void DeallocateI2C(uint32_t i2c) {
    in_use_[I2CToIndex(i2c)] = false;
  }

  void RegisterISRCallback(uint32_t i2c, Callback callback) {
    isr_callbacks_[I2CToIndex(i2c)] = callback;
  }

  void DeregisterISRCallback(uint32_t i2c) {
    isr_callbacks_[I2CToIndex(i2c)] = Callback();
  }

  void InvokeCallback(uint32_t i2c) {
    if (isr_callbacks_[I2CToIndex(i2c)]) {
      isr_callbacks_[I2CToIndex(i2c)]();
    }
  }

 private:
  I2CManager() {
    for (auto& in_use : in_use_) {
      in_use = false;
    }
  }

  std::array<Callback, kNumI2Cs> isr_callbacks_;
  std::array<bool, kNumI2Cs> in_use_;
};

template <uint32_t kI2C, GPIOPortPin kSDAPin, GPIOPortPin kSCLPin>
class I2C : public NonCopyable {
 public:
  I2C(I2CSpeed speed) 
    : sda_allocation_(GPIOManager::GetInstance().AllocatePin(kSDAPin)),
      scl_allocation_(GPIOManager::GetInstance().AllocatePin(kSCLPin)),
      info_(GetI2CInfo(kI2C)) {

    I2CManager::GetInstance().AllocateI2C(kI2C);

    int sda_af = -1;
    int scl_af = -1;
    for (const auto& option : info_.sda_options) {
      if (option.port_pin == kSDAPin) {
        sda_af = option.af;
        break;
      }
    }

    for (const auto& option : info_.scl_options) {
      if (option.port_pin == kSCLPin) {
        scl_af = option.af;
        break;
      }
    }

    if (sda_af == -1) { HandleError("I2C SDA pin invalid"); }
    if (scl_af == -1) { HandleError("I2C SCL pin invalid"); }

    sda_allocation_.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ);
    scl_allocation_.SetOutputOptions(GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ);

    sda_allocation_.SetAF(sda_af);
    scl_allocation_.SetAF(scl_af);

    rcc_periph_clock_enable(info_.i2c_rcc);

    // Setup.
    const auto& speed_settings = kI2CSpeedSettings[static_cast<int>(speed)];

    // Calculate prescalar based on APB1 clocks.
    uint32_t prescalar_setting = 0;

    while ((g_apb1_freq / (prescalar_setting + 1)) >
           speed_settings.presc_target) {
      ++prescalar_setting;
    }

    i2c_set_prescaler(kI2C, prescalar_setting);
    i2c_set_scl_low_period(kI2C, speed_settings.scll);
    i2c_set_scl_high_period(kI2C, speed_settings.sclh);
    i2c_set_data_setup_time(kI2C, speed_settings.scldel);
    i2c_set_data_hold_time(kI2C, speed_settings.sdadel);

    // TODO: Use interrupts.
    // I2CManager::GetInstance().RegisterISRCallback(
    //   kI2C, [this]() { });
    // nvic_enable_irq(info_.irq);

    // Enable the I2C peripheral.
    i2c_peripheral_enable(kI2C);
  }

  ~I2C() {
    //nvic_disable_irq(info_.irq);
    i2c_peripheral_disable(kI2C);
    rcc_periph_clock_disable(info_.i2c_rcc);
    I2CManager::GetInstance().DeregisterISRCallback(kI2C);
    I2CManager::GetInstance().DeallocateI2C(kI2C);
  }

  // All addresses should be in 7-bit form.
  // All these functions return whether the transmission was successful
  // (everything acked).
  // Maximum transfer size is 256 bytes.
  bool Send(uint8_t addr, const uint8_t* data, std::size_t len) {
    return SendReceive(addr, data, len);
  }

  bool Receive(uint8_t addr, uint8_t* buf, std::size_t len) {
    return SendReceive(addr, nullptr, 0, buf, len);
  }

  // Write write_len bytes, then read read_len bytes, with a repeated start
  // between read and write.
  // This function is adapted from i2c_transfer7() in libopencm3.
  bool SendReceive(uint8_t addr, const uint8_t* write_data = nullptr,
                   std::size_t write_len = 0, uint8_t* read_buf = nullptr,
                   std::size_t read_len = 0) {
    /*  waiting for busy is unnecessary. read the RM */
    if (write_data) {
      i2c_set_7bit_address(kI2C, addr);
      i2c_set_write_transfer_dir(kI2C);
      i2c_set_bytes_to_transfer(kI2C, write_len);
      if (read_buf) {
        i2c_disable_autoend(kI2C);
      } else {
        i2c_enable_autoend(kI2C);
      }
      i2c_send_start(kI2C);

      for (std::size_t i = 0; i < write_len; ++i) {
        // Wait till we are either cleared to transmit or we got NACKed.
        while (!i2c_transmit_int_status(kI2C)) {
          if (i2c_nack(kI2C)) {
            HandleError("I2C NACK received");
            return false;
          }
        }

        i2c_send_data(kI2C, write_data[i]);
      }

      /* not entirely sure this is really necessary.
       * RM implies it will stall until it can write out the later bits
       */
      if (read_buf) {
        while (!i2c_transfer_complete(kI2C));
      }
    }

    if (read_buf) {
      i2c_set_7bit_address(kI2C, addr);
      i2c_set_read_transfer_dir(kI2C);
      i2c_set_bytes_to_transfer(kI2C, read_len);

      i2c_send_start(kI2C);
      /* important to do it afterwards to do a proper repeated start! */
      i2c_enable_autoend(kI2C);

      for (std::size_t i = 0; i < read_len; i++) {
        while (i2c_received_data(kI2C) == 0) {}
        read_buf[i] = i2c_get_data(kI2C);
      }
    }

    return true;
  }

 private:
  GPIOManager::PinAllocation sda_allocation_;
  GPIOManager::PinAllocation scl_allocation_;

  const I2CInfo& info_;
};

}; // namespace Ostrich

#endif // __I2C_H__
