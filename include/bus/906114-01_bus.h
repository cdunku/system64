#pragma once 

#include <stdint.h>

#include "bus_template.h"

#define LUT_SIZE 0x10000

typedef enum {

  C64_PLA_FIELD_SHIFT = 0,
  C64_PLA_ABUS_SHIFT = 8,
  C64_PLA_VA_SHIFT    = 12,

  C64_PLA_FIELD_MASK = 0xFFULL << C64_PLA_FIELD_SHIFT,
  C64_PLA_ABUS_MASK  = 0xFULL << C64_PLA_ABUS_SHIFT,
  C64_PLA_VA_MASK    = 0x7ULL << C64_PLA_VA_SHIFT,

  // Individual pins inside FIELD mask, designed for output.
  
  C64_PLA_F_CASRAM_PIN = 0,
  C64_PLA_F_BASIC_ROM_PIN,
  C64_PLA_F_KERNAL_ROM_PIN,
  C64_PLA_F_CHAROM_PIN,
  C64_PLA_F_GRW_PIN,
  C64_PLA_F_IO_PIN,
  C64_PLA_F_ROML_PIN,
  C64_PLA_F_ROMH_PIN,

  // Individual pins for VA and Address Bus 

  C64_PLA_A12_PIN,
  C64_PLA_A13_PIN,
  C64_PLA_A14_PIN,
  C64_PLA_A15_PIN,

  C64_PLA_VA12_PIN,
  C64_PLA_VA13_PIN,
  C64_PLA_VA14_PIN,

  // Individual pins inside INPUT mask
  
  C64_PLA_I_CAS_PIN,
  C64_PLA_I_LORAM_PIN,
  C64_PLA_I_HIRAM_PIN,
  C64_PLA_I_CHAREN_PIN,
  C64_PLA_I_GAME8_PIN,
  C64_PLA_I_EXROM9_PIN,
  C64_PLA_I_BA_PIN,
  C64_PLA_I_AEC_PIN,
  C64_PLA_I_RW_PIN,

  C64_PLA_FE_PIN,
  C64_PLA_CE_PIN,

} C64_PLA_PINOUT;

static const uint64_t C64_PLA_F_CASRAM     = PINMASK(C64_PLA_F_CASRAM_PIN);
static const uint64_t C64_PLA_F_BASIC_ROM  = PINMASK(C64_PLA_F_BASIC_ROM_PIN);
static const uint64_t C64_PLA_F_KERNAL_ROM = PINMASK(C64_PLA_F_KERNAL_ROM_PIN);
static const uint64_t C64_PLA_F_CHAROM     = PINMASK(C64_PLA_F_CHAROM_PIN);
static const uint64_t C64_PLA_F_GRW        = PINMASK(C64_PLA_F_GRW_PIN);
static const uint64_t C64_PLA_F_IO         = PINMASK(C64_PLA_F_IO_PIN);
static const uint64_t C64_PLA_F_ROML       = PINMASK(C64_PLA_F_ROML_PIN);
static const uint64_t C64_PLA_F_ROMH       = PINMASK(C64_PLA_F_ROMH_PIN);

static const uint64_t C64_PLA_I_CAS        = PINMASK(C64_PLA_I_CAS_PIN);
static const uint64_t C64_PLA_I_LORAM      = PINMASK(C64_PLA_I_LORAM_PIN);
static const uint64_t C64_PLA_I_HIRAM      = PINMASK(C64_PLA_I_HIRAM_PIN);
static const uint64_t C64_PLA_I_CHAREN     = PINMASK(C64_PLA_I_CHAREN_PIN);
static const uint64_t C64_PLA_I_GAME8      = PINMASK(C64_PLA_I_GAME8_PIN);
static const uint64_t C64_PLA_I_EXROM9     = PINMASK(C64_PLA_I_EXROM9_PIN);
static const uint64_t C64_PLA_I_RW         = PINMASK(C64_PLA_I_RW_PIN);

static const uint64_t C64_PLA_I_BA         = PINMASK(C64_PLA_I_BA_PIN);
static const uint64_t C64_PLA_I_AEC        = PINMASK(C64_PLA_I_AEC_PIN);

static const uint64_t C64_PLA_A12          = PINMASK(C64_PLA_A12_PIN);
static const uint64_t C64_PLA_A13          = PINMASK(C64_PLA_A13_PIN);
static const uint64_t C64_PLA_A14          = PINMASK(C64_PLA_A14_PIN);
static const uint64_t C64_PLA_A15          = PINMASK(C64_PLA_A15_PIN);

static const uint64_t C64_PLA_VA12         = PINMASK(C64_PLA_VA12_PIN);
static const uint64_t C64_PLA_VA13         = PINMASK(C64_PLA_VA13_PIN);
static const uint64_t C64_PLA_VA14         = PINMASK(C64_PLA_VA14_PIN);

static const uint64_t C64_PLA_FE           = PINMASK(C64_PLA_FE_PIN);
static const uint64_t C64_PLA_CE           = PINMASK(C64_PLA_CE_PIN);

typedef struct {

  uint8_t pla_lut[LUT_SIZE];
  uint64_t pla_pins;

} c64_pla_t;
