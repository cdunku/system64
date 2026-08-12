#pragma once

#include <stdint.h>

typedef struct {

  uint64_t c6526_pins;

  uint8_t pra; // Peripheral Data Register A 
  uint8_t prb; // Peripheral Data Register B

  uint8_t ddra; // Data Direction Register A
  uint8_t ddrb; // Data Direction Register B

  uint16_t talo; // Timer A Low Register
  uint16_t tahi; // Timer A High Register
  uint16_t tblo; // Timer B Low Register
  uint16_t tbhi; // Timer B High Register

  uint8_t tod_10th; // 10ths of seconds Register
  uint8_t tod_sec; // Seconds Register
  uint8_t tod_min; // Minutes Register
  uint8_t tod_hr; // Hours -- AM/PM Register

  uint8_t sdr; // Serial Data Register
  
  uint8_t icr; // Interrupt Control Register
  uint8_t cra; // Control Register A
  uint8_t crb; // Control Register B

} c6526_t;


typedef enum {
  
  DATA_PORT_A = 0,
  DATA_PORT_B,
  DATA_DIRECTION_PORT_A,
  DATA_DIRECTION_PORT_B,
  
  TIMER_A_LOW_BYTE,
  TIMER_A_HIGH_BYTE,
  TIMER_B_LOW_BYTE,
  TIMER_B_HIGH_BYTE,
  
  REAL_TIME_CLOCK_TENTH,
  REAL_TIME_CLOCK_SECONDS,
  REAL_TIME_CLOCK_MINUTES,
  REAL_TIME_CLOCK_HOURS,
  
  SERIAL_SHIFT_REGISTER,

  INTERRUPT_CONTROL_REGISTER,
  CONTROL_TIMER_A,
  CONTROL_TIMER_B,

} C6562_REGISTER_SELECT;
