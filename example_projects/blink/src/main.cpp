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

#include "gpio_defs.h"
#include "ostrich.h"
#include "systick.h"

using namespace Ostrich;

OutputPin<PIN_B4> led_0;
OutputPin<PIN_B5> led_1;
OutputPin<PIN_B6> led_2;
OutputPin<PIN_B7> led_3;

int main() {
  while (true) {
    DelayMilliseconds(1000);

    led_0 = 0;
    led_1 = 1;
    led_2 = 0;
    led_3 = 1;

    DelayMilliseconds(1000);

    led_0 = 1;
    led_1 = 0;
    led_2 = 1;
    led_3 = 0;
  }
}
