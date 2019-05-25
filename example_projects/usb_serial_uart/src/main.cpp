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

#include <memory>
#include <string>

#include "ostrich.h"
#include "systick.h"
#include "usart.h"
#include "usb/serial.h"

using Ostrich::USBSerial;
using Ostrich::USART;
using Ostrich::OutputPin;
using namespace Ostrich::GpioDef;

constexpr auto kUartTxPin = PIN_G14;
constexpr auto kUartRxPin = PIN_G9;
constexpr auto kUart = USART6;
constexpr auto kFlushIntervalMilliseconds = 10;

int main() {
  USBSerial usb_serial;

  Ostrich::SetErrorHandler([&usb_serial](const std::string& error) {
    usb_serial << error << std::endl;
  });

  Ostrich::SetLoggingHandler([&usb_serial](const std::string& log) {
    usb_serial << log << std::endl;
  });

  // Wait for port to be opened.
  while (!usb_serial.PortOpen()) {}

  uint32_t baud_rate = usb_serial.BaudRate();

  using UsartPortType = USART<kUart, kUartTxPin, kUartRxPin>;
  auto usart = std::make_unique<UsartPortType>(baud_rate);

  uint64_t last_flush_time = Ostrich::GetTimeMilliseconds();

  while (true) {
    auto usb_available = usb_serial.DataAvailable();

    if (usb_available) {
      std::vector<char> buf(usb_available);
      usb_serial.Read(buf.data(), usb_available);
      usart->Write(buf.data(), usb_available);
    }

    auto usart_available = usart->DataAvailable();

    if (usart_available) {
      std::vector<char> buf(usart_available);
      usart->Read(buf.data(), usart_available);
      usb_serial.Write(buf.data(), usart_available);
    }

    if (usb_serial.BaudRate() != baud_rate) {
      baud_rate = usb_serial.BaudRate();
      usart.reset();
      usart.reset(new USART<kUart, kUartTxPin, kUartRxPin>(baud_rate));
    }

    // Don't let data sit in the buffer for too long.
    uint64_t now = Ostrich::GetTimeMilliseconds();
    if ((now - last_flush_time) > kFlushIntervalMilliseconds) {
      usart->Flush();
      usb_serial.Flush();
      last_flush_time = now;
    }
  }
}
