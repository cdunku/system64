#pragma once

#include <stdbool.h>

#include "bus_template.h"

#include "m6510.h"
#include "vic-ii.h"
#include "display.h"

#include "m6510_bus.h"
#include "vic-ii_bus.h"

typedef struct c64_t {

  m65xx_t *m6510;
  vic_ii_t *vic;

  host_display_t *display;


  uint64_t c64_pins;

  // RAM Structure:
  // 
  // BASIC RAM 
  // Usable RAM
  uint8_t ram[0x10000];

  uint8_t kernal_rom[0x1FFF];
  uint8_t basic_rom[0x1FFF];
  uint8_t char_rom[0xFFF];

  uint64_t master_clock;

  uint8_t cycles_till_cpu_freeze;
} c64_t;


uint8_t c64_read(c64_t *c, uint16_t addr);
void c64_write(c64_t *c, uint16_t addr, uint8_t data);

void on(m65xx_t* m, uint64_t pin);
void off(m65xx_t* m, uint64_t pin);


