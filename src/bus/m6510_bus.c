#include "m6510.h"
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

void m6510_check_io_requests(m65xx_t* const m) {

  m6510_port_t *p = m->port;

  // Data Port -> ram[0x0001]
  if(m6510_get_abus(m) & 0x1) {
    if(m->m6510_pins & M6510_RW) {
    }
    else {

    }
  }
  // Data Direction Register -> ram[0x0000]
  else {
    if(m->m6510_pins & M6510_RW) {
      m6510_set_dbus(m, p->io_ddr);
    }
    else {
      // First we get which lines will be used for input or output
      p->io_ddr = m6510_get_dbus(m);
      // The lines that are ready for output are calcuated by checking which bits
      // in the CPU output latch and the Data Direction Register are 0 in real hardware.
      // The left side of "|" calculates the output requests of the IO and sets it accordingly,
      // while on the other hand the right side preserves and/or sets the input requests.

      p->active_lines_out = (p->out_or & p->io_ddr) | (p->active_lines_out | ~p->io_ddr);

      p->out_extern_device((p->out_or & p->io_ddr) | (USED_IO_PINS_MASK & ~p->io_ddr), p->user_data);

      // We calculate the finale state of the port lines using a multiplexer equation
      p->final_lines_state = (p->out_or & p->io_ddr) | (p->out_or & ~p->io_ddr);

    }
  }
}

