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

} pin_active_state_t;


static inline uint16_t get_16bit(uint64_t pins, uint64_t mask, unsigned shift) {
  return (uint16_t)((pins & mask) >> shift);
}

static inline uint8_t get_8bit(uint64_t pins, uint64_t mask, unsigned shift) {
  return (uint8_t)((pins & mask) >> shift);
}

static inline void set_16bit(uint64_t *pins, uint64_t mask, unsigned shift, uint16_t addr) {
  *pins = (*pins & ~mask) | (((uint64_t)addr << shift) & mask);
}

static inline void set_8bit(uint64_t *pins, uint64_t mask, unsigned shift, uint8_t data) {
  *pins = (*pins & ~mask) | (((uint64_t)data << shift) & mask);
}

static inline void set_pin_voltage(uint64_t *pins, PIN_ACTIVITY_STATE state, uint64_t pin) {
  if(state == HI) { 
    *pins |= PINMASK(pin); // HIGH state (voltage supplied) 
  } else { 
    *pins &= ~PINMASK(pin); // LOW state (voltage not supplied)  
  }
}

// Returns a 1 if the pin is active or a 0 if it is inactive
static inline bool is_pin_asserted(uint64_t pins, const pin_active_state_t *table, uint64_t pin) { 
  return (pins & PINMASK(pin) ? HI : LO) == table[pin].active;
}



