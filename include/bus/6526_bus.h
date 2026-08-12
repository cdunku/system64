#pragma once

#include <stdint.h>

#include "6526.h"

#include "bus_template.h"

#define C6526_PINS_AMOUNT 37

typedef enum C6526_PINOUT {

  C6526_PA_SHIFT = 0,
  C6526_PB_SHIFT = 8,
  C6526_DBUS_SHIFT = 16,
  C6526_RS_SHIFT = 24,

  C6526_PA_MASK    =  0xFFULL << C6526_PA_SHIFT,
  C6526_PB_MASK    =  0xFFULL << C6526_PB_SHIFT,
  C6526_DBUS_MASK  =  0xFFULL << C6526_DBUS_SHIFT,
  C6526_RS_MASK    =  0x0FULL << C6526_RS_SHIFT, 

  // Port A Pins
  C6526_PA0_PIN = 0,
  C6526_PA1_PIN,
  C6526_PA2_PIN,
  C6526_PA3_PIN,
  C6526_PA4_PIN,
  C6526_PA5_PIN,
  C6526_PA6_PIN,
  C6526_PA7_PIN,

  // Port B Pins
  C6526_PB0_PIN,
  C6526_PB1_PIN,
  C6526_PB2_PIN,
  C6526_PB3_PIN,
  C6526_PB4_PIN,
  C6526_PB5_PIN,
  C6526_PB6_PIN,
  C6526_PB7_PIN,

  // Data bus Pins
  C6526_D0_PIN,
  C6526_D1_PIN,
  C6526_D2_PIN,
  C6526_D3_PIN,
  C6526_D4_PIN,
  C6526_D5_PIN,
  C6526_D6_PIN,
  C6526_D7_PIN,

  // Register Select pins
  C6526_RS0_PIN,
  C6526_RS1_PIN,
  C6526_RS2_PIN,
  C6526_RS3_PIN,

  C6526_PC_PIN, // Port Control (Handshaking)
  C6526_TOD_PIN, // Time of Day
 
  C6526_IRQ_PIN, // IRQ (connected to the CPU)
  C6526_RW_PIN, // Read/Write

  C6526_CS_PIN, // Chip Select
  C6526_FLAG_PIN, // Flag port
  C6526_RES_PIN, // Reset pin
  C6526_SP_PIN, // Serial port

  C6526_NT_PIN, // Counter

} C6526_PINOUT;

extern const pin_state_table_t c6526_pin_state_table[C6526_PINS_AMOUNT];

void c6526_set_pin(c6526_t *cia, PIN_ACTIVITY_STATE, uint64_t pin);
bool c6526_check_pin(c6526_t* cia, uint64_t pin);

uint8_t c6526_pa_get(c6526_t* cia);
uint8_t c6526_pb_get(c6526_t* cia);
uint8_t c6526_rs_get(c6526_t* cia);
uint8_t c6526_dbus_get(c6526_t* cia);

void c6526_pa_set(c6526_t* cia, uint8_t data);
void c6526_pb_set(c6526_t* cia, uint8_t data);
void c6526_rs_set(c6526_t* cia, uint8_t data);
void c6526_dbus_set(c6526_t* cia, uint8_t data);

