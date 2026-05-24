#pragma once

#include <stdint.h>

typedef enum {
  SPRITE_X_COORD_0         = 0x00,
  SPRITE_Y_COORD_0         = 0x01,
  SPRITE_X_COORD_1         = 0x02,
  SPRITE_Y_COORD_1         = 0x03,
  SPRITE_X_COORD_2         = 0x04,                       
  SPRITE_Y_COORD_2         = 0x05,
  SPRITE_X_COORD_3         = 0x06,
  SPRITE_Y_COORD_3         = 0x07,
  SPRITE_X_COORD_4         = 0x08,
  SPRITE_Y_COORD_4         = 0x09,
  SPRITE_X_COORD_5         = 0x0A,
  SPRITE_Y_COORD_5         = 0x0B,
  SPRITE_X_COORD_6         = 0x0C,
  SPRITE_Y_COORD_6         = 0x0D,
  SPRITE_X_COORD_7         = 0x0E,
  SPRITE_Y_COORD_7         = 0x0F,

  MSBS_X_COORD             = 0x10,
  CONTROL_REG1             = 0x11,
  RASTER_COUNTER           = 0x12,
  LPX                      = 0x13,
  LPY                      = 0x14,
 
  SPRITE_ENABLED           = 0x15,
  CONTROL_REG2             = 0x16,

  SPRITE_Y_EXPANSION       = 0x17,
  MEMORY_POINTER           = 0x18,

  INTERRUPT_LATCH          = 0x19,
  INTERRUPT_ENABLED        = 0x1A,

  SPRITE_DATA_PRIORITY     = 0x1B,
  SPRITE_MULTICOLOR        = 0x1C,
  SPRITE_X_EXPANSION       = 0x1D,
  SPRITE_SPRITE_COLLISION  = 0x1E,
  SPRITE_DATA_COLLISION    = 0x1F,

  BORDER_COLOR             = 0x20,
  BACKGROUND_COLOR_0       = 0x21,
  BACKGROUND_COLOR_1       = 0x22,
  BACKGROUND_COLOR_2       = 0x23,
  BACKGROUND_COLOR_3       = 0x24,

  SPRITE_MULTICOLOR_0      = 0x25,
  SPRITE_MULTICOLOR_1      = 0x26,

  COLOR_SPRITE_0           = 0x27,
  COLOR_SPRITE_1           = 0x28,
  COLOR_SPRITE_2           = 0x29,
  COLOR_SPRITE_3           = 0x2A,
  COLOR_SPRITE_4           = 0x2B,
  COLOR_SPRITE_5           = 0x2C,
  COLOR_SPRITE_6           = 0x2D,
  COLOR_SPRITE_7           = 0x2E,
} VIC_REGISTERS;


typedef enum { VIC_PAL, VIC_NTSC } VIC_STANDARD;

typedef struct vic_timing_t {

  VIC_STANDARD std;
  int lines_per_frame;
  int cycle_per_line;

} vic_timing_t;


// Positions of interrupt flags in the Interrupt register and Interrupt enabled registers
typedef enum {

  RASTER_INTERRUPT            = 0x01,
  SPRITE_BACKGROUND_INTERRUPT = 0x02,
  SPRITE_SPRITE_INTERRUPT     = 0x04,
  LIGHT_PEN_INTERRUPT         = 0x08,

  INTERRUPT_REQUEST           = 0x80,

} VIC_INTERRUPT_FLAGS;

typedef struct vic_ii_t {

  uint64_t vic_pins;

  // Contains all the VIC-II registers
  // from 0xD000 to 0xD03F are reserved for VIC-II registers
  uint8_t vic_reg[0x40];

  uint16_t x_pos, y_pos;
  uint16_t frame;

  // Video buffer of the VIC-II
  uint8_t video_buffer[320 * 200];

  vic_timing_t *vic_time;

} vic_ii_t;


void vic_ii_init(vic_ii_t *vic);
