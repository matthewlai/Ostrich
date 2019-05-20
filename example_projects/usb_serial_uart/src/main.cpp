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

#include <string>

#include "ostrich.h"
#include "systick.h"
#include "usart.h"
#include "usb/serial.h"

using Ostrich::USBSerial;
using Ostrich::USART;
using Ostrich::OutputPin;
using namespace Ostrich::GpioDef;

constexpr auto kUartTxPin = PIN_C10;
constexpr auto kUartRxPin = PIN_C11;
constexpr auto kUart = USART3;
constexpr int kBaudRate = 115200;

int main() {
  USBSerial usb_serial;

  // Wait for port to be opened.
  while (!usb_serial.PortOpen()) {}

  Ostrich::SetErrorHandler([&usb_serial](const std::string& error) {
    usb_serial << error << std::endl;
  });

  Ostrich::SetLoggingHandler([&usb_serial](const std::string& log) {
    usb_serial << log << std::endl;
  });

  USART<kUart, kUartTxPin, kUartRxPin> usart(kBaudRate);

  while (true) {
    auto usb_available = usb_serial.DataAvailable();

    if (usb_available) {
      std::vector<char> buf(usb_available);
      usb_serial.Read(buf.data(), usb_available);
      usart.Write(buf.data(), usb_available);
      usart.Flush();
    }

    auto usart_available = usart.DataAvailable();

    if (usart_available) {
      std::vector<char> buf(usart_available);
      usart.Read(buf.data(), usart_available);
      usb_serial.Write(buf.data(), usart_available);
      usb_serial.Flush();
    }
  }
}
