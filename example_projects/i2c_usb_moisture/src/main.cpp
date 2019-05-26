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

#include "i2c.h"
#include "ostrich.h"
#include "systick.h"
#include "usb/serial.h"

using namespace Ostrich;

constexpr uint8_t kI2CAddress = 0x20;
constexpr uint8_t kMoistureRegister = 0x0;
constexpr uint8_t kTemperatureRegister = 0x5;

template <typename I2CType>
uint16_t ReadRegister(I2CType* i2c, uint8_t register_addr) {
  uint8_t buf[2];
  i2c->SendReceive(kI2CAddress, &register_addr, 1, buf, 2);
  return (static_cast<uint8_t>(buf[0]) << 8) | buf[1];
}

int main() {
  USBSerial usb_serial;

  // Wait for port to be opened.
  while (!usb_serial.PortOpen()) {}

  SetErrorHandler([&usb_serial](const std::string& error) {
    usb_serial << error << std::endl;
  });

  SetLoggingHandler([&usb_serial](const std::string& log) {
    usb_serial << log << std::endl;
  });

  I2C<I2C4, PIN_D13, PIN_D12> i2c(I2CSpeed::Speed100kHz);

  usb_serial << "Begin:" << std::endl;

  while (true) {
    auto moisture = ReadRegister(&i2c, kMoistureRegister);
    auto temperature = ReadRegister(&i2c, kTemperatureRegister);
    usb_serial << "Moisture: " << moisture << ", Temperature: " << temperature
        << std::endl;
    DelayMilliseconds(1000);
  }
}
