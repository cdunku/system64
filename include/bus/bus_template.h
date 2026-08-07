#pragma once 

#include <stdint.h>

/*
 *
 *  Bus templates that help me for creating templates for compononents 
 *  that have pin-outs and access the address and data busses.
 *
 */

#define PINMASK(pin) (1ULL << (pin))

// The state of activity of each pin can be either active-LOW or active-HIGH
typedef enum {
  
  LO = 0,
  HI = 1,

} PIN_ACTIVITY_STATE;

// Table for storing the exact active and inactive states of pins emulating the hardware states.
typedef struct {

  bool active;
  bool inactive;

} pin_state_table_t;


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

static inline void pin_on(uint64_t *pins, uint64_t pin) { *pins |= pin; }
static inline void pin_off(uint64_t *pins, uint64_t pin) { *pins &= ~pin; }

static inline void set_pin(uint64_t *pins, const pin_state_table_t *table, PIN_ACTIVITY_STATE state, uint64_t pin) {
  PIN_ACTIVITY_STATE level = (state == HI) ? table[pin].active : table[pin].inactive;
  if(level) { *pins |= PINMASK(pin); } else { *pins &= ~(PINMASK(pin)); } 
}

// Returns a 1 if the pin is active or a 0 if it is inactive
static inline bool pin_check(uint64_t pins, const pin_state_table_t *table, uint64_t pin) { 
  return (((uint64_t)table[pin].active << pin) == (pins & PINMASK(pin)));
}



