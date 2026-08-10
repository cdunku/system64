#pragma once 

#include <stdint.h>

#include "bus_template.h"

#define LUT_SIZE 0x10000
#define PLA_PINS_AMOUNT 26

typedef enum {

  C64_PLA_FIELD_SHIFT = 0,
  C64_PLA_ABUS_SHIFT = 8,
  C64_PLA_VA_SHIFT    = 12,

  C64_PLA_FIELD_MASK = 0xFFULL << C64_PLA_FIELD_SHIFT,
  C64_PLA_ABUS_MASK  = 0xFULL << C64_PLA_ABUS_SHIFT,
  C64_PLA_VA_MASK    = 0x7ULL << C64_PLA_VA_SHIFT,

  // Individual pins inside FIELD mask, designed for output.
  
  C64_PLA_F_CASRAM_PIN = 0,
  C64_PLA_F_BASIC_ROM_PIN,
  C64_PLA_F_KERNAL_ROM_PIN,
  C64_PLA_F_CHAROM_PIN,
  C64_PLA_F_GRW_PIN,
  C64_PLA_F_IO_PIN,
  C64_PLA_F_ROML_PIN,
  C64_PLA_F_ROMH_PIN,

  // Individual pins for VA and Address Bus 

  C64_PLA_A12_PIN,
  C64_PLA_A13_PIN,
  C64_PLA_A14_PIN,
  C64_PLA_A15_PIN,

  C64_PLA_VA12_PIN,
  C64_PLA_VA13_PIN,
  C64_PLA_VA14_PIN,

  // Individual pins inside INPUT mask
  
  C64_PLA_I_CAS_PIN,
  C64_PLA_I_LORAM_PIN,
  C64_PLA_I_HIRAM_PIN,
  C64_PLA_I_CHAREN_PIN,
  C64_PLA_I_GAME8_PIN,
  C64_PLA_I_EXROM9_PIN,
  C64_PLA_I_BA_PIN,
  C64_PLA_I_AEC_PIN,
  C64_PLA_I_RW_PIN,

  C64_PLA_FE_PIN,
  C64_PLA_CE_PIN,

} C64_PLA_PINOUT;


typedef struct {

  uint8_t pla_lut[LUT_SIZE];
  uint64_t pla_pins;

} c64_pla_t;

extern const pin_state_table_t pla_pin_state_table[PLA_PINS_AMOUNT];

void pla_set_pin(c64_pla_t *pla, PIN_ACTIVITY_STATE state, uint64_t pin);
bool pla_check_pin(c64_pla_t *pla, uint64_t pin);

c64_pla_t *pla_init(void);
uint8_t pla_decode(c64_pla_t *pla, uint64_t cpu_pins, uint64_t vic_pins, uint64_t cia_pins);
