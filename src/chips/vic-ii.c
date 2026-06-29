#include <stdio.h>
#include <string.h>

#include "vic-ii_bus.h"
#include "vic-ii.h"

#include "m6510.h"
#include "m6510_bus.h"

void vic_ii_init(vic_ii_t *vic) {

  memset(vic, 0, sizeof(*vic));

  vic->vic_time->std = VIC_PAL;

  if(vic->vic_time->std == VIC_PAL) {
    vic->vic_time->cycle_per_line = 63;
    vic->vic_time->lines_per_frame = 312;
  }

  vic->vic_reg[CONTROL_REG1] = 0x9B;
  vic->vic_reg[CONTROL_REG2] = 0x08;

  vic->vic_reg[MEMORY_POINTER] = 0x14;

}

static inline uint16_t vic_get_raster(const vic_ii_t *vic) {
  uint16_t raster = vic->vic_reg[RASTER_COUNTER];
  raster |= (vic->vic_reg[CONTROL_REG1] & 0x80) << 1;

  return raster;
}

void vic_set_reg(vic_ii_t *vic, uint16_t addr, uint8_t val) {
  uint8_t r = (addr - 0xD000) & 0x3F;
  switch (r) {
    case CONTROL_REG1: {
      vic->vic_reg[r] = val;

      vic->raster_compare = (vic->raster_compare & 0xFF) | (val & 0x80) << 1;
      break;
    }
    case RASTER_COUNTER: {
      vic->vic_reg[r] = val;

      vic->raster_compare = (vic->raster_compare & 0x100) | val;
      break;
    }
    case INTERRUPT_LATCH: {
      // Sets the corresponding flag inside the interrupt register. 
      // In the original chip:
      // 0 -> Sets the flag 
      // 1 -> Clear the flag
      //
      // But instead we will just do it vice-versa for better readability.
      vic->vic_reg[r] &= ~val;
      break;
    }
    case INTERRUPT_ENABLED: {
      vic->vic_reg[r] |= 0xF0;

      break;
    }
    default: {
      vic->vic_reg[r] = val;
      break;
    }
  }
}

uint8_t vic_get_reg(vic_ii_t *vic, uint16_t addr) {
  uint8_t r = (addr - 0xD000) & 0x3F;
  switch (r) {
    case CONTROL_REG1: {
      return (vic->vic_reg[r] & 0x7F) | ((vic->y_pos & 0x100) >> 1);
    }
    case RASTER_COUNTER: {
      return (vic->y_pos & 0xFF);
    }
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

static inline void vic_tick(vic_ii_t *vic) {

  if((vic->x_pos == 1 && vic->y_pos == vic->raster_compare) || 
     (vic->x_pos == 2 && vic->y_pos == 0 && vic->raster_compare == 0)) {
      vic->vic_reg[INTERRUPT_LATCH] |= RASTER_INTERRUPT;

      if(vic->vic_reg[INTERRUPT_ENABLED] & RASTER_INTERRUPT) {
        vic->vic_reg[INTERRUPT_LATCH] |= INTERRUPT_REQUEST;
        vic_pin_on(vic, VIC_II_IRQ);
      }
  }

  vic->x_pos++;

  if(vic->x_pos == vic->vic_time->cycle_per_line) {
    vic->x_pos = 0;
    vic->y_pos++; 

    if(vic->y_pos == vic->vic_time->lines_per_frame) {
      vic->y_pos = 0;
      vic->frame++;
    }
  }
}

void vic_ii_run(vic_ii_t *vic) {

  vic_tick(vic); 

  if(vic->vic_pins & VIC_II_BA) {
    if(vic->vic_pins & VIC_II_RW) {
      vic_set_mem_dbus(vic, vic_get_reg(vic, vic_get_abus(vic)));
    }
    else {
      vic_set_reg(vic, vic_get_abus(vic), vic_get_mem_dbus(vic));
    }
  }
}
