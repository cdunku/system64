#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "c64.h"

int main() {

  c64_t *c64;

  c64 = c64_init();
  c64_run(c64);
  
  return 0;
}
