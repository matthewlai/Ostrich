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

#include "i2c.h"

#include <string>

#include "ostrich.h"

extern "C" {

// TODO: ISRs.

}

namespace Ostrich {

std::array<const I2CInfo, kNumI2Cs> kI2CInfo{{
{I2C1, RCC_I2C1, NVIC_I2C1_EV_IRQ, "I2C1",
    {{PIN_B7, 4}, {PIN_B9, 4}},
    {{PIN_B6, 4}, {PIN_B8, 4}}},
{I2C2, RCC_I2C2, NVIC_I2C2_EV_IRQ, "I2C2",
    {{PIN_F0, 4}, {PIN_B11, 4}, {PIN_H5, 4}},
    {{PIN_F1, 4}, {PIN_B10, 4}, {PIN_H4, 4}}},
{I2C3, RCC_I2C3, NVIC_I2C3_EV_IRQ, "I2C3",
    {{PIN_C9, 4}, {PIN_H8, 4}},
    {{PIN_A8, 4}, {PIN_H7, 4}}},
{I2C4, RCC_I2C4, NVIC_I2C4_EV_IRQ, "I2C4",
    {{PIN_D13, 4}, {PIN_H5, 4}, {PIN_H12, 4}, {PIN_F15, 4}},
    {{PIN_D12, 4}, {PIN_H4, 4}, {PIN_H11, 4}, {PIN_F14, 4}}},
}};

}; // namespace Ostrich