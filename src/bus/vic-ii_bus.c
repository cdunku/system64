#include "vic-ii_bus.h"

void vic_set_abus(vic_ii_t* vic, uint16_t addr) {
  set_abus(&vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT, addr);
}
void vic_set_mem_dbus(vic_ii_t* vic, uint8_t data) {
  set_dbus(&vic->vic_pins, VIC_II_MEM_DBUS_MASK, VIC_II_MEM_DBUS_SHIFT, data);
}

uint16_t vic_get_abus(const vic_ii_t* vic) {
  return get_abus(vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT);
}
uint8_t vic_get_mem_dbus(const vic_ii_t* vic) {
  return get_dbus(vic->vic_pins, VIC_II_MEM_DBUS_MASK, VIC_II_MEM_DBUS_SHIFT);
}

uint8_t vic_get_color_dbus(const vic_ii_t* vic) {
  return (get_dbus(vic->vic_pins, VIC_II_COLOR_DBUS_MASK, VIC_II_COLOR_DBUS_SHIFT) & 0x0F);
}
void vic_set_color_dbus(vic_ii_t* vic, uint8_t data) {
  set_dbus(&vic->vic_pins, VIC_II_COLOR_DBUS_MASK, VIC_II_COLOR_DBUS_SHIFT, data & 0x0F);
}


void vic_pin_on(vic_ii_t *vic, uint64_t bit) { pin_on(&vic->vic_pins, bit); }
void vic_pin_off(vic_ii_t *vic, uint64_t bit) { pin_off(&vic->vic_pins, bit); }



void vic_write_color_ram(vic_ii_t *vic, uint16_t addr, uint8_t val) {
  vic->color_ram[addr - 0xD800] = (val & 0x0F) | 0xF0;
}
uint8_t vic_read_color_ram(vic_ii_t *vic, uint16_t addr) {
  return (vic->color_ram[addr - 0xD800] & 0x0F) | 0xF0;
}
