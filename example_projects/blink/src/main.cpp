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

#include "board_config.h"
#include "gpio_defs.h"
#include "ostrich.h"

using Ostrich::OutputPin;
using namespace Ostrich::GpioDef;

OutputPin<PIN_B14> red_led;
OutputPin<PIN_B7> blue_led;
OutputPin<PIN_B0> green_led;

int main() {
  Ostrich::Init(DefaultInitInfo());

  while (true) {
    red_led = 1;
    blue_led = 0;
    green_led = 1;
    for (int i = 0; i < (216000000 / 4); i++) {
      // Woohoo! Double issue!
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
    }
    
    red_led = 0;
    blue_led = 1;
    green_led = 0;
    for (int i = 0; i < (216000000 / 4); i++) {
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
      __asm__("nop");
    }
  }
}
