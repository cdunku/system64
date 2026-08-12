#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "906114-01_bus.h"

#include "6510_bus.h"


void pla_set_pin(c64_pla_t *pla, PIN_ACTIVITY_STATE state, uint64_t pin) {
  set_pin_voltage(&pla->pla_pins, state, pin);
}
bool pla_is_pin_asserted(c64_pla_t *pla, uint64_t pin) {
  return is_pin_asserted(pla->pla_pins, pla_pin_state_table, pin);
}

// Sets the output pins after checking input combinations
static inline void output8_set_pin(uint8_t *pins8, PIN_ACTIVITY_STATE state, uint8_t pin) {
  if(state) {
    *pins8 |= PINMASK(pin);
  }
  else {
    *pins8 &= ~PINMASK(pin);
  }
}

const pin_state_table_t pla_pin_state_table[PLA_PINS_AMOUNT] = {

  [C64_PLA_F_CASRAM_PIN]     = { .active = LO, .inactive = HI },
  [C64_PLA_F_BASIC_ROM_PIN]  = { .active = LO, .inactive = HI },
  [C64_PLA_F_KERNAL_ROM_PIN] = { .active = LO, .inactive = HI },
  [C64_PLA_F_CHAROM_PIN]     = { .active = LO, .inactive = HI },
  [C64_PLA_F_GRW_PIN]        = { .active = HI, .inactive = LO },
  [C64_PLA_F_IO_PIN]         = { .active = LO, .inactive = HI },
  [C64_PLA_F_ROML_PIN]       = { .active = LO, .inactive = HI },
  [C64_PLA_F_ROMH_PIN]       = { .active = LO, .inactive = HI },

  [C64_PLA_A12_PIN]          = { .active = HI, .inactive = LO },
  [C64_PLA_A13_PIN]          = { .active = HI, .inactive = LO },
  [C64_PLA_A14_PIN]          = { .active = HI, .inactive = LO },
  [C64_PLA_A15_PIN]          = { .active = HI, .inactive = LO },

  [C64_PLA_VA12_PIN]         = { .active = HI, .inactive = LO },
  [C64_PLA_VA13_PIN]         = { .active = HI, .inactive = LO },
  [C64_PLA_VA14_PIN]         = { .active = LO, .inactive = HI },

  [C64_PLA_I_CAS_PIN]        = { .active = LO, .inactive = HI },
  [C64_PLA_I_LORAM_PIN]      = { .active = LO, .inactive = HI },
  [C64_PLA_I_HIRAM_PIN]      = { .active = LO, .inactive = HI },
  [C64_PLA_I_CHAREN_PIN]     = { .active = LO, .inactive = HI },

  [C64_PLA_I_GAME8_PIN]      = { .active = LO, .inactive = HI },
  [C64_PLA_I_EXROM9_PIN]     = { .active = LO, .inactive = HI },

  [C64_PLA_I_BA_PIN]         = { .active = HI, .inactive = LO },
  [C64_PLA_I_AEC_PIN]        = { .active = LO, .inactive = HI },
  [C64_PLA_I_RW_PIN]         = { .active = HI, .inactive = LO },

  [C64_PLA_FE_PIN]           = { .active = HI, .inactive = LO },
  [C64_PLA_CE_PIN]           = { .active = LO, .inactive = HI },

};

// We map out the pins used for INPUT in the PLA for the initialisation step.
static const uint8_t pla_input_bits[PLA_PINS_AMOUNT] = {

  [C64_PLA_A12_PIN]      = 0,
  [C64_PLA_A13_PIN]      = 1,
  [C64_PLA_A14_PIN]      = 2,
  [C64_PLA_A15_PIN]      = 3,

  [C64_PLA_VA12_PIN]     = 4,
  [C64_PLA_VA13_PIN]     = 5,
  [C64_PLA_VA14_PIN]     = 6,
  
  [C64_PLA_I_CAS_PIN]    = 7,
  [C64_PLA_I_LORAM_PIN]  = 8,
  [C64_PLA_I_HIRAM_PIN]  = 9,
  [C64_PLA_I_CHAREN_PIN] = 10,
  
  [C64_PLA_I_GAME8_PIN]  = 11,
  [C64_PLA_I_EXROM9_PIN] = 12,
  
  [C64_PLA_I_BA_PIN]     = 13,
  [C64_PLA_I_AEC_PIN]    = 14,
  [C64_PLA_I_RW_PIN]     = 15,

};


// Sets the bit position of a specific INPUT pin according to the index.
static inline bool pla_input_check(C64_PLA_PINOUT pin, size_t i) {
  return (i & (1U << pla_input_bits[pin]));
}

c64_pla_t *pla_init(void) {
  c64_pla_t *pla = malloc(sizeof(c64_pla_t));
  if(pla == NULL) {
    fprintf(stderr, "Error: failed to allocate memory to the PLA\n");
    return NULL;
  }
  memset(pla, 0, sizeof(c64_pla_t));

  for(size_t i = 0; i < LUT_SIZE; i++) {

    // In hardware, these pins are active-LOW. Inversion is necessary.
    bool cas    = !pla_input_check(C64_PLA_I_CAS_PIN, i);
    bool loram  = !pla_input_check(C64_PLA_I_LORAM_PIN, i);
    bool hiram  = !pla_input_check(C64_PLA_I_HIRAM_PIN, i);
    bool charen = !pla_input_check(C64_PLA_I_CHAREN_PIN, i);
    bool exrom  = !pla_input_check(C64_PLA_I_EXROM9_PIN, i);
    bool game   = !pla_input_check(C64_PLA_I_GAME8_PIN, i);
    bool aec    = !pla_input_check(C64_PLA_I_AEC_PIN, i);
    bool va14   = !pla_input_check(C64_PLA_VA14_PIN, i);

    // In hardware, these pins are active-HIGH.
    bool a12    = pla_input_check(C64_PLA_A12_PIN, i);    
    bool a13    = pla_input_check(C64_PLA_A13_PIN, i);
    bool a14    = pla_input_check(C64_PLA_A14_PIN, i); 
    bool a15    = pla_input_check(C64_PLA_A15_PIN, i);
    bool va12   = pla_input_check(C64_PLA_VA12_PIN, i);
    bool va13   = pla_input_check(C64_PLA_VA13_PIN, i);
    bool ba     = pla_input_check(C64_PLA_I_BA_PIN, i);
    bool rw     = pla_input_check(C64_PLA_I_RW_PIN, i);

    bool p[32];

    // BASIC ROM
    p[0] = loram && hiram && a15 && !a14 && a13 && !aec && rw && game;
    
    // KERNAL ROM
    p[1] = hiram && a15 && a14 && a13 && !aec && rw && game;
    p[2] = hiram && a15 && a14 && a13 && !aec && rw && !exrom && !game;

    // CHAROM
    p[3] = hiram && !charen && a15 && a14 && !a13 && a12 && !aec && rw && game;
    p[4] = loram && !charen && a15 && a14 && !a13 && a12 && !aec && rw && game;
    p[5] = hiram && !charen && a15 && a14 && !a13 && a12 && !aec && rw && !exrom && !game;
    p[6] = va14 && !va13 && va12 && aec && game;
    p[7] = va14 && !va13 && va12 && aec && !exrom && !game;

    // Unused Product Term 
    p[8] = cas && a15 && a14 && !a13 && a12 && !aec && !rw;

    // Product Terms for I/O chip
    p[9] = hiram && charen && a15 && a14 && !a13 && a12 && !aec && ba && rw && game;
    p[10] = hiram && charen && a15 && a14 && !a13 && a12 && !aec && !rw && game;
    p[11] = loram && charen && a15 && a14 && !a13 && a12 && !aec && ba && rw && game;
    p[12] = loram && charen && a15 && a14 && !a13 && a12 && !aec && !rw && game;
    p[13] = hiram && charen && a15 && a14 && !a13 && a12 && !aec && ba && rw && !exrom && !game;
    p[14] = hiram && charen && a15 && a14 && !a13 && a12 && !aec && !rw && !exrom && !game;
    p[15] = loram && charen && a15 && a14 && !a13 && a12 && !aec && ba && rw && !exrom && !game;
    p[16] = loram && charen && a15 && a14 && !a13 && a12 && !aec && !rw && !exrom && !game;
    p[17] = a15 && a14 && !a13 && a12 && !aec && ba && rw && exrom && !game;
    p[18] = a15 && a14 && !a13 && a12 && !aec && !rw && exrom && !game;

    // Product Terms for ROML cartridge line
    p[19] = loram && hiram && a15 && !a14 && !a13 && !aec && rw && !exrom;
    p[20] = a15 && !a14 && !a13 && !aec && exrom && !game;

    // Product Terms for ROMH cartridge line
    p[21] = hiram && a15 && !a14 && a13 && !aec && rw && !exrom && !game;
    p[22] = a15 && a14 && a13 && !aec && exrom && !game;
    p[23] = va13 && va12 && aec && exrom && !game;

    // Product Terms for CASRAM
    p[24] = !a15 && !a14 && a12 && exrom && !game;
    p[25] = !a15 && !a14 && a13 && exrom && !game;
    p[26] = !a15 && a14 && exrom && !game;
    p[27] = a15 && !a14 && a13 && exrom && !game;
    p[28] = a15 && a14 && !a13 && !a12 && exrom && !game;

    // Unused Product Term 
    p[29] = !cas;

    // Product Term to Forward CAS to CASRAM
    p[30] = cas;

    // Product Term for Gateway R/W 
    p[31] = !cas && a15 && a14 && !a13 && a12 && !aec && !rw;

    uint8_t output_pins = 0x00; 

    if(p[0] || p[1] || p[2] || p[3] || p[4] || p[5] || p[6] || p[7] ||
       p[9] || p[10] || p[11] || p[12] || p[13] || p[14] || p[15] || 
       p[16] || p[17] || p[18] || p[19] || p[20] || p[21] || p[22] ||
       p[23] || p[24] || p[25] || p[26] || p[27] || p[28] || p[30]) {
      output8_set_pin(&output_pins, LO, C64_PLA_F_CASRAM_PIN);
    }

    if(!p[0]) {  
      output8_set_pin(&output_pins, LO, C64_PLA_F_BASIC_ROM_PIN);
    }
    if(!(p[1] || p[2])) {
      output8_set_pin(&output_pins, LO, C64_PLA_F_KERNAL_ROM_PIN);
    }
    if(!(p[3] || p[4] || p[5] || p[6] || p[7])) {  
      output8_set_pin(&output_pins, LO, C64_PLA_F_CHAROM_PIN);
    }
    if(!p[31]) {
      output8_set_pin(&output_pins, HI, C64_PLA_F_GRW_PIN);
    }
    if(!(p[9] || p[10] || p[11] || p[12] || p[13] || p[14] || p[15] || p[16] || p[17] || p[18])) {  
      output8_set_pin(&output_pins, LO, C64_PLA_F_IO_PIN);
    }
    if(!(p[19] || p[20])) {
      output8_set_pin(&output_pins, LO, C64_PLA_F_ROML_PIN);
    }   
    if(!(p[21] || p[22] || p[23])) {
      output8_set_pin(&output_pins, LO, C64_PLA_F_ROMH_PIN);
    }

    pla->pla_lut[i] = output_pins;
  }
  
  return pla;
}

uint8_t pla_decode(c64_pla_t *pla, uint64_t cpu_pins, uint64_t vic_pins, uint64_t cia_pins) {


  vic_pins = cia_pins = 0;

  const pin_state_table_t *cpu_table = m6510_pin_state_table;

  if(is_pin_asserted(cpu_pins, cpu_table, M6510_A12_PIN)) { pla_set_pin(pla, HI, C64_PLA_A12_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_A13_PIN)) { pla_set_pin(pla, HI, C64_PLA_A13_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_A14_PIN)) { pla_set_pin(pla, HI, C64_PLA_A14_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_A15_PIN)) { pla_set_pin(pla, HI, C64_PLA_A15_PIN); }


  if(is_pin_asserted(cpu_pins, cpu_table, M6510_RW_PIN)) { pla_set_pin(pla, HI, C64_PLA_I_RW_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_P0_PIN)) { pla_set_pin(pla, LO, C64_PLA_I_LORAM_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_P1_PIN)) { pla_set_pin(pla, LO, C64_PLA_I_HIRAM_PIN); }
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_P2_PIN)) { pla_set_pin(pla, LO, C64_PLA_I_CHAREN_PIN); }

  // The AEC pin on the 6510 and PLA are both input signals,
  // The 6510 AEC pin is the Bus Control pin, while the PLA AEC pin is the address decoder.
  // Since all our pins are active-HIGH, when the AEC chip is active, the CPU controls it.
  // The VIC-II controls the AEC when the pin is LOW..
  if(is_pin_asserted(cpu_pins, cpu_table, M6510_AEC_PIN)) { pla_set_pin(pla, LO, C64_PLA_I_AEC_PIN); }

  pla_set_pin(pla, LO, C64_PLA_I_CAS_PIN); 
  pla_set_pin(pla, HI, C64_PLA_F_BASIC_ROM_PIN);
  // if(vic_pins & V6569_BA) { temp_pins |= C64_PLA_I_BA; }

  //if(cia_pins & C6526_VA14) { temp_pins |= C64_PLA_VA14_PIN; }
 
  uint8_t output_pins = pla->pla_lut[(pla->pla_pins >> 7) & 0xFFFF];

  return output_pins;
}
