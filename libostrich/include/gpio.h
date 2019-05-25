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
class GPIOManager : public Singleton {
 public:
  static GPIOManager& GetInstance() {
    static GPIOManager instance;
    return instance;
  }

  // This is an RAII token representing a grant from GPIOManager to the holder
  // to use the pin.
  class PinAllocation : public NonCopyable {
   public:
    friend class GPIOManager;

    PinAllocation(PinAllocation&& other) {
      portpin_ = other.portpin_;
      other.portpin_ = PIN_INVALID;
    }

    PinAllocation& operator=(PinAllocation&& other) {
      portpin_ = other.portpin_;
      other.portpin_ = PIN_INVALID;
      return *this;
    }

    ~PinAllocation() {
      // portpin_ may be invalid if this PinAllocation was std::move()d.
      if (portpin_ != PIN_INVALID) {
        GPIOManager::GetInstance().DeallocatePin(portpin_);
      }
    }

    void SetInput(uint32_t pupd = GPIO_PUPD_NONE) {
      gpio_mode_setup(UnpackPort(portpin_), GPIO_MODE_INPUT, pupd,
          UnpackPin(portpin_));
    }

    void SetOutput(uint32_t pupd = GPIO_PUPD_NONE) {
      gpio_mode_setup(UnpackPort(portpin_), GPIO_MODE_OUTPUT, pupd,
          UnpackPin(portpin_));
    }

    void SetAF(uint32_t af, uint32_t pupd = GPIO_PUPD_NONE) {
      auto port = UnpackPort(portpin_);
      auto pin = UnpackPin(portpin_);
      gpio_mode_setup(port, GPIO_MODE_AF, pupd, pin);
      gpio_set_af(port, af, pin);
    }

    void SetAnalog() {
      gpio_mode_setup(UnpackPort(portpin_), GPIO_MODE_ANALOG, GPIO_PUPD_NONE,
                      UnpackPin(portpin_));
    }

    /* pupd is GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, or GPIO_PUPD_PULLDOWN */
    /* output_type is GPIO_OTYPE_PP (push-pull) or GPIO_OTYPE_OD (open drain) */
    /* output_speed is GPIO_OSPEED_*MHZ, where * = {2, 25, 50, 100} */
    void SetOutputOptions(uint32_t output_type, uint32_t output_speed) {
      gpio_set_output_options(UnpackPort(portpin_), output_type, output_speed,
                              UnpackPin(portpin_));
    }

   private:
    // Only GPIOManager may construct PinAllocations.
    PinAllocation(GPIOPortPin portpin) : portpin_(portpin) {}

    GPIOPortPin portpin_;
  };

  friend class PinAllocation;

  // Allocate a pin and turn on the GPIO clock if necessary.
  PinAllocation AllocatePin(GPIOPortPin portpin) {
    uint32_t port = UnpackPort(portpin);
    auto port_index = PortToIndex(port);
    uint16_t pin = UnpackPin(portpin);

    // We need to turn clock on if we are the first pin to be allocated in this
    // port.
    if (in_use_[port_index] == 0x0000) {
      SetClock(port, true);
    }

    in_use_[port_index] |= pin;

    return PinAllocation(portpin);
  }

 private:
  GPIOManager();

  // This is only called by the destructor of PinAllocation.
  void DeallocatePin(GPIOPortPin portpin);

  static size_t PortToIndex(uint32_t port);
  static void SetClock(uint32_t port, bool on);

  // in_use_[port] records whether each pin is in use or not, in a
  // bitfield for each port.
  std::array<uint16_t, kNumGPIOPorts> in_use_;
};

// These are templatized convenience wrappers for libopencm3 GPIO functions.
template <GPIOPortPin kPortPin>
void SetGPIOPin(bool val) {
  if (val) {
    gpio_set(UnpackPort(kPortPin), UnpackPin(kPortPin));
  } else {
    gpio_clear(UnpackPort(kPortPin), UnpackPin(kPortPin));
  }
}

template <GPIOPortPin kPortPin>
void ToggleGPIOPin() {
  gpio_toggle(UnpackPort(kPortPin), UnpackPin(kPortPin));
}

template <GPIOPortPin kPortPin>
bool ReadGPIOPin() {
  return !!gpio_get(UnpackPort(kPortPin), UnpackPin(kPortPin));
}

template <GPIOPortPin kPortPin>
class OutputPin {
 public:
  /* pupd is GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, or GPIO_PUPD_PULLDOWN */
  /* output_type is GPIO_OTYPE_PP (push-pull) or GPIO_OTYPE_OD (open drain) */
  /* output_speed is GPIO_OSPEED_*MHZ, where * = {2, 25, 50, 100} */
  OutputPin(uint32_t pupd = GPIO_PUPD_NONE,
            uint32_t output_type = GPIO_OTYPE_PP,
            uint32_t output_speed = GPIO_OSPEED_25MHZ)
      : allocation_(GPIOManager::GetInstance().AllocatePin(kPortPin)) {
    allocation_.SetOutputOptions(output_type, output_speed);
    allocation_.SetOutput(pupd);
  }

  OutputPin& operator=(bool val) {
    SetGPIOPin<kPortPin>(val);
    return *this;
  }

  void Toggle() {
    ToggleGPIOPin<kPortPin>();
  }

 private:
  GPIOManager::PinAllocation allocation_;
};

template <GPIOPortPin kPortPin>
class InputPin {
 public:
  /* pupd is GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, or GPIO_PUPD_PULLDOWN */
  InputPin(uint32_t pupd = GPIO_PUPD_NONE)
      : allocation_(GPIOManager::GetInstance().AllocatePin(kPortPin)) {
    allocation_.SetInput(pupd);
  }

  bool value() const {
    return ReadGPIOPin<kPortPin>();
  }

  operator bool() const {
    return value();
  }

 private:
  GPIOManager::PinAllocation allocation_;
};

}; // namespace Ostrich

#endif // __GPIO_H__
