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

#ifndef __USART_H__
#define __USART_H__

#include "gpio.h"

namespace Ostrich {

constexpr int kNumUSARTs = 8;

struct USARTInfo {
  struct PinOption {
    GPIOPortPin port_pin;
    uint8_t af;
  };

  uint32_t usart_base;
  rcc_periph_clken usart_rcc;
  const char* str_name;
};

constexpr std::array<USARTInfo, kNumUSARTs> USARTInfo{{
  {USART1, RCC_USART1, "USART1"},
  {USART2, RCC_USART2, "USART2"},
  {USART3, RCC_USART3, "USART3"},
  {UART4, RCC_UART4, "UART4"},
  {UART5, RCC_UART5, "UART5"},
  {USART6, RCC_USART6, "USART6"},
  {UART7, RCC_UART7, "UART7"},
  {UART8, RCC_UART8, "UART8"},
}};

}; // namespace Ostrich

#endif // __USART_H__
