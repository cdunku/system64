#include "6510_test_helpers.h"

int m6502_timing_test(m65xx_t* m) {
   
  m6510_test_init(m); 

  load_file("tests/6510/bin/timingtest-1.bin", 0x1000); 

  m6510_set_abus(m, m->pc = 0x1000);

  size_t expected_cyc = 1141LU;
  while (true) {

    do {  
      if(m6510_check_pin(m, M6510_RW_PIN)) {
        m6510_set_dbus(m, rb(m6510_get_abus(m)));
      }
      m6510_test_tick(m);
      if(!m6510_check_pin(m, M6510_RW_PIN)) {
        wb(m6510_get_abus(m), m6510_get_dbus(m));
      }
    } while (!m6510_check_pin(m, M6510_SYNC_PIN));

    if (m->pc == 0x1269) {
      printf("%s", m->cpu_clock == expected_cyc ? "Timing test passed!\n" : "Timing Test failed!\n");
      break;
    }
  }
  return 0; 
}
