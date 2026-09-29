#pragma once

#include <stdio.h>
#include <stdint.h>

#include "6510.h"

extern uint8_t test_ram[0x10000];


// Memory operations for the RAM and CPU
uint8_t rb(uint16_t addr);
void wb(uint16_t addr, uint8_t data);

// Load a specific file into memory
int load_file(char *file, uint16_t addr);

// Basic initialisation for a CPU test suite.
void m6510_test_init(m65xx_t* m);

// Tick the 6510 without interrupts and I/O
void m6510_test_tick(m65xx_t* const m);

// All related test functions

int allsuiteasm(m65xx_t *m);
int m6502_timing_test(m65xx_t *m);
int m6502_functional_test(m65xx_t *m);
int m6502_decimal_test(m65xx_t *m);
int m6502_interrupt_test(m65xx_t *m);
int m6502_v1_harte_tests(m65xx_t *m, char *file);
void m6502_wolfgang_lorenz_test(m65xx_t *m);
