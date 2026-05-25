#include <stdint.h>

#include "c64_bus.h"
#include "vic-ii_bus.h"

/*
*
* C64 related bus-access functions
*
*/


uint8_t c64_read(c64_t* c64, uint16_t addr) {
  if(0xD000 <= addr && addr <= 0xD3FF) {
    return vic_get_reg(c64->vic, addr);
  }
  else if(0xD400 <= addr && addr <= 0xD7FF) {

  }
  else if(0xD800 <= addr && addr <= 0xD8FF) {

  }
  else if(0xDC00 <= addr && addr <= 0xDCFF) {

  }
  else if(0xDD00 <= addr && addr <= 0xDDFF) {

  }
  else if(0xDE00 <= addr && addr <= 0xDEFF) {

  }
  else if(0xDF00 <= addr && addr <= 0xDFFF) {
    return vic_read_color_ram(c64->vic, addr);
  }
}
void c64_write(c64_t* c64, uint16_t addr, uint8_t data) {
  if(0xD000 <= addr && addr <= 0xD3FF) {
    vic_set_reg(c64->vic, addr, data);
  }
  else if(0xD400 <= addr && addr <= 0xD7FF) {

  }
  else if(0xD800 <= addr && addr <= 0xD8FF) {

  }
  else if(0xDC00 <= addr && addr <= 0xDCFF) {

  }
  else if(0xDD00 <= addr && addr <= 0xDDFF) {

  }
  else if(0xDE00 <= addr && addr <= 0xDEFF) {

  }
  else if(0xDF00 <= addr && addr <= 0xDFFF) {
    vic_write_color_ram(c64->vic, addr, data);
  }
}

