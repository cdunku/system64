#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "6510_test_helpers.h"

#include "6510_bus.h"

uint8_t test_ram[0x10000];

// Memory operations for the RAM and CPU
uint8_t rb(uint16_t addr) {
  return test_ram[addr];
}
void wb(uint16_t addr, uint8_t data) {
  test_ram[addr] = data;
}


// Load a specific file into memory
int load_file(char *file, uint16_t addr) {
  FILE *f = fopen(file, "rb");
  if(f == NULL) {
    fprintf(stderr, "Error: file [%s] could not be opened\n", file);
    return 1;
  }
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  rewind(f);

  if(size + addr > 0x10000) {
    fprintf(stderr, "Error: file [%s] cannot be fit into memory\n", file);
    fclose(f);
    return 1;
  }

  size_t result = fread(&test_ram[addr], sizeof(uint8_t), size, f);
  if(result != size) {
    fprintf(stderr, "Error: file size not equivalent for [%s]", file);
    fclose(f);
    return 1;
  }

  fclose(f);
  return 0;
}

// Basic initialisation for a CPU test suite.
void m6510_test_init(m65xx_t *m) {

  memset(m, 0, sizeof(m65xx_t));

  memset(test_ram, 0, sizeof(test_ram));

  m6510_set_pin(m, HI, M6510_RW_PIN);
  m6510_set_pin(m, HI, M6510_SYNC_PIN);

  m->s = 0xFD;
  m->p |= UF;
  m->ir = 0x00;

  m->irq_active = m->nmi_active = 0;
}

// Tick the 6510 without interrupts and I/O
void m6510_test_tick(m65xx_t* const m) {

  if(m6510_check_pin(m, M6510_SYNC_PIN)) {
    
    m6510_set_pin(m, LO, M6510_SYNC_PIN);

    if(m->nmi_active) {
      m->ir = M6510_NMI_OPCODE;
      m->nmi_active = 0;
    }
    else if(m->irq_active) {
      m->ir = M6510_IRQ_OPCODE;
      m->irq_active = 0;
    }
    else {
      m->ir = m6510_get_dbus(m);
      m->pc++;
    }
  }
  m->tcu++;
  m->cpu_clock++;

  m6510_set_pin(m, HI, M6510_RW_PIN);

  // Call instruction/addressing mode 
  m6502_opcode_table[m->ir].mode(m);
}
