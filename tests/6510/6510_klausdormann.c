#include "6510_test_helpers.h"

int m6502_functional_test(m65xx_t* m) {
  
  m6510_test_init(m);

  load_file("tests/6510/bin/klausdormann6502/6502_functional_test.bin", 0x0);

  m6510_set_abus(m, m->pc = 0x400);

  uint16_t pc_ = 0;
  
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

    if(pc_ == m->pc) {
      if(m->pc == 0x3469) {
        printf("Klaus Dormann 6502 Functional test passed!\n");
        break;
      }
      printf("Klaus Dormann 6502 Functional test not passed! (trapped at 0x%04X)\n", m->pc);
      break;
  }
    pc_ = m->pc;
  }
  return 0;
}

int m6502_decimal_test(m65xx_t* m) {
  
  m6510_test_init(m);

  load_file("tests/6510/bin/klausdormann6502/6502_decimal_test.bin", 0x200);

  m6510_set_abus(m, m->pc = 0x200);  

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
    if (m->pc == 0x024b) {
      printf("%s", m->a == 0 ? "Klaus Dormann 6502 Decimal test passed!\n" : "Klaus Dormann 6502 Decimal test failed!\n");
      break;
    }
  }
  return 0;
}

uint8_t inte = 0;

void m6502_interrupt_handler(m65xx_t* m) {
  if ((inte & 0x2) == 0x2) {
    m->nmi_active = 1;
    inte &= ~0x2;
    }
  else if (!(m->p & IDF) && (inte & 0x1) == 0x1) {
    m->irq_active = 1;
    inte &= ~0x1;
  }
}
int m6502_interrupt_test(m65xx_t* m) {
  
  m6510_test_init(m);

  load_file("tests/6510/bin/klausdormann6502/6502_interrupt_test.bin", 0xA);

  uint16_t pc_ = 0;
  m6510_set_abus(m, m->pc = 0x400);

  wb(0xBFFC, 0);
  
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

    inte = rb(0xBFFC);
    m6502_interrupt_handler(m);
    wb(0xBFFC, inte);
    
    if (pc_ == m->pc) {
      if (m->pc == 0x06F5) {
        printf("Klaus Dormann 6502 Interrupt test passed!\n");
        break;
      }
      printf("Klaus Dormann 6502 Interrupt test failed!\n");
      break;
      }
    pc_ = m->pc;
  }
  return 0;
}

