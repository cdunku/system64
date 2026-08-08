#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "906114-01_bus.h"
#include "6510_bus.h"

static inline bool output8_check_pin(uint8_t pins, uint8_t pin) {
  const pin_state_table_t *table = pla_pin_state_table; 
  return (((uint8_t)table[pin].active << pin) == (pins & PINMASK(pin)));
}

void zero_page_and_stack_acces_test(c64_pla_t *pla, m65xx_t* m) {
 
  uint16_t target_addr = 0x1FFF;
  uint8_t decoded = 0;

  for(uint16_t addr = 0x0000; addr <= target_addr; addr++) {

    m6510_set_abus(m, addr);
    m6510_set_pin(m, HI, M6510_P0_PIN);
    m6510_set_pin(m, HI, M6510_P1_PIN);
    m6510_set_pin(m, HI, M6510_P2_PIN);

    m6510_set_pin(m, HI, M6510_RW_PIN);
    m6510_set_pin(m, HI, M6510_AEC_PIN);

    pla_set_pin(pla, LO, C64_PLA_I_EXROM9_PIN);
    pla_set_pin(pla, LO, C64_PLA_I_GAME8_PIN);
    pla_set_pin(pla, LO, C64_PLA_I_CAS_PIN);

    decoded = pla_decode(pla, m->m6510_pins, 0, 0);

    assert(output8_check_pin(decoded, C64_PLA_F_CASRAM_PIN) != 0);
    assert(output8_check_pin(decoded, C64_PLA_F_BASIC_ROM_PIN) == 0);
    assert(output8_check_pin(decoded, C64_PLA_F_KERNAL_ROM_PIN) == 0);
    assert(output8_check_pin(decoded, C64_PLA_F_CHAROM_PIN) == 0);
    assert(output8_check_pin(decoded, C64_PLA_F_IO_PIN) == 0);
    assert(output8_check_pin(decoded, C64_PLA_F_ROML_PIN) == 0);
    assert(output8_check_pin(decoded, C64_PLA_F_ROMH_PIN) == 0);

    pla->pla_pins = 0;

  }

  fprintf(stderr, "(0x0000 - 0x1FFF): Zero Page and Stack PLA accesses pass the test\n");
}

/*
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

  zero_page_and_stack_acces_test(pla, m);

  //kernal_rom_access_test(pla, m);
  free(pla);
  pla = NULL;
  free(m);
  m = NULL;

  return 0;
}
