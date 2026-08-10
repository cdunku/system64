#include "6526.h"

#include "6526_bus.h"


pin_state_table_t c6526_pin_state_table[C6526_PINS_AMOUNT] = {

  [CIA_PA0_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA1_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA2_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA3_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA4_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA5_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA6_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PA7_PIN] =  { .active = HI, .inactive = LO, },

  [CIA_PB0_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB1_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB2_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB3_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB4_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB5_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB6_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_PB7_PIN] =  { .active = HI, .inactive = LO, },

  [CIA_D0_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D1_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D2_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D3_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D4_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D5_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D6_PIN]  =  { .active = HI, .inactive = LO, },
  [CIA_D7_PIN]  =  { .active = HI, .inactive = LO, },

  [CIA_RS0_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_RS1_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_RS2_PIN] =  { .active = HI, .inactive = LO, },
  [CIA_RS3_PIN] =  { .active = HI, .inactive = LO, },

  [CIA_PC_PIN] =   { .active = LO, .inactive = HI, },
  [CIA_TOD_PIN] =  { .active = HI, .inactive = LO, },

  [CIA_IRQ_PIN] =  { .active = LO, .inactive = HI, },
  [CIA_RW_PIN] =   { .active = HI, .inactive = LO, },

  [CIA_CS_PIN] =   { .active = LO, .inactive = HI, },
  [CIA_FLAG_PIN] = { .active = LO, .inactive = HI, },
  [CIA_RES_PIN] =  { .active = LO, .inactive = HI, },
  [CIA_SP_PIN] =   { .active = HI, .inactive = LO, },
 
  [CIA_NT_PIN] =   { .active = HI, .inactive = LO, },

};



void c6526_set_pin(c6526_t *cia, PIN_ACTIVITY_STATE state, uint64_t pin) {
  set_pin_voltage(&cia->c6526_pins, state, pin);
}

bool c6526_check_pin(c6526_t *cia, uint64_t pin) {
  return is_pin_asserted(cia->c6526_pins, c6526_pin_state_table, pin);
}


