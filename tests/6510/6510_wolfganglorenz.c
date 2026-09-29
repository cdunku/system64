#include <stdlib.h>
#include <string.h>

#include "6510_test_helpers.h"


// Source: https://www.softwolves.com/arkiv/cbm-hackers/7/7114.html

// Guide on how to set up the Wolfgang Lorenz 6502 Tests:
static inline void wolfgang_lorenz_init(m65xx_t *m) {

  m6510_test_init(m);
  memset(test_ram, 0, sizeof(test_ram)); 

  // Initialise some memory locations 
  test_ram[0x0002] = 0x00;
  test_ram[0xA002] = 0x00;
  test_ram[0xA003] = 0x80; 
  test_ram[0xFFFE] = 0x48;
  test_ram[0xFFFF] = 0xFF;
  test_ram[0x01FE] = 0xFF;
  test_ram[0x01FF] = 0x7F;

  // Set up the KERNAL "IRQ Handler"
  test_ram[0xFF48] = 0x48;
  test_ram[0xFF49] = 0x8A;
  test_ram[0xFF4A] = 0x48;
  test_ram[0xFF4B] = 0x98;
  test_ram[0xFF4C] = 0x48;
  test_ram[0xFF4D] = 0xBA;
  test_ram[0xFF4E] = 0xBD;
  test_ram[0xFF4F] = 0x04;
  test_ram[0xFF50] = 0x01;
  test_ram[0xFF51] = 0x29;
  test_ram[0xFF52] = 0x10;
  test_ram[0xFF53] = 0xF0;
  test_ram[0xFF54] = 0x03;
  test_ram[0xFF55] = 0x6C;
  test_ram[0xFF56] = 0x16;
  test_ram[0xFF57] = 0x03;
  test_ram[0xFF58] = 0x6C;
  test_ram[0xFF59] = 0x14;
  test_ram[0xFF5A] = 0x03;

  // Trap functions (JAM)
  test_ram[0xFFD2] = 0x02;
  test_ram[0xE16F] = 0x02;
  test_ram[0xFFE4] = 0x02;
  test_ram[0x8000] = 0x02;
  test_ram[0xA474] = 0x02;

  // Registers needed to initialise
  m->p = IDF | UF;
  m->pc = 0x0801;
}

// Loads a file from the Wolfgang Lorenz test suite
static int wolfgang_lorenz_load(char *filename) {

  char path[1024];
  snprintf(path, sizeof(path), "tests/6510/bin/wolfganglorenz6502/%s", filename);

  FILE *f = fopen(path, "rb");
  if(f == NULL) { 
    fprintf(stderr, "Error: unable to load Lorenz's \'%s\'\n", filename);
    return 1;
  }

  // Reads the first 2 bytes from start (since the beginning address vectors are located there) 
  uint8_t addr_byte[2];
  // fread returns the number of values read
  if(fread(addr_byte, 1, 2, f) != 2) {
    fprintf(stderr, "Error: unable to read Lorenz's address vectors \'%s\'\n", filename);
    fclose(f);
    return 2;
  }
  uint16_t addr = (addr_byte[1] << 8) | addr_byte[0];

  fread(&test_ram[addr], 1, sizeof(test_ram) - addr, f);

  fclose(f);
  return 0;
}

static inline char petscii_to_ascii(uint8_t data) {
  // Use this as a reference guide for values of upper and lower characters:
  // https://everything.explained.today/PETSCII/
  // https://www.pagetable.com/c64ref/charset/

  uint8_t c = 0;
  if(data >= 0x41 && data <= 0x5A) {
    c = data + 0x20;
  }
  else if(data >= 0xC1 && data <= 0xDA) {
    c = data - 0x80;
  }
  else if(data == 0x0D) {
    // Return or newline character
    c = '\n';
  }
  else if(data >= 0x20 && data <= 0x3F) {
    c = data;
  }
  return c;
}

static inline void wolfgang_lorenz_trap_handler(m65xx_t* m, char *filename) {
  if(m->pc == 0xFFD2) {

    wb(0x030C, 0);
    m->adl = rb(0x100 | ++m->s);
    m->adh = rb(0x100 | ++m->s);

    m->pc = m->ad + 1;

    char c = petscii_to_ascii(m->a); 

    putchar(c);

    // Set values to memory of current events
    m6510_set_abus(m, m->pc);
    m6510_set_dbus(m, rb(m->pc));
  }
  else if(m->pc == 0xE16F) {
    
    m->adl = rb(0x00BB);
    m->adh = rb(0x00BC);
    uint8_t length = rb(0x00B7);

    // Load the file
    for(size_t i = 0; i < length; i++) {
      uint16_t address = m->ad + i;
      filename[i] = petscii_to_ascii(test_ram[address]);
    }
    filename[length] = '\0';
 
    if(strcmp(filename, "trap17") == 0) {
      fprintf(stderr, "Lorenz: passed successfully!\n");
      m->pc = 0x8000;
      return;
    }
  
    wolfgang_lorenz_init(m);

    wolfgang_lorenz_load(filename);

    m->adl = rb(0x100 | ++m->s);
    m->adh = rb(0x100 | ++m->s);

    m->pc = 0x0816;

    // Set values to memory of current events
    m6510_set_abus(m, m->pc);
    m6510_set_dbus(m, test_ram[m->pc]);
  }
  else if(m->pc == 0xFFE4) {
    m->a = 3;

    m->adl = rb(0x100 | ++m->s);
    m->adh = rb(0x100 | ++m->s);

    m->pc = m->ad + 1;
    
    // Set values to memory of current events
    m6510_set_abus(m, m->pc);
    m6510_set_dbus(m, rb(m->pc));
  }
}

void m6502_wolfgang_lorenz_test(m65xx_t* m) {
  char filename[256] = " start";

  memset(test_ram, 0, sizeof(test_ram)); 
  wolfgang_lorenz_init(m);

  if (wolfgang_lorenz_load(filename) != 0) {
    fprintf(stderr, "Error loading file!\n");
    return;
  }

  m6510_set_abus(m, m->pc);

  while(true) {

    if(m6510_check_pin(m, M6510_RW_PIN)) {
      m6510_set_dbus(m, rb(m6510_get_abus(m))); 
    }

    wolfgang_lorenz_trap_handler(m, filename);

    m6510_test_tick(m);

    if(!m6510_check_pin(m, M6510_RW_PIN)) {
      wb(m6510_get_abus(m), m6510_get_dbus(m));   
    }

    if (m->pc == 0x8000 || m->pc == 0xA474) {
      fprintf(stderr, "Lorenz test finished. Exit at %04X\n", m->pc);
      break; 
    }
  }
}

