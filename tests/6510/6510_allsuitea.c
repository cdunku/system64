#include "6510_test_helpers.h"

int allsuiteasm(m65xx_t* m) {
  
  m6510_test_init(m);

  load_file("tests/6510/bin/AllSuiteA.bin", 0x4000);

  while(true) {
    do {  
      if(m6510_check_pin(m, M6510_RW_PIN)) {
        m6510_set_dbus(m, rb(m6510_get_abus(m)));
      }
      m6510_test_tick(m);
      if(!m6510_check_pin(m, M6510_RW_PIN)) {
        wb(m6510_get_abus(m), m6510_get_dbus(m));
      }
    } while (!m6510_check_pin(m, M6510_SYNC_PIN));
    if(m->pc == 0x45C0) {
      if(rb(0x0210) == 0xFF) {
        printf("AllSuiteASM passed!\n");
        break;
      }
      else {
        fprintf(stderr, "AllSuiteASM failed!\n");
        break;
      }
    }
  }
  return 0;
}
