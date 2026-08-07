#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "906114-01_bus.h"
#include "6510_bus.h"

/*
uint8_t zero_page_and_stack_acces_test(c64_pla_t *pla, m65xx_t* m) {
  
  m6510_pin_on(m, M6510_P0);
  m6510_pin_on(m, M6510_P1);
  m6510_pin_on(m, M6510_P2);

  m6510_pin_on(m, M6510_RW);
  m6510_pin_on(m, M6510_AEC);

  pla_pin_on(pla, C64_PLA_I_EXROM9);
  pla_pin_on(pla, C64_PLA_I_GAME8);
  pla_pin_on(pla, C64_PLA_I_CAS);

  m6510_set_abus(m, 0x0000);

  uint8_t decoded = pla_decode(pla, m->m6510_pins, 0, 0);

  assert((decoded & C64_PLA_F_CASRAM) != 0);
  assert((decoded & C64_PLA_F_BASIC_ROM) == 0);
  assert((decoded & C64_PLA_F_KERNAL_ROM) == 0);
  assert((decoded & C64_PLA_F_CHAROM) == 0);
  assert((decoded & C64_PLA_F_IO) == 0);
  assert((decoded & C64_PLA_F_ROML) == 0);
  assert((decoded & C64_PLA_F_ROMH) == 0);


  fprintf(stderr, "Zero Page (0x0000) passes the test\n");

  m->m6510_pins = 0;
  pla->pla_pins = 0;

  m6510_pin_on(m, M6510_P0);
  m6510_pin_on(m, M6510_P1);
  m6510_pin_on(m, M6510_P2);

  m6510_pin_on(m, M6510_RW);

  pla_pin_on(pla, C64_PLA_I_EXROM9);
  pla_pin_on(pla, C64_PLA_I_GAME8);
  pla_pin_on(pla, C64_PLA_I_CAS);

  m6510_set_abus(m, 0x0080);

  decoded = pla_decode(pla, m->m6510_pins, 0, 0);

  assert((decoded & C64_PLA_F_CASRAM) != 0);
  assert((decoded & C64_PLA_F_BASIC_ROM) == 0);
  assert((decoded & C64_PLA_F_KERNAL_ROM) == 0);
  assert((decoded & C64_PLA_F_CHAROM) == 0);
  assert((decoded & C64_PLA_F_IO) == 0);
  assert((decoded & C64_PLA_F_ROML) == 0);
  assert((decoded & C64_PLA_F_ROMH) == 0);


  fprintf(stderr, "Zero Page (0x0080) passes the test\n");
}

uint8_t kernal_rom_access_test(c64_pla_t *pla, m65xx_t* m) {
    // Reset state
    m->m6510_pins = 0;
    pla->pla_pins = 0;

    // Enable CPU RAM/ROM banking configuration (LORAM=1, HIRAM=1, CHAREN=1)
    m6510_pin_on(m, M6510_P0);
    m6510_pin_on(m, M6510_P1);
    m6510_pin_on(m, M6510_P2);

    // Set Read operation and AEC active
    m6510_pin_on(m, M6510_RW);
    m6510_pin_on(m, M6510_AEC);

    // Set Cartridge lines and CAS active on PLA
    pla_pin_on(pla, C64_PLA_I_EXROM9);
    pla_pin_on(pla, C64_PLA_I_GAME8);
    pla_pin_on(pla, C64_PLA_I_CAS);

    // Set address to 0xE000 (Binary: 1110 0000 0000 0000)
    // A13, A14, and A15 are HIGH. A12 is LOW.
    m6510_set_abus(m, 0xE000);

    uint8_t decoded = pla_decode(pla, m->m6510_pins, 0, 0);

    // For 0xE000 with KERNAL enabled:
    // 1. CASRAM should be DISABLED (0) because ROM is mapped over it.
    // 2. KERNAL_ROM should be ENABLED (!= 0).
    assert((decoded & C64_PLA_F_CASRAM) == 0);
    assert((decoded & C64_PLA_F_KERNAL_ROM) != 0);
    
    // Other ROMs/IO should remain inactive for this address
    assert((decoded & C64_PLA_F_BASIC_ROM) == 0);
    assert((decoded & C64_PLA_F_CHAROM) == 0);
    assert((decoded & C64_PLA_F_IO) == 0);

    fprintf(stderr, "KERNAL ROM (0xE000) passes the test\n");
    return 0;
}

*/
int main(void) {

  c64_pla_t *pla = pla_init();
  m65xx_t *m = m6510_init();

  // zero_page_and_stack_acces_test(pla, m);

  //kernal_rom_access_test(pla, m);
  free(pla);
  pla = NULL;
  free(m);
  m = NULL;

  return 0;
}
