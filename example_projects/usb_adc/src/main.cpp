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

#include <string>

#include "adc.h"
#include "ostrich.h"
#include "systick.h"
#include "usb/serial.h"

using Ostrich::USBSerial;
using Ostrich::SingleConversionADC;

int main() {
  USBSerial serial;

  SingleConversionADC<ADC1> adc;
  auto temp_sampler = adc.GetTemperatureInput();

  while (true) {
    if (serial.PortOpen()) {
      serial << temp_sampler.ReadTempC() << std::endl;
      Ostrich::DelayMilliseconds(1000);
    }
  }
}
