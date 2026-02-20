#include <stdint.h>

#include "c64_bus.h"

/*
*
* C64 related bus-access functions
*
*/


uint8_t c64_read(c64_t* c, uint16_t addr) {
  return c->ram[addr];
}
void c64_write(c64_t* c, uint16_t addr, uint8_t data) {
  c->ram[addr] = data;
}

