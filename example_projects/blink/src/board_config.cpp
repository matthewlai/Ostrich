#include "ostrich.h"

#include <libopencm3/stm32/rcc.h>

namespace Ostrich {
Ostrich::InitInfo GetInitInfo() {
  Ostrich::InitInfo ii;

  ii.clock_scale = rcc_3v3[RCC_CLOCK_3V3_216MHZ];
  ii.hse_mhz = 8;
  ii.use_hse = true;

  // 1 ms.
  ii.systick_period_ns = 1000000;

  return ii;
}
}
