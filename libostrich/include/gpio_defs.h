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

#ifndef __GPIO_DEFS_H__
#define __GPIO_DEFS_H__

#include <libopencm3/stm32/gpio.h>

#include <cstdint>

namespace Ostrich {

// We define a type that combines both GPIO port and pin for better user code
// readability. Upper 32-bit encode the port, and lower 32-bit encode the pin
// (bitfield).
using GPIOPortPin = uint64_t;

constexpr uint32_t UnpackPort(GPIOPortPin portpin) {
  return portpin >> 32;
}

constexpr uint16_t UnpackPin(GPIOPortPin portpin) {
  return static_cast<uint16_t>(portpin & 0xffff);
}

constexpr GPIOPortPin MakePortPin(uint32_t port, uint16_t pin) {
  return (static_cast<uint64_t>(port) << 32) | (1 << pin);
}

constexpr auto PIN_A0 = MakePortPin(GPIOA, 0);
constexpr auto PIN_A1 = MakePortPin(GPIOA, 1);
constexpr auto PIN_A2 = MakePortPin(GPIOA, 2);
constexpr auto PIN_A3 = MakePortPin(GPIOA, 3);
constexpr auto PIN_A4 = MakePortPin(GPIOA, 4);
constexpr auto PIN_A5 = MakePortPin(GPIOA, 5);
constexpr auto PIN_A6 = MakePortPin(GPIOA, 6);
constexpr auto PIN_A7 = MakePortPin(GPIOA, 7);
constexpr auto PIN_A8 = MakePortPin(GPIOA, 8);
constexpr auto PIN_A9 = MakePortPin(GPIOA, 9);
constexpr auto PIN_A10 = MakePortPin(GPIOA, 10);
constexpr auto PIN_A11 = MakePortPin(GPIOA, 11);
constexpr auto PIN_A12 = MakePortPin(GPIOA, 12);
constexpr auto PIN_A13 = MakePortPin(GPIOA, 13);
constexpr auto PIN_A14 = MakePortPin(GPIOA, 14);
constexpr auto PIN_A15 = MakePortPin(GPIOA, 15);
constexpr auto PIN_B0 = MakePortPin(GPIOB, 0);
constexpr auto PIN_B1 = MakePortPin(GPIOB, 1);
constexpr auto PIN_B2 = MakePortPin(GPIOB, 2);
constexpr auto PIN_B3 = MakePortPin(GPIOB, 3);
constexpr auto PIN_B4 = MakePortPin(GPIOB, 4);
constexpr auto PIN_B5 = MakePortPin(GPIOB, 5);
constexpr auto PIN_B6 = MakePortPin(GPIOB, 6);
constexpr auto PIN_B7 = MakePortPin(GPIOB, 7);
constexpr auto PIN_B8 = MakePortPin(GPIOB, 8);
constexpr auto PIN_B9 = MakePortPin(GPIOB, 9);
constexpr auto PIN_B10 = MakePortPin(GPIOB, 10);
constexpr auto PIN_B11 = MakePortPin(GPIOB, 11);
constexpr auto PIN_B12 = MakePortPin(GPIOB, 12);
constexpr auto PIN_B13 = MakePortPin(GPIOB, 13);
constexpr auto PIN_B14 = MakePortPin(GPIOB, 14);
constexpr auto PIN_B15 = MakePortPin(GPIOB, 15);
constexpr auto PIN_C0 = MakePortPin(GPIOC, 0);
constexpr auto PIN_C1 = MakePortPin(GPIOC, 1);
constexpr auto PIN_C2 = MakePortPin(GPIOC, 2);
constexpr auto PIN_C3 = MakePortPin(GPIOC, 3);
constexpr auto PIN_C4 = MakePortPin(GPIOC, 4);
constexpr auto PIN_C5 = MakePortPin(GPIOC, 5);
constexpr auto PIN_C6 = MakePortPin(GPIOC, 6);
constexpr auto PIN_C7 = MakePortPin(GPIOC, 7);
constexpr auto PIN_C8 = MakePortPin(GPIOC, 8);
constexpr auto PIN_C9 = MakePortPin(GPIOC, 9);
constexpr auto PIN_C10 = MakePortPin(GPIOC, 10);
constexpr auto PIN_C11 = MakePortPin(GPIOC, 11);
constexpr auto PIN_C12 = MakePortPin(GPIOC, 12);
constexpr auto PIN_C13 = MakePortPin(GPIOC, 13);
constexpr auto PIN_C14 = MakePortPin(GPIOC, 14);
constexpr auto PIN_C15 = MakePortPin(GPIOC, 15);
constexpr auto PIN_D0 = MakePortPin(GPIOD, 0);
constexpr auto PIN_D1 = MakePortPin(GPIOD, 1);
constexpr auto PIN_D2 = MakePortPin(GPIOD, 2);
constexpr auto PIN_D3 = MakePortPin(GPIOD, 3);
constexpr auto PIN_D4 = MakePortPin(GPIOD, 4);
constexpr auto PIN_D5 = MakePortPin(GPIOD, 5);
constexpr auto PIN_D6 = MakePortPin(GPIOD, 6);
constexpr auto PIN_D7 = MakePortPin(GPIOD, 7);
constexpr auto PIN_D8 = MakePortPin(GPIOD, 8);
constexpr auto PIN_D9 = MakePortPin(GPIOD, 9);
constexpr auto PIN_D10 = MakePortPin(GPIOD, 10);
constexpr auto PIN_D11 = MakePortPin(GPIOD, 11);
constexpr auto PIN_D12 = MakePortPin(GPIOD, 12);
constexpr auto PIN_D13 = MakePortPin(GPIOD, 13);
constexpr auto PIN_D14 = MakePortPin(GPIOD, 14);
constexpr auto PIN_D15 = MakePortPin(GPIOD, 15);
constexpr auto PIN_E0 = MakePortPin(GPIOE, 0);
constexpr auto PIN_E1 = MakePortPin(GPIOE, 1);
constexpr auto PIN_E2 = MakePortPin(GPIOE, 2);
constexpr auto PIN_E3 = MakePortPin(GPIOE, 3);
constexpr auto PIN_E4 = MakePortPin(GPIOE, 4);
constexpr auto PIN_E5 = MakePortPin(GPIOE, 5);
constexpr auto PIN_E6 = MakePortPin(GPIOE, 6);
constexpr auto PIN_E7 = MakePortPin(GPIOE, 7);
constexpr auto PIN_E8 = MakePortPin(GPIOE, 8);
constexpr auto PIN_E9 = MakePortPin(GPIOE, 9);
constexpr auto PIN_E10 = MakePortPin(GPIOE, 10);
constexpr auto PIN_E11 = MakePortPin(GPIOE, 11);
constexpr auto PIN_E12 = MakePortPin(GPIOE, 12);
constexpr auto PIN_E13 = MakePortPin(GPIOE, 13);
constexpr auto PIN_E14 = MakePortPin(GPIOE, 14);
constexpr auto PIN_E15 = MakePortPin(GPIOE, 15);
constexpr auto PIN_F0 = MakePortPin(GPIOF, 0);
constexpr auto PIN_F1 = MakePortPin(GPIOF, 1);
constexpr auto PIN_F2 = MakePortPin(GPIOF, 2);
constexpr auto PIN_F3 = MakePortPin(GPIOF, 3);
constexpr auto PIN_F4 = MakePortPin(GPIOF, 4);
constexpr auto PIN_F5 = MakePortPin(GPIOF, 5);
constexpr auto PIN_F6 = MakePortPin(GPIOF, 6);
constexpr auto PIN_F7 = MakePortPin(GPIOF, 7);
constexpr auto PIN_F8 = MakePortPin(GPIOF, 8);
constexpr auto PIN_F9 = MakePortPin(GPIOF, 9);
constexpr auto PIN_F10 = MakePortPin(GPIOF, 10);
constexpr auto PIN_F11 = MakePortPin(GPIOF, 11);
constexpr auto PIN_F12 = MakePortPin(GPIOF, 12);
constexpr auto PIN_F13 = MakePortPin(GPIOF, 13);
constexpr auto PIN_F14 = MakePortPin(GPIOF, 14);
constexpr auto PIN_F15 = MakePortPin(GPIOF, 15);
constexpr auto PIN_G0 = MakePortPin(GPIOG, 0);
constexpr auto PIN_G1 = MakePortPin(GPIOG, 1);
constexpr auto PIN_G2 = MakePortPin(GPIOG, 2);
constexpr auto PIN_G3 = MakePortPin(GPIOG, 3);
constexpr auto PIN_G4 = MakePortPin(GPIOG, 4);
constexpr auto PIN_G5 = MakePortPin(GPIOG, 5);
constexpr auto PIN_G6 = MakePortPin(GPIOG, 6);
constexpr auto PIN_G7 = MakePortPin(GPIOG, 7);
constexpr auto PIN_G8 = MakePortPin(GPIOG, 8);
constexpr auto PIN_G9 = MakePortPin(GPIOG, 9);
constexpr auto PIN_G10 = MakePortPin(GPIOG, 10);
constexpr auto PIN_G11 = MakePortPin(GPIOG, 11);
constexpr auto PIN_G12 = MakePortPin(GPIOG, 12);
constexpr auto PIN_G13 = MakePortPin(GPIOG, 13);
constexpr auto PIN_G14 = MakePortPin(GPIOG, 14);
constexpr auto PIN_G15 = MakePortPin(GPIOG, 15);
constexpr auto PIN_H0 = MakePortPin(GPIOH, 0);
constexpr auto PIN_H1 = MakePortPin(GPIOH, 1);
constexpr auto PIN_H2 = MakePortPin(GPIOH, 2);
constexpr auto PIN_H3 = MakePortPin(GPIOH, 3);
constexpr auto PIN_H4 = MakePortPin(GPIOH, 4);
constexpr auto PIN_H5 = MakePortPin(GPIOH, 5);
constexpr auto PIN_H6 = MakePortPin(GPIOH, 6);
constexpr auto PIN_H7 = MakePortPin(GPIOH, 7);
constexpr auto PIN_H8 = MakePortPin(GPIOH, 8);
constexpr auto PIN_H9 = MakePortPin(GPIOH, 9);
constexpr auto PIN_H10 = MakePortPin(GPIOH, 10);
constexpr auto PIN_H11 = MakePortPin(GPIOH, 11);
constexpr auto PIN_H12 = MakePortPin(GPIOH, 12);
constexpr auto PIN_H13 = MakePortPin(GPIOH, 13);
constexpr auto PIN_H14 = MakePortPin(GPIOH, 14);
constexpr auto PIN_H15 = MakePortPin(GPIOH, 15);
constexpr auto PIN_I0 = MakePortPin(GPIOI, 0);
constexpr auto PIN_I1 = MakePortPin(GPIOI, 1);
constexpr auto PIN_I2 = MakePortPin(GPIOI, 2);
constexpr auto PIN_I3 = MakePortPin(GPIOI, 3);
constexpr auto PIN_I4 = MakePortPin(GPIOI, 4);
constexpr auto PIN_I5 = MakePortPin(GPIOI, 5);
constexpr auto PIN_I6 = MakePortPin(GPIOI, 6);
constexpr auto PIN_I7 = MakePortPin(GPIOI, 7);
constexpr auto PIN_I8 = MakePortPin(GPIOI, 8);
constexpr auto PIN_I9 = MakePortPin(GPIOI, 9);
constexpr auto PIN_I10 = MakePortPin(GPIOI, 10);
constexpr auto PIN_I11 = MakePortPin(GPIOI, 11);
constexpr auto PIN_I12 = MakePortPin(GPIOI, 12);
constexpr auto PIN_I13 = MakePortPin(GPIOI, 13);
constexpr auto PIN_I14 = MakePortPin(GPIOI, 14);
constexpr auto PIN_I15 = MakePortPin(GPIOI, 15);
constexpr auto PIN_J0 = MakePortPin(GPIOJ, 0);
constexpr auto PIN_J1 = MakePortPin(GPIOJ, 1);
constexpr auto PIN_J2 = MakePortPin(GPIOJ, 2);
constexpr auto PIN_J3 = MakePortPin(GPIOJ, 3);
constexpr auto PIN_J4 = MakePortPin(GPIOJ, 4);
constexpr auto PIN_J5 = MakePortPin(GPIOJ, 5);
constexpr auto PIN_J6 = MakePortPin(GPIOJ, 6);
constexpr auto PIN_J7 = MakePortPin(GPIOJ, 7);
constexpr auto PIN_J8 = MakePortPin(GPIOJ, 8);
constexpr auto PIN_J9 = MakePortPin(GPIOJ, 9);
constexpr auto PIN_J10 = MakePortPin(GPIOJ, 10);
constexpr auto PIN_J11 = MakePortPin(GPIOJ, 11);
constexpr auto PIN_J12 = MakePortPin(GPIOJ, 12);
constexpr auto PIN_J13 = MakePortPin(GPIOJ, 13);
constexpr auto PIN_J14 = MakePortPin(GPIOJ, 14);
constexpr auto PIN_J15 = MakePortPin(GPIOJ, 15);
constexpr auto PIN_K0 = MakePortPin(GPIOK, 0);
constexpr auto PIN_K1 = MakePortPin(GPIOK, 1);
constexpr auto PIN_K2 = MakePortPin(GPIOK, 2);
constexpr auto PIN_K3 = MakePortPin(GPIOK, 3);
constexpr auto PIN_K4 = MakePortPin(GPIOK, 4);
constexpr auto PIN_K5 = MakePortPin(GPIOK, 5);
constexpr auto PIN_K6 = MakePortPin(GPIOK, 6);
constexpr auto PIN_K7 = MakePortPin(GPIOK, 7);
constexpr auto PIN_K8 = MakePortPin(GPIOK, 8);
constexpr auto PIN_K9 = MakePortPin(GPIOK, 9);
constexpr auto PIN_K10 = MakePortPin(GPIOK, 10);
constexpr auto PIN_K11 = MakePortPin(GPIOK, 11);
constexpr auto PIN_K12 = MakePortPin(GPIOK, 12);
constexpr auto PIN_K13 = MakePortPin(GPIOK, 13);
constexpr auto PIN_K14 = MakePortPin(GPIOK, 14);
constexpr auto PIN_K15 = MakePortPin(GPIOK, 15);

constexpr GPIOPortPin PIN_INVALID = ~0ULL;

}; // namespace Ostrich

#endif // __GPIO_DEFS_H__
