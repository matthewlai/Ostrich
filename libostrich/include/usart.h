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

#if 0

#include <istream>
#include <streambuf>
#include <vector>

#include <libopencm3/stm32/usart.h>

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

std::array<const USARTInfo, kNumUSARTs> kUSARTInfo{{
  {USART1, RCC_USART1, NVIC_USART1_IRQ, "USART1",
      {{GpioDef::PIN_B14, 4}, {GpioDef::PIN_A9, 7}, {GpioDef::PIN_B6, 7}},
      {{GpioDef::PIN_B15, 4}, {GpioDef::PIN_A10, 7}, {GpioDef::PIN_B7, 7}}},
  {USART2, RCC_USART2, NVIC_USART2_IRQ, "USART2",
      {{GpioDef::PIN_A2, 7}, {GpioDef::PIN_D5, 7}},
      {{GpioDef::PIN_A3, 7}, {GpioDef::PIN_D6, 7}}},
  {USART3, RCC_USART3, NVIC_USART3_IRQ, "USART3",
      {{GpioDef::PIN_B10, 7}, {GpioDef::PIN_C10, 7}, {GpioDef::PIN_D8, 7}},
      {{GpioDef::PIN_B11, 7}, {GpioDef::PIN_C11, 7}, {GpioDef::PIN_D9, 7}}},
  {UART4, RCC_UART4, NVIC_UART4_IRQ, "UART4",
      {{GpioDef::PIN_A12, 6}, {GpioDef::PIN_A0, 8}, {GpioDef::PIN_C10, 8},
       {GpioDef::PIN_D1, 8}, {GpioDef::PIN_H13, 8}},
      {{GpioDef::PIN_A11, 6}, {GpioDef::PIN_A1, 8}, {GpioDef::PIN_C11, 8},
       {GpioDef::PIN_D0, 8}, {GpioDef::PIN_H14, 8}, {GpioDef::PIN_I9, 8}}},
  {UART5, RCC_UART5, NVIC_UART5_IRQ, "UART5",
      {{GpioDef::PIN_B6, 1}, {GpioDef::PIN_B9, 7}, {GpioDef::PIN_B13, 8},
       {GpioDef::PIN_C12, 8} },
      {{GpioDef::PIN_B5, 1}, {GpioDef::PIN_B8, 7}, {GpioDef::PIN_B12, 8},
       {GpioDef::PIN_D2, 8}}},
  {USART6, RCC_USART6, NVIC_USART6_IRQ, "USART6",
      {{GpioDef::PIN_C6, 8}, {GpioDef::PIN_G14, 8} },
      {{GpioDef::PIN_C7, 8}, {GpioDef::PIN_G9, 8}}},
  {UART7, RCC_UART7, NVIC_UART7_IRQ, "UART7",
      {{GpioDef::PIN_E8, 8}, {GpioDef::PIN_F7, 8}, {GpioDef::PIN_A15, 12},
       {GpioDef::PIN_B4, 12}},
      {{GpioDef::PIN_E7, 8}, {GpioDef::PIN_F6, 8}, {GpioDef::PIN_A8, 12},
       {GpioDef::PIN_B3, 12}}},
  {UART8, RCC_UART8, NVIC_UART8_IRQ, "UART8",
      {{GpioDef::PIN_E1, 8}},
      {{GpioDef::PIN_E0, 8}}},
}};

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

const USARTInfo& UsartInfo(uint32_t usart) {
  return kUSARTInfo[UsartToIndex(usart)];
}

// We have a strange setup here. USART derives from both streambuf and iostream.
// It is primarily a streambuf, and the iostream inheritance allows the user to use
// it as an iostream wrapped around itself. We call the iostream constructor with
// "this" in our constructor, and that's the only thing we do with iostream here.
template <uint32_t kUsart, GPIOPortPin kTxPin, GPIOPortPin kRxPin>
class USART : public std::streambuf, public std::iostream {
 public:
  USART(uint32_t baud_rate, uint32_t data_bits = 8, uint32_t stop_bits = 1,
        uint32_t parity = USART_PARITY_NONE) 
    : std::iostream(this), info_(UsartInfo(kUsart)) {
    if (in_use_) {
      HandleError(std::string("USART already in use: ") + info_.str_name);
    } 

    in_use_ = true;

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

    /* Finally enable the USART. */
    usart_enable(kUsart);
    nvic_enable_irq(info_.irq);
  }

  ~USART() {
    nvic_disable_irq(info_.irq);
    rcc_periph_clock_disable(info_.usart_rcc);
    in_use_ = false;
  }

  USART(const USART&) = delete;
  USART& operator=(const USART&) = delete;

  // These typedefs exist in both streambuf and iostream, so we need to disambiguate.
  using std::streambuf::int_type;
  using std::streambuf::traits_type;
  using std::streambuf::char_type;

  void Send(const char* data, std::size_t length) {
    //usart_wait_send_ready()
  }

  void Send(const std::string& s) { Send(s.data(), s.size()); }

  std::size_t DataAvailable() const { return receive_buffer_.Available(); }

  // Wait for the receive buffer to become non-empty.
  void WaitForData() { while (receive_buffer_.Empty()) {} }

  // Non-blocking receive.
  std::size_t Receive(char* buf, std::size_t buf_size) {
    std::size_t to_read = std::min(buf_size, receive_buffer_.Available());
    for (std::size_t i = 0; i < to_read; ++i) {
      buf[i] = receive_buffer_.Pop();
    }
    return to_read;
  }

  void Poll() { usbd_poll(usbd_dev_); }

 protected:
  // std::streambuf interface (outputs)
  std::streamsize xsputn(const char_type* s, std::streamsize count) override {
    Send(s, count);
    return count;
  }

  int_type overflow(int_type ch = traits_type::eof()) override {
    if (!traits_type::eq_int_type(ch, traits_type::eof())) {
      char_type ch_c = ch;
      Send(&ch_c, 1);
    }
    return 1;
  }

  // std::streambuf interface (inputs)
  std::streamsize showmanyc() override {
    return receive_buffer_.Available();
  }
  int_type underflow() override  {
    WaitForData();
    return traits_type::to_int_type(receive_buffer_.Peek());
  }
  int_type uflow() override {
    WaitForData();
    return traits_type::to_int_type(receive_buffer_.Pop());
  }

 private:
  static bool in_use_;
  const USARTInfo& info_;
  volatile CircularQueue<1024> receive_buffer_;
};

}; // namespace Ostrich

#endif

#endif // __USART_H__
