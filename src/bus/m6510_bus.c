#include "m6510_bus.h"

void m6510_set_abus(m65xx_t* m, uint16_t addr) { 
  set_abus(&m->m6510_pins, M6510_ABUS_MASK, M6510_ABUS_SHIFT, addr); 
}
void m6510_set_dbus(m65xx_t* m, uint8_t data) {
  set_dbus(&m->m6510_pins, M6510_DBUS_MASK, M6510_DBUS_SHIFT, data);
}
void m6510_set_abus_dbus(m65xx_t* m, uint16_t addr, uint8_t data) {
  set_abus(&m->m6510_pins, M6510_ABUS_MASK, M6510_ABUS_SHIFT, addr); 
  set_dbus(&m->m6510_pins, M6510_DBUS_MASK, M6510_DBUS_SHIFT, data);
}

uint16_t m6510_get_abus(const m65xx_t* m) {
  return get_abus(m->m6510_pins, M6510_ABUS_MASK, M6510_ABUS_SHIFT);
}
uint8_t m6510_get_dbus(const m65xx_t* m) {
  return get_dbus(m->m6510_pins, M6510_DBUS_MASK, M6510_DBUS_SHIFT);
}

void m6510_pin_on(m65xx_t *m, uint64_t bit) { pin_on(&m->m6510_pins, bit); }
void m6510_pin_off(m65xx_t *m, uint64_t bit) { pin_off(&m->m6510_pins, bit); }
