#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "6510.h"

#include "6510_test_helpers.h"

int main(void) {
  
  m65xx_t m;

  clock_t start = clock();
  int pass = 0;
  
  // Runs all tests at once for TomHarte:
  
  printf("Starting 6502 test...\n");

  for(int i = 0; i < 0x100; i++) {
    char file[50];
    snprintf(file, sizeof(file), "tests/6510/bin/tomharte6502-v1/%02x.json", i);
    pass += m6502_v1_harte_tests(&m, file);
  }
  
  printf("Tests passed = %d\n", pass);
  
  // For running a test for a specific opcode:
  // Pass: 1 (All tests pass), Pass: 0 (A test has failed)
  /*
  pass = m65xx_harte_tests(&m, "tests/6510/bin/tomharte6502-v1/00.json");
  printf("Pass: %d, opcode: 0x%02X\n", pass, m.ir);
  */
  // AllSuiteA test
  allsuiteasm(&m);

  // Timing test for legal opcodes
  m6502_timing_test(&m);

  // Klaus Dormann test
  m6502_decimal_test(&m);
  m6502_functional_test(&m);
  m6502_interrupt_test(&m);

  m6502_wolfgang_lorenz_test(&m);

  clock_t end = clock();

  double time = (double)(end - start)/ CLOCKS_PER_SEC;
  printf("Tests completed in %.4f\n", time);
  return 0;
}

