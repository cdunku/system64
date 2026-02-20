#pragma once 

#include <stdint.h>

/*
 *
 *  Bus templates that help me for creating templates for compononents 
 *  that have pin-outs and access the address and data busses.
 *
 */

static inline uint16_t get_abus(uint64_t pins, uint64_t mask, unsigned shift) {
  return (uint16_t)((pins & mask) >> shift);
}

static inline uint8_t get_dbus(uint64_t pins, uint64_t mask, unsigned shift) {
  return (uint8_t)((pins & mask) >> shift);
}

static inline void set_abus(uint64_t *pins, uint64_t mask, unsigned shift, uint16_t addr) {
  *pins = (*pins & ~mask) | (((uint64_t)addr << shift) & mask);
}

static inline void set_dbus(uint64_t *pins, uint64_t mask, unsigned shift, uint8_t data) {
  *pins = (*pins & ~mask) | (((uint64_t)data << shift) & mask);
}

static inline void pin_on(uint64_t *pins, uint64_t bit)  { *pins |= bit; }
static inline void pin_off(uint64_t *pins, uint64_t bit) { *pins &= ~bit; }
