#pragma once

#include <stdint.h>

typedef enum C6526_REGISTER_SIDE {

  // Refers to register pairs with A and B.
  A = 0,
  B = 1,

} C6526_REGISTER_SIDE;

typedef enum C6526_PB67 {
  PB_BIT6 = 0x40,
  PB_BIT7 = 0x80,
} C6526_PB67;

typedef struct c6526_timer_reg_t {

  // A = 0, B = 1
  
  // Active countdown counter
  union { struct { uint8_t counter_lo[2]; uint8_t counter_hi[2]; }; uint16_t counter[2]; }; 
  // Reload latch (written by CPU)
  union { struct { uint8_t latch_lo[2]; uint8_t latch_hi[2]; }; uint16_t latch[2]; };

} c6526_timer_reg_t;

typedef enum C6526_PIPELINE {
  

  // Injections from COUNT_A0 to ONE_SHOT_B0 have been documented in Wolfgang's 6526 Software Model

  COUNT_A0    = (1ULL << 0),
  COUNT_A1    = (1ULL << 1),
  COUNT_A2    = (1ULL << 2),
  COUNT_A3    = (1ULL << 3),

  COUNT_B0    = (1ULL << 4),
  COUNT_B1    = (1ULL << 5),
  COUNT_B2    = (1ULL << 6),
  COUNT_B3    = (1ULL << 7),

  LOAD_A0     = (1ULL << 8),
  LOAD_A1     = (1ULL << 9),
  LOAD_B0     = (1ULL << 10),
  LOAD_B1     = (1ULL << 11),

  PB_BIT6_HI  = (1ULL << 12),
  PB_BIT6_LO  = (1ULL << 13),
  PB_BIT7_HI  = (1ULL << 14),
  PB_BIT7_LO  = (1ULL << 15),

  INT_ASSERT_0 = (1ULL << 16),
  INT_ASSERT_1 = (1ULL << 17),

  ONE_SHOT_A0 = (1ULL << 18),
  ONE_SHOT_B0 = (1ULL << 19),

  SET_ICR_0   = (1ULL << 20),
  SET_ICR_1   = (1ULL << 21),

  READ_ICR_0  = (1ULL << 22),
  READ_ICR_1  = (1ULL << 23),

  CLEAR_ICR_0 = (1ULL << 24),
  CLEAR_ICR_1 = (1ULL << 25),
  CLEAR_ICR_2 = (1ULL << 26),

} C6526_PIPELINE;

static const uint32_t C6526_TIMER_DELAY_MASK = (INT_ASSERT_1 | PB_BIT7_LO | PB_BIT6_LO |
                                                LOAD_B1 | LOAD_A1 | 
                                                COUNT_B3 | COUNT_B2 | COUNT_B1 | 
                                                COUNT_A3 | COUNT_A2 | COUNT_A1 );

typedef struct c6526_timer_t {

  // Delay is the pipeline or the "conveyer belt" for events.
  uint32_t delay;
  // Feed is the variable that stores what should happen,
  // we only set the beginning stages of each event.
  // For an example feed |= COUNT_A0; or feed |= ONE_SHOT_A0;
  // We load directly to delay, only when a force load occurs.
  // But normally everything is loaded through feed.
  // Feed feeds delay new data every PHI2.
  uint32_t feed;

  // A dedicated memory bit is needed to act and store internal hardware flip-flops,
  // that remembers the current state of the wave (HIGH or LOW).
  // It is toggled either HIGH or LOW during every timer underflow.
  uint8_t pb67_timer_toggle;
  // It holds the current and finalized static logic (HIGH or LOW), 
  // from where the port logic can instantly read from.
  uint8_t pb67_timer_output; // PB On/Off

  // Holds the timer mode for the Timer that will use.
  uint8_t pb67_timer_mode;

  c6526_timer_reg_t *treg;

  bool is_idle;
  uint16_t idle_master_cycles;

} c6526_timer_t;

typedef struct c6526_t {

  uint64_t c6526_pins;

  uint8_t pr[2]; // Peripheral Data Register A & B
  uint8_t pr_in[2]; // Stores the inputs from external devices (Keyboard/Joystick)

  uint8_t ddr[2]; // Data Direction Register A & B

  c6526_timer_t *t;

  uint8_t tod_10th; // 10ths of seconds Register
  uint8_t tod_sec; // Seconds Register
  uint8_t tod_min; // Minutes Register
  uint8_t tod_hr; // Hours -- AM/PM Register

  uint8_t sdr; // Serial Data Register
  
  uint8_t icr; // Interrupt Control Register
  uint8_t imr; // Interrupt Mask Register
  uint8_t cr[2]; // Control Register A & B; 
  
  bool pc_rw;
  bool prev_cnt;

} c6526_t;


typedef enum C6562_REGISTER_SELECT {
  
  // IF 0th Bit is 0: A else: B
  DATA_PORT_A = 0,
  DATA_PORT_B,
  DATA_DIRECTION_PORT_A,
  DATA_DIRECTION_PORT_B,
  
  TIMER_A_LOW_BYTE,
  TIMER_B_LOW_BYTE,
  TIMER_A_HIGH_BYTE,
  TIMER_B_HIGH_BYTE,
  
  REAL_TIME_CLOCK_TENTH,
  REAL_TIME_CLOCK_SECONDS,
  REAL_TIME_CLOCK_MINUTES,
  REAL_TIME_CLOCK_HOURS,
  
  SERIAL_SHIFT_REGISTER,

  INTERRUPT_CONTROL_REGISTER,
 
  CONTROL_TIMER_A,
  CONTROL_TIMER_B,

  INTERRUPT_MASK_REGISTER,

} C6562_REGISTER_SELECT;


typedef enum C6526_INTERRUPT_CONTROL {

  UNDERFLOW_TIMER_A     = (1 << 0),
  UNDERFLOW_TIMER_B     = (1 << 1),
  
  ALARM                 = (1 << 2),
  
  SERIAL_PORT           = (1 << 3),
  
  FLAG                  = (1 << 4), // If CIA2, it is for NMI interrupts
  
  UNUSED1               = (1 << 5),
  UNUSED2               = (1 << 6),
  
  IRQ_SC                = (1 << 7), // If READ: IRQ, if WRITE: SET/CLEAR

} C6526_INTERRUPT_CONTROL;


typedef enum C6526_CONTROL_REGISTER_A {

  // START
  START_STOP_CRA                 = (1 << 0),
  // PBON
  INDICATE_TIMER_UNDERFLOW_B_CRA = (1 << 1), // PORT B (PIN 6)
  // OUTMODE
  SET_TIMER_UNDERFLOW_B_CRA      = (1 << 2), // PORT B (PIN 6)

  // RUNMODE
  TIMER_RESTART_CRA              = (1 << 3),
  // LOAD
  LOAD_LATCH_INTO_TIMER_CRA      = (1 << 4),

  // INMODE
  TIMER_POS_SLOPE_COUNT_CNT_CRA  = (1 << 5),
  // SPMODE
  SERIAL_SHIFT_REGISTER_DIR_CRA  = (1 << 6), // Serial Shift Register (Input or Output)

  //TODIN
  REAL_TIME_CLOCK_CRA            = (1 << 7),

} C6526_CONTROL_REGISTER_A;

typedef enum C6526_CONTROL_REGISTER_B {

  // START
  START_STOP_CRB                   = (1 << 0),
  // PBON
  INDICATE_TIMER_UNDERFLOW_B_CRB   = (1 << 1), // PORT B (PIN 7)
  // OUTMODE
  SET_TIMER_UNDERFLOW_B_CRB        = (1 << 2), // PORT B (PIN 7)

  // RUNMODE
  TIMER_RESTART_CRB                = (1 << 3),
  // LOAD
  LOAD_LATCH_INTO_TIMER_CRB        = (1 << 4),

  // INMODES
  TIMER_COUNTS_SYS_CYCLE_CRB       =        0, // If both bits are 0, the condition is true
  TIMER_COUNTS_POS_SLOPE_CNT_CRB   = (1 << 5),
  TIMER_COUNTS_UNDERFLOW_A_CRB     = (1 << 6),
  TIMER_COUNTS_UNDERFLOW_A_CNT_CRB = (3 << 5),

  // ALARM
  TOD_REG_TIME_OR_ALARM_CRB        = (1 << 7),

} C6526_CONTROL_REGISTER_B;


