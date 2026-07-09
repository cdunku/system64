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

void m6510_set_port(m65xx_t* const m) {
  m->m6510_pins = (uint64_t)(m->port->final_lines_state << M6510_PORT_SHIFT) & M6510_PORT_MASK;
}

/*
 * The logic behind I/O requests is relatively complicating and I have not come-up with this code.
 * This implementation has been taken directly from the floooh's m6502 implementation, 
 * but I will try my best at documenting the following code block.
 */


void m6510_check_io_requests(m65xx_t* const m) {

  m6510_port_t *p = m->port;

  // Data Port -> ram[0x0001]
  if(m6510_get_abus(m) & 0x1) {
    if(m->m6510_pins & M6510_RW) {

      // Input Operation
  
      // Fetch the input data from an external device or peripheral.
      p->in_or = p->in_extern_device(p->user_data);

      // Combine current live inputs (with the floating bits)
      // and blend the CPU's output latches based on the DDR layoout filter.
      uint8_t data = ((p->in_or | (p->active_lines_out & FLOATING_IO_PINS_MASK)) & ~p->io_ddr) | (p->out_or & p->io_ddr);
      
      m6510_set_dbus(m, data); // Store on the data bus
    }
    else {

      // Output Operation

      // Fetch the byte written by the CPU
      p->out_or = m6510_get_dbus(m);
      
      // Preserve old inputs, while updating outputs
      p->active_lines_out = (p->out_or & p->io_ddr) | (p->active_lines_out & ~p->io_ddr);

      // Finally write the usere data as input for the external devices or peripherals
      p->out_extern_device((p->out_or & p->io_ddr) | (USED_IO_PINS_MASK & ~p->io_ddr), p->user_data);
    }

    // Update the final state of lines on the motherboard for the PORT pins.
    p->final_lines_state = (p->out_or & p->io_ddr) | (p->in_or & ~p->io_ddr);
  }
  // Data Direction Register -> ram[0x0000]
  else {
    if(m->m6510_pins & M6510_RW) {
      m6510_set_dbus(m, p->io_ddr);
    }
    else {
      // Lines will be used for input or output
      p->io_ddr = m6510_get_dbus(m);

      // The lines that are ready for output are calcuated by checking which bits
      // in the CPU output latch and the Data Direction Register are 0 in real hardware.
      // The left side of "|" calculates the output requests of the IO and sets it accordingly,
      // while on the other hand the right side preserves and/or sets the input requests.
      p->active_lines_out = (p->out_or & p->io_ddr) | (p->active_lines_out & ~p->io_ddr);

      p->out_extern_device((p->out_or & p->io_ddr) | (USED_IO_PINS_MASK & ~p->io_ddr), p->user_data);

      // We calculate the finale state of the port lines using a multiplexer equation
      p->final_lines_state = (p->out_or & p->io_ddr) | (p->out_or & ~p->io_ddr);
    }
  }
}
