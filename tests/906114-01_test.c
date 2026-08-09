#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "906114-01_bus.h"
#include "6510_bus.h"

#include "marko_makela.h"

void compare_to_marko_makela(c64_pla_t *pla) {
  uint16_t *mm_lut = marko_makela_lut();

  for(size_t i = 0; i < 0x10000; i++) {
    if(i == 13) {
    }
    if(mm_lut[i] == pla->pla_lut[i]) {
      continue;
    }
    else {
      printf("Problem at index (%zu): Marko's LUT=%02X, cdunku's LUT=%02X\n", i, mm_lut[i], pla->pla_lut[i]);
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

  compare_to_marko_makela(pla);

  free(pla);
  pla = NULL;
  free(m);
  m = NULL;

  return 0;
}
