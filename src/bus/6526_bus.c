#include "6526.h"

#include "6526_bus.h"

const pin_state_table_t c6526_pin_state_table[C6526_PINS_AMOUNT] = {

  [C6526_PA0_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA1_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA2_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA3_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA4_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA5_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA6_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PA7_PIN] =  { .active = HI, .inactive = LO, },

  [C6526_PB0_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB1_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB2_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB3_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB4_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB5_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB6_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_PB7_PIN] =  { .active = HI, .inactive = LO, },

  [C6526_D0_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D1_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D2_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D3_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D4_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D5_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D6_PIN]  =  { .active = HI, .inactive = LO, },
  [C6526_D7_PIN]  =  { .active = HI, .inactive = LO, },

  [C6526_RS0_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_RS1_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_RS2_PIN] =  { .active = HI, .inactive = LO, },
  [C6526_RS3_PIN] =  { .active = HI, .inactive = LO, },

  [C6526_PC_PIN] =   { .active = LO, .inactive = HI, },
  [C6526_TOD_PIN] =  { .active = HI, .inactive = LO, },

  [C6526_IRQ_PIN] =  { .active = LO, .inactive = HI, },
  [C6526_RW_PIN] =   { .active = HI, .inactive = LO, },

  [C6526_CS_PIN] =   { .active = LO, .inactive = HI, },
  [C6526_FLAG_PIN] = { .active = LO, .inactive = HI, },
  [C6526_RES_PIN] =  { .active = LO, .inactive = HI, },
  [C6526_SP_PIN] =   { .active = HI, .inactive = LO, },
 
  [C6526_NT_PIN] =   { .active = HI, .inactive = LO, },

};


void c6526_set_pin(c6526_t *cia, PIN_ACTIVITY_STATE state, uint64_t pin) {
  set_pin_voltage(&cia->c6526_pins, state, pin);
}

bool c6526_check_pin(c6526_t *cia, uint64_t pin) {
  return is_pin_asserted(cia->c6526_pins, c6526_pin_state_table, pin);
}

uint8_t c6526_pa_get(c6526_t* cia) {
  return get_8bit(cia->c6526_pins, C6526_PA_MASK, C6526_PA_SHIFT);
}
uint8_t c6526_pb_get(c6526_t* cia) {
  return get_8bit(cia->c6526_pins, C6526_PB_MASK, C6526_PB_SHIFT);
}
uint8_t c6526_rs_get(c6526_t* cia) {
  return get_8bit(cia->c6526_pins, C6526_RS_MASK, C6526_RS_SHIFT) & 0xF; 
}
uint8_t c6526_dbus_get(c6526_t* cia) {
  return get_8bit(cia->c6526_pins, C6526_DBUS_MASK, C6526_DBUS_SHIFT);
}

void c6526_pa_set(c6526_t* cia, uint8_t data) {
  set_8bit(&cia->c6526_pins, C6526_PA_MASK, C6526_PA_SHIFT, data);
}
void c6526_pb_set(c6526_t* cia, uint8_t data) {
  set_8bit(&cia->c6526_pins, C6526_PB_MASK, C6526_PB_SHIFT, data);
}
void c6526_rs_set(c6526_t* cia, uint8_t data) {
  set_8bit(&cia->c6526_pins, C6526_RS_MASK, C6526_RS_SHIFT, data & 0xF);
}
void c6526_dbus_set(c6526_t* cia, uint8_t data) {
  set_8bit(&cia->c6526_pins, C6526_DBUS_MASK, C6526_DBUS_SHIFT, data);
}
