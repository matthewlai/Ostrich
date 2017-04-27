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
  for (auto& port_users : allocations_) {
    for (auto& pin_users : port_users) {
      pin_users = 0;
    }
  }

  for (auto& clock_enabled : port_clock_enabled_) {
    clock_enabled = false;
  }
}
#pragma GCC diagnostic pop

void GPIOManager::AllocatePins(uint32_t port, uint16_t pin) {
  auto port_index = PortToIndex(port);
  while (pin) {
    auto pin_num = __builtin_ctz(pin);
    pin &= ~(1 << pin_num);
    ++allocations_[port_index][pin_num];
  }

  if (!port_clock_enabled_[port_index]) {
    EnableClock(port);
    port_clock_enabled_[port_index] = true;
  }
}

/*static*/ size_t GPIOManager::PortToIndex(uint32_t port) {
  #define GPIO_TUPLES_ITEM(gpio_port, port_rcc, index) \
    if (port == gpio_port) return index;
  GPIO_TUPLES
  #undef GPIO_TUPLES_ITEM

  return 0;
}

/*static*/ void GPIOManager::EnableClock(uint32_t port) {
  #define GPIO_TUPLES_ITEM(gpio_port, port_rcc, index) \
    if (port == gpio_port) rcc_periph_clock_enable(port_rcc);
  GPIO_TUPLES
  #undef GPIO_TUPLES_ITEM
}

}; // namespace Ostrich
