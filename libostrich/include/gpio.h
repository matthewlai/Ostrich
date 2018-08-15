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

#ifndef __GPIO_H__
#define __GPIO_H__

#include "gpio_defs.h"
#include "util.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <cstdint>

#include <array>

#define GPIO_TUPLES \
  GPIO_TUPLES_ITEM(GPIOA, RCC_GPIOA, 0) \
  GPIO_TUPLES_ITEM(GPIOB, RCC_GPIOB, 1) \
  GPIO_TUPLES_ITEM(GPIOC, RCC_GPIOC, 2) \
  GPIO_TUPLES_ITEM(GPIOD, RCC_GPIOD, 3) \
  GPIO_TUPLES_ITEM(GPIOE, RCC_GPIOE, 4) \
  GPIO_TUPLES_ITEM(GPIOF, RCC_GPIOF, 5) \
  GPIO_TUPLES_ITEM(GPIOG, RCC_GPIOG, 6) \
  GPIO_TUPLES_ITEM(GPIOH, RCC_GPIOH, 7) \
  GPIO_TUPLES_ITEM(GPIOI, RCC_GPIOI, 8) \
  GPIO_TUPLES_ITEM(GPIOJ, RCC_GPIOJ, 9) \
  GPIO_TUPLES_ITEM(GPIOK, RCC_GPIOK, 10)

namespace Ostrich {

constexpr size_t kNumGPIOPorts = 11;

// GPIOManager handles peripheral clock enable, and GPIO pin allocations.
class GPIOManager : public NonCopyable {
 public:
  static GPIOManager& GetInstance() {
    static GPIOManager instance;
    return instance;
  }

  // Allocate one or more pins (pin is a bitfield), and turn on clock for
  // the GPIO if necessary.
  void AllocatePins(uint32_t port, uint16_t pin);

 private:
  GPIOManager();

  static size_t PortToIndex(uint32_t port);
  static void EnableClock(uint32_t port);

  // allocations_[port][pin] is the number of allocations for port + pin
  // This should be 0 or 1 normally, but we allow higher numbers to
  // potentially support sharing.
  std::array<std::array<int, 16>, kNumGPIOPorts> allocations_;
  std::array<bool, kNumGPIOPorts> port_clock_enabled_;
};

template <GPIOPortPin kPortPin>
class OutputPin : NonCopyable {
 public:
  /* pupd is GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, or GPIO_PUPD_PULLDOWN */
  OutputPin(uint32_t pupd = GPIO_PUPD_NONE) {
    GPIOManager::GetInstance().AllocatePins(UnpackPort(kPortPin),
      UnpackPin(kPortPin));
    gpio_mode_setup(UnpackPort(kPortPin), GPIO_MODE_OUTPUT, pupd,
      UnpackPin(kPortPin));
  }

  OutputPin& operator=(bool val) {
    if (val) {
      gpio_set(UnpackPort(kPortPin), UnpackPin(kPortPin));
    } else {
      gpio_clear(UnpackPort(kPortPin), UnpackPin(kPortPin));
    }
    return *this;
  }
};

template <GPIOPortPin kPortPin>
class InputPin : NonCopyable {
 public:
  /* pupd is GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, or GPIO_PUPD_PULLDOWN */
  InputPin(uint32_t pupd = GPIO_PUPD_NONE) {
    GPIOManager::GetInstance().AllocatePins(UnpackPort(kPortPin),
      UnpackPin(kPortPin));
    gpio_mode_setup(UnpackPort(kPortPin), GPIO_MODE_INPUT, pupd,
      UnpackPin(kPortPin));
  }

  bool value() const {
    return gpio_get(UnpackPort(kPortPin), UnpackPin(kPortPin));
  }

  operator bool() const {
    return value();
  }
};

}; // namespace Ostrich

#endif // __GPIO_H__
