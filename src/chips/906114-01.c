#include <stdlib.h>

#include "906114-01_bus.h"

#include "6510_bus.h"

static inline bool shift_input_line(uint64_t pin, size_t shift) {
  return (shift >> pin) & 1;
}

c64_pla_t *pla_init(c64_pla_t *pla, uint64_t m6510_pins) {

  for(size_t i = 0; i < LUT_SIZE; i++) {
  
    bool cas    = shift_input_line(C64_PLA_F_CASRAM_PIN, i);
    bool loram  = shift_input_line(C64_PLA_I_LORAM_PIN, i);  
    bool hiram  = shift_input_line(C64_PLA_I_HIRAM_PIN, i); 
    bool charen = shift_input_line(C64_PLA_F_CHAROM_PIN, i);
    bool a15    = shift_input_line(C64_PLA_A15_PIN, i);   
    bool a14    = shift_input_line(C64_PLA_A14_PIN, i);   
    bool a13    = shift_input_line(C64_PLA_A13_PIN, i);   
    bool a12    = shift_input_line(C64_PLA_A12_PIN, i);   
    bool ba     = shift_input_line(C64_PLA_I_BA_PIN, i);  
    bool aec    = shift_input_line(C64_PLA_I_AEC_PIN, i); 
    bool rw     = shift_input_line(M6510_RW_PIN, i); 
    bool exrom  = shift_input_line(C64_PLA_I_EXROM9_PIN, i); 
    bool game   = shift_input_line(C64_PLA_I_GAME8_PIN, i); 
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
    p[12] = loram && charen && a15 && a14 && !a13 && a12 && !aec && !rd && game;
  }
}

void pla_decode(c64_pla_t *pla, uint64_t pins) {



}
