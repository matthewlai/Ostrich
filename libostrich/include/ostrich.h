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

#ifndef __OSTRICH_H__
#define __OSTRICH_H__

#include "gpio.h"

#include <libopencm3/stm32/rcc.h>

namespace Ostrich {

struct ClockInfo {
	rcc_clock_scale clock_scale;
	uint8_t hse_mhz;
	bool external;
};

inline void SetupClocks(const ClockInfo& ci) {
	if (ci.external) {
		rcc_clock_setup_hse(&ci.clock_scale, ci.hse_mhz);
	} else {
		rcc_clock_setup_hsi(&ci.clock_scale);
	}
}

}; // namespace Ostrich

#endif // __OSTRICH_H__
