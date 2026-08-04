#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "906114-01_bus.h"

#include "6510_bus.h"

static inline bool shift_input_line(uint16_t pin, size_t shift) {
  return (shift >> pin) & 1;
}

c64_pla_t *pla_init(void) {

  c64_pla_t *pla = malloc(sizeof(c64_pla_t));
  if(pla == NULL) {
    fprintf(stderr, "Error: failed to allocate memory to the PLA\n");
    return NULL;
  }
  memset(pla, 0, sizeof(c64_pla_t));

  for(size_t i = 0; i < LUT_SIZE; i++) {
 
    // In hardware, the following pins are active-LOW.
    // But in order to make the more readable, we set each chip to be active-HIGH.
    bool cas    = shift_input_line(C64_PLA_I_CAS_PIN, i);
    bool loram  = shift_input_line(C64_PLA_I_LORAM_PIN, i);  
    bool hiram  = shift_input_line(C64_PLA_I_HIRAM_PIN, i); 
    bool charen = shift_input_line(C64_PLA_I_CHAREN_PIN, i);
    bool exrom  = shift_input_line(C64_PLA_I_EXROM9_PIN, i); 
    bool game   = shift_input_line(C64_PLA_I_GAME8_PIN, i); 
    bool aec    = shift_input_line(C64_PLA_I_AEC_PIN, i); 

    // In hardware, these pins are active-HIGH.
    bool a15    = shift_input_line(C64_PLA_A15_PIN, i);   
    bool a14    = shift_input_line(C64_PLA_A14_PIN, i);   
    bool a13    = shift_input_line(C64_PLA_A13_PIN, i);   
    bool a12    = shift_input_line(C64_PLA_A12_PIN, i);   
    bool ba     = shift_input_line(C64_PLA_I_BA_PIN, i);  
    bool rw     = shift_input_line(C64_PLA_I_RW_PIN, i); 
    bool va13   = shift_input_line(C64_PLA_VA13_PIN, i);  
    bool va12   = shift_input_line(C64_PLA_VA12_PIN, i);
    bool va14   = shift_input_line(C64_PLA_VA14_PIN, i); 

    bool p[32];

    // BASIC ROM
    p[0] = loram && hiram && a15 && !a14 && a13 && !aec && rw && game;
    
    // KERNAL ROM
    p[1] = hiram && a15 && a14 && a13 && !aec && rw && game;
    p[2] = hiram && a15 && a14 && a13 && !aec && rw && !exrom && !game;

    // CHAR ROM
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

    // Product Terms for CASROM
    p[24] = !a15 && !a14 && a12 && exrom && !game;
    p[25] = !a15 && !a14 && a13 && exrom && !game;
    p[26] = !a15 && a14 && exrom && !game;
    p[27] = a15 && !a14 && a13 && exrom && !game;
    p[28] = a15 && a14 && !a13 && !a12 && exrom && !game;

    // Unused Product Term 
    p[29] = !cas;

    // Product Term to Forwarw CAS to CASRAM
    p[30] = cas;

    // Product Term for Gateway R/W 
    p[31] = !cas && a15 && a14 && !a13 && a12 && !aec && !rw;

    uint8_t output_pins = 0;

    bool check_casram = p[0] || p[1] || p[2] || p[3] || p[4] || p[5] || p[6] || p[7] ||
                        p[9] || p[10] || p[11] || p[12] || p[13] || p[14] || p[15] || p[16] ||
                        p[17] || p[18] || p[19] || p[20] || p[21] || p[22] || p[23] ||
                        p[24] || p[25] || p[26] || p[27] || p[28] || p[30];
    if(check_casram) {
      output_pins |= C64_PLA_F_CASRAM;
    }

    if(p[0]) { 
      output_pins |= C64_PLA_F_BASIC_ROM; 
    }
    if(p[1] || p[2]) { 
      output_pins |= C64_PLA_F_KERNAL_ROM;
    }
    if(p[3] || p[4] || p[5] || p[6] || p[7]) { 
      output_pins |= C64_PLA_F_CHAROM; 
    }

    if(p[31]) { 
      output_pins |= C64_PLA_F_GRW;
    }
    if(p[9] || p[10] || p[11] || p[12] || p[13] || p[14] || p[15] || p[16] || p[17] || p[18]) { 
      output_pins |= C64_PLA_F_IO;
    }
    if(p[19] ||  p[20]) {
      output_pins |= C64_PLA_F_ROML; 
    } 
    if(p[21] || p[22] || p[23]) {
      output_pins |= C64_PLA_F_ROMH;
    }

    pla->pla_lut[i] = output_pins;
  }
  
  return pla;
}

uint8_t pla_decode(c64_pla_t *pla, uint64_t cpu_pins) {

  uint64_t temp_pins = 0;

  if(cpu_pins & (1 << 11)) { temp_pins |= C64_PLA_A12; }
  if(cpu_pins & (1 << 12)) { temp_pins |= C64_PLA_A13; }
  if(cpu_pins & (1 << 13)) { temp_pins |= C64_PLA_A14; }
  if(cpu_pins & (1 << 14)) { temp_pins |= C64_PLA_A15; }

  if(cpu_pins & M6510_RW) { temp_pins |= C64_PLA_I_RW; }
  if(cpu_pins & M6510_P0) { temp_pins |= C64_PLA_I_LORAM; }
  if(cpu_pins & M6510_P1) { temp_pins |= C64_PLA_I_HIRAM; }
  if(cpu_pins & M6510_P2) { temp_pins |= C64_PLA_I_CHAREN; }
  if(cpu_pins & M6510_AEC) { temp_pins |= C64_PLA_I_AEC; }

  uint8_t output_pins = pla->pla_lut[temp_pins & 0xFFFF];
  pla->pla_pins |= (temp_pins | output_pins);

  return output_pins;
}
