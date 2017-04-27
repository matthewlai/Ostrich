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

namespace GpioDefs {

constexpr GPIOPortPin PIN_A0 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_A1 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_A2 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_A3 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_A4 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_A5 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_A6 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_A7 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_A8 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_A9 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_A10 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_A11 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_A12 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_A13 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_A14 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_A15 = (static_cast<uint64_t>(GPIOA) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_B0 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_B1 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_B2 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_B3 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_B4 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_B5 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_B6 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_B7 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_B8 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_B9 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_B10 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_B11 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_B12 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_B13 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_B14 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_B15 = (static_cast<uint64_t>(GPIOB) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_C0 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_C1 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_C2 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_C3 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_C4 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_C5 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_C6 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_C7 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_C8 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_C9 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_C10 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_C11 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_C12 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_C13 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_C14 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_C15 = (static_cast<uint64_t>(GPIOC) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_D0 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_D1 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_D2 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_D3 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_D4 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_D5 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_D6 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_D7 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_D8 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_D9 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_D10 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_D11 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_D12 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_D13 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_D14 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_D15 = (static_cast<uint64_t>(GPIOD) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_E0 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_E1 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_E2 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_E3 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_E4 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_E5 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_E6 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_E7 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_E8 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_E9 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_E10 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_E11 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_E12 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_E13 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_E14 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_E15 = (static_cast<uint64_t>(GPIOE) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_F0 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_F1 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_F2 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_F3 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_F4 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_F5 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_F6 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_F7 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_F8 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_F9 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_F10 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_F11 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_F12 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_F13 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_F14 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_F15 = (static_cast<uint64_t>(GPIOF) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_G0 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_G1 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_G2 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_G3 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_G4 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_G5 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_G6 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_G7 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_G8 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_G9 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_G10 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_G11 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_G12 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_G13 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_G14 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_G15 = (static_cast<uint64_t>(GPIOG) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_H0 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_H1 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_H2 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_H3 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_H4 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_H5 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_H6 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_H7 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_H8 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_H9 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_H10 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_H11 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_H12 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_H13 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_H14 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_H15 = (static_cast<uint64_t>(GPIOH) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_I0 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_I1 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_I2 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_I3 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_I4 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_I5 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_I6 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_I7 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_I8 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_I9 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_I10 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_I11 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_I12 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_I13 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_I14 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_I15 = (static_cast<uint64_t>(GPIOI) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_J0 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_J1 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_J2 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_J3 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_J4 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_J5 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_J6 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_J7 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_J8 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_J9 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_J10 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_J11 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_J12 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_J13 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_J14 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_J15 = (static_cast<uint64_t>(GPIOJ) << 32) | (1 << 15);
constexpr GPIOPortPin PIN_K0 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 0);
constexpr GPIOPortPin PIN_K1 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 1);
constexpr GPIOPortPin PIN_K2 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 2);
constexpr GPIOPortPin PIN_K3 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 3);
constexpr GPIOPortPin PIN_K4 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 4);
constexpr GPIOPortPin PIN_K5 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 5);
constexpr GPIOPortPin PIN_K6 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 6);
constexpr GPIOPortPin PIN_K7 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 7);
constexpr GPIOPortPin PIN_K8 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 8);
constexpr GPIOPortPin PIN_K9 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 9);
constexpr GPIOPortPin PIN_K10 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 10);
constexpr GPIOPortPin PIN_K11 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 11);
constexpr GPIOPortPin PIN_K12 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 12);
constexpr GPIOPortPin PIN_K13 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 13);
constexpr GPIOPortPin PIN_K14 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 14);
constexpr GPIOPortPin PIN_K15 = (static_cast<uint64_t>(GPIOK) << 32) | (1 << 15);


}; // namespace GpioDefs

}; // namespace Ostrich

#endif // __GPIO_DEFS_H__
