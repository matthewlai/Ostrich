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

#include "usart.h"

#include <string>

#include "ostrich.h"

extern "C" {

void usart1_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(USART1);
}

void usart2_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(USART2);
}

void usart3_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(USART3);
}

void uart4_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(UART4);
}

void uart5_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(UART5);
}

void usart6_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(USART6);
}

void uart7_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(UART7);
}

void uart8_isr(void) {
  Ostrich::USARTManager::GetInstance().InvokeCallback(UART8);
}

}

namespace Ostrich {

std::array<const USARTInfo, kNumUSARTs> kUSARTInfo{{
{USART1, RCC_USART1, NVIC_USART1_IRQ, "USART1",
    {{GpioDef::PIN_B14, 4}, {GpioDef::PIN_A9, 7}, {GpioDef::PIN_B6, 7}},
    {{GpioDef::PIN_B15, 4}, {GpioDef::PIN_A10, 7}, {GpioDef::PIN_B7, 7}}},
{USART2, RCC_USART2, NVIC_USART2_IRQ, "USART2",
    {{GpioDef::PIN_A2, 7}, {GpioDef::PIN_D5, 7}},
    {{GpioDef::PIN_A3, 7}, {GpioDef::PIN_D6, 7}}},
{USART3, RCC_USART3, NVIC_USART3_IRQ, "USART3",
    {{GpioDef::PIN_B10, 7}, {GpioDef::PIN_C10, 7}, {GpioDef::PIN_D8, 7}},
    {{GpioDef::PIN_B11, 7}, {GpioDef::PIN_C11, 7}, {GpioDef::PIN_D9, 7}}},
{UART4, RCC_UART4, NVIC_UART4_IRQ, "UART4",
    {{GpioDef::PIN_A12, 6}, {GpioDef::PIN_A0, 8}, {GpioDef::PIN_C10, 8},
     {GpioDef::PIN_D1, 8}, {GpioDef::PIN_H13, 8}},
    {{GpioDef::PIN_A11, 6}, {GpioDef::PIN_A1, 8}, {GpioDef::PIN_C11, 8},
     {GpioDef::PIN_D0, 8}, {GpioDef::PIN_H14, 8}, {GpioDef::PIN_I9, 8}}},
{UART5, RCC_UART5, NVIC_UART5_IRQ, "UART5",
    {{GpioDef::PIN_B6, 1}, {GpioDef::PIN_B9, 7}, {GpioDef::PIN_B13, 8},
     {GpioDef::PIN_C12, 8} },
    {{GpioDef::PIN_B5, 1}, {GpioDef::PIN_B8, 7}, {GpioDef::PIN_B12, 8},
     {GpioDef::PIN_D2, 8}}},
{USART6, RCC_USART6, NVIC_USART6_IRQ, "USART6",
    {{GpioDef::PIN_C6, 8}, {GpioDef::PIN_G14, 8} },
    {{GpioDef::PIN_C7, 8}, {GpioDef::PIN_G9, 8}}},
{UART7, RCC_UART7, NVIC_UART7_IRQ, "UART7",
    {{GpioDef::PIN_E8, 8}, {GpioDef::PIN_F7, 8}, {GpioDef::PIN_A15, 12},
     {GpioDef::PIN_B4, 12}},
    {{GpioDef::PIN_E7, 8}, {GpioDef::PIN_F6, 8}, {GpioDef::PIN_A8, 12},
     {GpioDef::PIN_B3, 12}}},
{UART8, RCC_UART8, NVIC_UART8_IRQ, "UART8",
    {{GpioDef::PIN_E1, 8}},
    {{GpioDef::PIN_E0, 8}}},
}};

}; // namespace Ostrich