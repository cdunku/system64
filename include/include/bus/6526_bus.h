#pragma once

#include "6526.h"

#include "bus_template.h"

#define C6526_PINS_AMOUNT 36

typedef enum {

  CIA_PA_SHIFT = 0,
  CIA_PB_SHIFT = 8,
  CIA_DBUS_SHIFT = 16,
  CIA_RS_SHIFT = 24,

  CIA_PA_MASK    =  0xFFULL << CIA_PA_SHIFT,
  CIA_PB_MASK    =  0xFFULL << CIA_PB_SHIFT,
  CIA_DBUS_MASK  =  0xFFULL << CIA_DBUS_SHIFT,
  CIA_RS_MASK    =  0x0FULL << CIA_RS_SHIFT, 


  // Port A Pins
  CIA_PA0_PIN = 0;
  CIA_PA1_PIN,
  CIA_PA2_PIN,
  CIA_PA3_PIN,
  CIA_PA4_PIN,
  CIA_PA5_PIN,
  CIA_PA6_PIN,
  CIA_PA7_PIN,

  // Port B Pins
  CIA_PB0_PIN,
  CIA_PB1_PIN,
  CIA_PB2_PIN,
  CIA_PB3_PIN,
  CIA_PB4_PIN,
  CIA_PB5_PIN,
  CIA_PB6_PIN,
  CIA_PB7_PIN,

  // Data bus Pins
  CIA_D0_PIN,
  CIA_D1_PIN,
  CIA_D2_PIN,
  CIA_D3_PIN,
  CIA_D4_PIN,
  CIA_D5_PIN,
  CIA_D6_PIN,
  CIA_D7_PIN,

  // Register Select pins
  CIA_RS0_PIN,
  CIA_RS1_PIN,
  CIA_R2_PIN,
  CIA_R3_PIN,

  CIA_PC_PIN, // Port Control (Handshaking)
  CIA_TOD_PIN, // Time of Day
 
  CIA_IRQ_PIN, // IRQ (connected to the CPU)
  CIA_RW_PIN, // Read/Write

  CIA_CS_PIN, // Chip Select
  CIA_FLAG_PIN, // Flag port
  CIA_RES_PIN, // Reset pin
  CIA_SP_PIN, // Serial port

  CIA_NT_PIN, // Counter

} CIA_PINOUT;

pin_active_state_t c6526_pin_active_state[C6526_PINS_AMOUNT];



void c6526_set_pin(c6526_t *cia, PIN_ACTIVITY_STATE, uint64_t pin);
bool c6526_check_pin(c6526_t* cia, uint64_t pin);
