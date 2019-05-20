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

#ifndef __USART_H__
#define __USART_H__

#include <functional>
#include <vector>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#include "buffered_stream.h"
#include "gpio.h"
#include "util.h"

namespace Ostrich {

constexpr int kNumUSARTs = 8;

struct USARTInfo {
  struct PinOption {
    GPIOPortPin port_pin;
    int af;
  };

  uint32_t usart_base;
  rcc_periph_clken usart_rcc;
  uint8_t irq;
  const char* str_name;

  std::vector<PinOption> tx_options;
  std::vector<PinOption> rx_options;
};

extern std::array<const USARTInfo, kNumUSARTs> kUSARTInfo;

constexpr int UsartToIndex(uint32_t usart) {
  switch (usart) {
    case USART1: return 0;
    case USART2: return 1;
    case USART3: return 2;
    case UART4: return 3;
    case UART5: return 4;
    case USART6: return 5;
    case UART7: return 6;
    case UART8: return 7;
    default: return -1;
  }
}

inline const USARTInfo& UsartInfo(uint32_t usart) {
  return kUSARTInfo[UsartToIndex(usart)];
}

class USARTManager : public NonCopyable {
 public:
  static USARTManager& GetInstance() {
    static USARTManager instance;
    return instance;
  }

  using Callback = std::function<void()>;

  void AllocateUSART(uint32_t usart) {
    int index = UsartToIndex(usart);
    if (in_use_[index]) {
      HandleError(std::string("USART already in use"));
    } 

    in_use_[index] = true;
  }

  void DeallocateUSART(uint32_t usart) {
    in_use_[UsartToIndex(usart)] = false;
  }

  void RegisterISRCallback(uint32_t usart, Callback callback) {
    isr_callbacks_[UsartToIndex(usart)] = callback;
  }

  void DeregisterISRCallback(uint32_t usart) {
    isr_callbacks_[UsartToIndex(usart)] = Callback();
  }

  void InvokeCallback(uint32_t usart) {
    if (isr_callbacks_[UsartToIndex(usart)]) {
      isr_callbacks_[UsartToIndex(usart)]();
    }
  }

 private:
  USARTManager() {
    for (auto& in_use : in_use_) {
      in_use = false;
    }
  }

  std::array<Callback, kNumUSARTs> isr_callbacks_;
  std::array<bool, kNumUSARTs> in_use_;
};

template <uint32_t kUsart, GPIOPortPin kTxPin, GPIOPortPin kRxPin>
class USART : public BufferedInputStream<1024>, public UnbufferedOutputStream {
 public:
  USART(uint32_t baud_rate, uint32_t data_bits = 8, uint32_t stop_bits = 1,
        uint32_t parity = USART_PARITY_NONE) 
    : info_(UsartInfo(kUsart)) {

    USARTManager::GetInstance().AllocateUSART(kUsart);

    int tx_af = -1;
    int rx_af = -1;
    for (const auto& option : info_.tx_options) {
      if (option.port_pin == kTxPin) {
        tx_af = option.af;
        break;
      }
    }

    for (const auto& option : info_.rx_options) {
      if (option.port_pin == kRxPin) {
        rx_af = option.af;
        break;
      }
    }

    if (tx_af == -1) { HandleError("USART TX pin invalid"); }
    if (rx_af == -1) { HandleError("USART RX pin invalid"); }

    GPIOManager::GetInstance().AllocateAFPin(kTxPin, tx_af);
    GPIOManager::GetInstance().AllocateAFPin(kRxPin, rx_af);

    rcc_periph_clock_enable(info_.usart_rcc);

    usart_set_baudrate(kUsart, baud_rate);
    usart_set_databits(kUsart, data_bits);

    if (stop_bits != 1) {
      HandleError("Stop bits other than 1 not yet implemented.");
    }

    usart_set_stopbits(kUsart, USART_STOPBITS_1);
    usart_set_mode(kUsart, USART_MODE_TX_RX);
    usart_set_parity(kUsart, parity);
    usart_set_flow_control(kUsart, USART_FLOWCONTROL_NONE);
    usart_enable_rx_interrupt(kUsart);

    USARTManager::GetInstance().RegisterISRCallback(
      kUsart, [this]() { CheckForReceivedData(); });

    /* Finally enable the USART. */
    usart_enable(kUsart);
    nvic_enable_irq(info_.irq);
  }

  ~USART() {
    nvic_disable_irq(info_.irq);
    rcc_periph_clock_disable(info_.usart_rcc);
    USARTManager::GetInstance().DeregisterISRCallback(kUsart);
    USARTManager::GetInstance().DeallocateUSART(kUsart);
  }

  USART(const USART&) = delete;
  USART& operator=(const USART&) = delete;

  void CheckForReceivedData() {
    if (USART_ISR(kUsart) & USART_ISR_RXNE) {
      char received = usart_recv(kUsart);
      AddDataToBuffer(&received, 1);
    }

    USART_ICR(kUsart) |= USART_ICR_ORECF;
  }

 protected:
  void OutputImpl(const char* data, std::size_t len) override {
    for (std::size_t i = 0; i < len; ++i) {
      usart_send_blocking(kUsart, data[i]);
    }
  }

 private:
  const USARTInfo& info_;
  volatile CircularQueue<1024> receive_buffer_;
};

}; // namespace Ostrich

#endif // __USART_H__
