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

#include <iostream>
#include <string>

constexpr int kNumGpioPorts = 11;

int main() {
	for (int x = 0; x < kNumGpioPorts; ++x) {
		for (int pin = 0; pin < 16; ++pin) {
			std::cout << "constexpr GPIOPortPin PIN_" << 
				static_cast<char>('A' + x) << pin << " = (static_cast<uint64_t>(GPIO" << static_cast<char>('A' + x) << ") << 32) | (1 << " << pin << ");" << std::endl;
		}
	}
}
