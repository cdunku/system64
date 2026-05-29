#include "vic-ii_bus.h"

void vic_set_abus(vic_ii_t* vic, uint16_t addr) {
  set_abus(&vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT, addr);
}
void vic_set_mem_dbus(vic_ii_t* vic, uint8_t data) {
  set_dbus(&vic->vic_pins, VIC_II_MEM_DBUS_MASK, VIC_II_MEM_DBUS_SHIFT, data);
}

uint16_t vic_get_abus(const vic_ii_t* vic) {
  return get_abus(vic->vic_pins, VIC_II_ABUS_MASK, VIC_II_ABUS_SHIFT);
}
uint8_t vic_get_mem_dbus(const vic_ii_t* vic) {
  return get_dbus(vic->vic_pins, VIC_II_MEM_DBUS_MASK, VIC_II_MEM_DBUS_SHIFT);
}

uint8_t vic_get_color_dbus(const vic_ii_t* vic) {
  return (get_dbus(vic->vic_pins, VIC_II_COLOR_DBUS_MASK, VIC_II_COLOR_DBUS_SHIFT) & 0x0F);
}
void vic_set_color_dbus(vic_ii_t* vic, uint8_t data) {
  set_dbus(&vic->vic_pins, VIC_II_COLOR_DBUS_MASK, VIC_II_COLOR_DBUS_SHIFT, data & 0x0F);
}


void vic_pin_on(vic_ii_t *vic, uint64_t bit) { pin_on(&vic->vic_pins, bit); }
void vic_pin_off(vic_ii_t *vic, uint64_t bit) { pin_off(&vic->vic_pins, bit); }

void vic_set_reg(vic_ii_t *vic, uint16_t addr, uint8_t val) {
  uint8_t r = (addr - 0xD000) & 0x3F;
  switch (r) {
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
    default: {
      vic->vic_reg[r] = val;
      break;
    }
  }
}

uint8_t vic_get_reg(vic_ii_t *vic, uint16_t addr) {
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

void vic_write_color_ram(vic_ii_t *vic, uint16_t addr, uint8_t val) {
  vic->color_ram[addr - 0xDF00] = val & 0x0F;
}
uint8_t vic_read_color_ram(vic_ii_t *vic, uint16_t addr) {
  return vic->color_ram[addr - 0xDF00] & 0x0F;
}
