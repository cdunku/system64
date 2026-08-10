#include "vic-ii_bus.h"

void vic_set_abus(vic_ii_t* vic, uint16_t addr) {
  set_abus(&vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT, addr);
}
void vic_set_dbus(vic_ii_t* vic, uint8_t data) {
  set_dbus(&vic->vic_pins, VIC_II_DBUS_MASK, VIC_II_DBUS_SHIFT, data);
}

uint16_t vic_get_abus(const vic_ii_t* vic) {
  return get_abus(vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT);
}
uint8_t vic_get_dbus(const vic_ii_t* vic) {
  return get_dbus(vic->vic_pins, VIC_II_DBUS_MASK, VIC_II_DBUS_SHIFT);
}

void vic_pin_on(vic_ii_t *vic, uint64_t bit) { pin_on(&vic->vic_pins, bit); }
void vic_pin_off(vic_ii_t *vic, uint64_t bit) { pin_off(&vic->vic_pins, bit); }
