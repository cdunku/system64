#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "906114-01_bus.h"
#include "6510_bus.h"

#include "marko_makela.h"

static inline bool output8_check_pin(uint8_t pins, uint8_t pin) {
  return (pins & PINMASK(pin) ? true : false); 
}

static inline bool pla_assert_pin_state(uint8_t decoded, uint64_t pin_id, PIN_ACTIVITY_STATE expected_state) {
    bool is_asserted = (output8_check_pin(decoded, pin_id) != 0);
    // If expected_state is HI, we want true; if LO, we want false
    bool pin_is_high = is_asserted; 
    return pin_is_high == (expected_state == HI);
}

void compare_to_marko_makela(c64_pla_t *pla) {
  uint16_t *mm_lut = marko_makela_lut();

  for(size_t i = 0; i < 0x10000; i++) {
    if(mm_lut[i] == pla->pla_lut[i]) {
      continue;
    }
    else {
      printf("Problem at index (%zu): marko=%02X, mine=%02X\n", i, mm_lut[i], pla->pla_lut[i]);
      free(mm_lut);
      return;
    }
  }

  printf("Passes all marko_makela tests!\n");

  free(mm_lut);
}

int main(void) {

  c64_pla_t *pla = pla_init();
  m65xx_t *m = m6510_init();

  uint64_t pla_inactive_mask = 0;

  for(uint64_t i = 0; i < PLA_PINS_AMOUNT; i++) {
    pla_inactive_mask |= (pla_pin_state_table[i].inactive << i); 
  }

  compare_to_marko_makela(pla);

  free(pla);
  pla = NULL;
  free(m);
  m = NULL;

  return 0;
}
