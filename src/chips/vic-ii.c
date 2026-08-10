#include <stdio.h>

#include "vic-ii.h"
#include "m6510.h"

void vic_ii_init(vic_ii_t *vic) {

  if(vic->vic_time->std == VIC_PAL) {
    vic->vic_time->cycle_per_line = 63;
    vic->vic_time->lines_per_frame = 312;
  }
}

static inline uint16_t vic_get_raster(const vic_ii_t *vic) {
  uint16_t raster = vic->vic_reg[RASTER_COUNTER];
  raster |= (vic->vic_reg[CONTROL_REG1] & 0x80) << 1;

  return raster;
}

static inline void set_vic_reg(vic_ii_t *vic, uint16_t addr, uint8_t val) {
  uint8_t r = (addr - 0xD000) & 0x3F;
  switch (r) {
    case INTERRUPT_LATCH: {
      // Sets the corresponding flag inside the interrupt register. 
      // 0 -> Sets the flag 
      // 1 -> Clear the flag
      vic->vic_reg[r] &= val;
      break;
    }
    default: {
      vic->vic_reg[r] = val;
      break;
    }
  }
}

static inline uint8_t get_vic_reg(vic_ii_t *vic, uint16_t addr) {
  uint8_t r = (addr - 0xD000) & 0x3F;
  switch (r) {
    case SPRITE_DATA_COLLISION:
    case SPRITE_SPRITE_COLLISION: {
      uint8_t sprite_collision = vic->vic_reg[r];
      vic->vic_reg[r] = 0;

      return sprite_collision;
    }

    default: {
      return vic->vic_reg[r];
    }

  }
}


void vic_tick(vic_ii_t *vic) {

  if((vic->x_pos == 1 && vic->y_pos == 0) 
    || vic->y_pos == vic_get_raster(vic)) {
     set_vic_reg(vic, INTERRUPT_LATCH, ~(RASTER_INTERRUPT | INTERRUPT_REQUEST));
  }

  vic->x_pos++;

  if(vic->x_pos == vic->vic_time->cycle_per_line) {
    vic->x_pos = 0;
    vic->y_pos++;
  } 
  else if(vic->y_pos == vic->vic_time->lines_per_frame) {
    vic->y_pos = 0;
    vic->frame++;
  }
} 
