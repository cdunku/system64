#pragma once

#include "c64_bus.h"

static inline void set_vic_pins_to_main_bus(c64_t *c64) {
  c64->c64_pins |= (c64->vic->vic_pins & (M6510_RDY | M6510_AEC | M6510_IRQ));
}
