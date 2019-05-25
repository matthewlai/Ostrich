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
#include "usb/serial.h"

using namespace Ostrich;

int main() {
  USBSerial serial;

  bool port_open = false;

  while (true) {
    if (!port_open && serial.PortOpen()) {
      // Port has just been opened (DTR received).
      serial << "Port opened" << '\n';
    }

    port_open = serial.PortOpen();

    if (serial.DataAvailable()) {
      serial << serial.GetLine() << std::endl;
    }
  }
}
