#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "6510_bus.h"

typedef struct m6510_port_t m6510_port_t;

static const uint8_t NF  = (1 << 7);
static const uint8_t VF  = (1 << 6);
static const uint8_t BF  = (1 << 4);
static const uint8_t DF  = (1 << 3);
static const uint8_t IDF = (1 << 2);
static const uint8_t ZF  = (1 << 1);
static const uint8_t CF  = (1 << 0);

static const uint16_t M6510_RES_OPCODE = 0x100;
static const uint16_t M6510_NMI_OPCODE = 0x101;
static const uint16_t M6510_IRQ_OPCODE = 0x102;

typedef struct m65xx_t {
  uint8_t ram[0x10000];
  uint64_t m6510_pins;
  
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t s;
  uint8_t p;
 
  uint8_t tcu;
  uint16_t ir;
 
  union { struct { uint8_t pcl; uint8_t pch; }; uint16_t pc; };
  union { struct { uint8_t adl; uint8_t adh; }; uint16_t ad; };
  bool bra;
  uint64_t cpu_clock;

  // Checks for interrupt polling in the second-to-last cycle of each instruction 
  // (BRK and its derivatives are excluded)
  bool interrupt_poll;
  bool nmi_edge_sensitive;
  bool nmi_previous_state;
  bool nmi_active;
  bool irq_active;

  bool cpu_instr_done;

  m6510_port_t *port;

} m65xx_t;

typedef struct { 
  void (*mode)(m65xx_t*); 
  void (*instr)(m65xx_t*); 
} m65xx_opcodes_t;

extern const m65xx_opcodes_t m6502_opcode_table[0x103];

void m6510_tick(m65xx_t* const m);
m65xx_t *m6510_init(void);

