#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vic-ii_bus.h"
#include "vic-ii.h"

#include "m6510.h"
#include "m6510_bus.h"

vic_timing_t *vic_std_init(void) {

  vic_timing_t *vic_std = malloc(sizeof(vic_timing_t));
  memset(vic_std, 0, sizeof(vic_timing_t));

  vic_std->std = VIC_PAL;

  if(vic_std->std == VIC_PAL) {
    vic_std->cycle_per_line = 63;
    vic_std->lines_per_frame = 312;
  }

  return vic_std;
}


vic_ii_t *vic_ii_init(void) {

  vic_ii_t *vic = malloc(sizeof(vic_ii_t));
  memset(vic, 0, sizeof(vic_ii_t));

  vic->vic_time = vic_std_init();

  vic->vic_reg[CONTROL_REG1] = 0x9B;
  vic->vic_reg[CONTROL_REG2] = 0x08;

  vic->vic_reg[MEMORY_POINTER] = 0x14;

  return vic;
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

      vic->vic_reg[r] &= (~val & 0x0F);
      break;
    }
    case INTERRUPT_ENABLED: {
      vic->vic_reg[r] = (val & 0x0F) | 0xF0;

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

void vic_bank_switching(vic_ii_t *vic) {

  uint8_t bnk_ptr = vic->vic_reg[MEMORY_POINTER];

  // Video Matrix is from bits 4 to 7.
  uint8_t vm = bnk_ptr & 0xF0;
  // Character Generator Data Block is from bits 1 to 3.
  // 0th bit is unused.
  uint8_t cb = bnk_ptr & 0x0E;
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

  // Checks whether if an interrupt is set.
  // If so enable the IRQ bit in the latch.
  if(vic->vic_reg[INTERRUPT_LATCH] & 0x0F) {
    vic->vic_reg[INTERRUPT_LATCH] |= INTERRUPT_REQUEST;
  }
  // If there are no interrupts clear the IRQ bit.
  else {
    vic->vic_reg[INTERRUPT_LATCH] &= ~INTERRUPT_REQUEST;
  }
  
  // If the IRQ bit is set, IRQ pin is active
  if(vic->vic_reg[INTERRUPT_LATCH] & INTERRUPT_REQUEST) {
    vic_pin_on(vic, VIC_II_IRQ);
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
