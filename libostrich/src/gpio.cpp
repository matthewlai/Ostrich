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

#include "gpio.h"

namespace Ostrich {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
GPIOManager::GPIOManager() {
  for (auto& port_in_use : in_use_) {
    port_in_use = 0x0000;
  }
}
#pragma GCC diagnostic pop

void GPIOManager::DeallocatePin(GPIOPortPin portpin) {
  uint32_t port = UnpackPort(portpin);
  auto port_index = PortToIndex(port);
  uint16_t pin = UnpackPin(portpin);

  in_use_[port_index] &= ~pin;

  // Turn the lights out if we are the last one.
  if (in_use_[port_index] == 0x0000) {
    SetClock(port, false);
  }
}

/*static*/ size_t GPIOManager::PortToIndex(uint32_t port) {
  #define GPIO_TUPLES_ITEM(gpio_port, port_rcc, index) \
    if (port == gpio_port) return index;
  GPIO_TUPLES
  #undef GPIO_TUPLES_ITEM

  return 0;
}

/*static*/ void GPIOManager::SetClock(uint32_t port, bool on) {
  #define GPIO_TUPLES_ITEM(gpio_port, port_rcc, index) \
    if (port == gpio_port) { \
      if (on) { \
        rcc_periph_clock_enable(port_rcc); \
      } else { \
        rcc_periph_clock_disable(port_rcc); \
      } \
    }
  GPIO_TUPLES
  #undef GPIO_TUPLES_ITEM
}

}; // namespace Ostrich
