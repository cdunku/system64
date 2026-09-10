#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "6526.h"

#include "6526_bus.h"

static inline void set_pip_bit(uint32_t *pip, uint32_t mask, bool value) {
  if (value) { *pip |= mask; } else { *pip &= ~mask; }
}
static inline bool get_pip_bit(uint32_t pip, C6526_PIPELINE mask) {
  return (pip & mask) != 0;
}

static inline void set_pb_bit(uint8_t *pb, uint8_t mask, bool value) {
  if (value) { *pb |= mask; } else { *pb &= ~mask; }
}
static inline bool get_pb_bit(uint8_t pb, uint8_t mask) {
  return (pb & mask) != 0;
}
static inline void toggle_pb_bit(uint8_t *pb, uint8_t mask) {
  *pb ^= mask;
}

// What is happening here? 
// Well we fetch the value from the PR[B] latch (written by the CPU), writing the new values for OUTPUT.
// Why do we inverse DDR[B]? Well, we need mask the corresponding bits from PR to DDR for output, 
// while preserving the INPUT as well. Addtionally, we mask the corresponding bit 6 or 7 for PB from the timer mode.
// Since, the the multiplexer PB_OUTPUT has the inputs from the current bit from the PRB latch and and PB_MODE.
// Finally, since the PB_OUTPUT is directly correlated to the output of PB_MODE multiplexer. 
// (meaning the PB_OUTPUT condition depends on the output of the PB_MODE mutliplexer), we mask them to get the final value.
static inline void set_pb67_bit(c6526_t *c) {
  c6526_set_pb(c, ((c->pr[B] | ~c->ddr[B]) & ~c->t->pb67_timer_mode) | 
              (c->t->pb67_timer_output & c->t->pb67_timer_mode));
}

static inline bool get_cnt_rising(c6526_t *c) {
  bool cnt = c6526_check_pin(c, C6526_CNT_PIN);
  bool edge = cnt && !c->prev_cnt;
  c->prev_cnt = cnt;

  return edge;
}


static inline void set_timer_idle(c6526_t *c) {
  c->t->is_idle = (((c->cr[A] & START_STOP_CRA) == 0) &&
                  ((c->cr[B] & START_STOP_CRB) == 0)) ||
                  ((c->t->delay == 0) &&
                  (c->t->feed == 0));
}

static inline void check_idle_state(c6526_t *c) {
  if(c->t->is_idle) {
    if(c->t->idle_master_cycles != 0) {
      if((c->t->delay & COUNT_A3) != 0) {
        c->t->treg->counter[A] -= c->t->idle_master_cycles; 
      }
      if((c->t->delay & COUNT_B3) != 0) {
        c->t->treg->counter[B] -= c->t->idle_master_cycles; 
      }
      c->t->idle_master_cycles = 0;
    }
  }
  c->t->is_idle = false;
} 

static inline uint8_t read_pra(c6526_t* c) {
  c->pr_in[A] = (c6526_get_pa(c) & ~c->ddr[A]) | (c->pr[A] & c->ddr[A]);
  return c->pr_in[A];
}

static inline uint8_t read_prb(c6526_t *c) {
  c->pr_in[B] = ((c6526_get_pb(c) & ~c->ddr[B]) | (c->pr[B] & c->ddr[B])) & 0x3F;
  c->pr_in[B] |= c->t->pb67_timer_output & c->t->pb67_timer_toggle;
  return c->pr_in[B];
}
static inline void write_pr(c6526_t* c, uint8_t data, C6526_REGISTER_SIDE s) {

}

static inline void cra_write(c6526_t* c, uint8_t data) {


  uint32_t saved_delay = c->t->delay;
  uint32_t saved_feed = c->t->feed;

  bool pb67_changed = false;

  // In the Wolfgang 6526 Software Model, bCRA is the current, active data.
  

  // We first check for both CNT of the Timer and Start/Stop.
  // Since CNT tells is a different pulse compared to PHI2, we need to distinguish them both.
  // Start/Stop without CNT source is PHI2.
  // Start/Stop with CNT source is CNT. 
  // When the source is PHI2 we feed COUNT_A0 and COUNT_A1 into saved_delay, because that is the default pulse,
  // while also feeding COUNT_A0 to saved_feed so it can be feeded into saved_delay later.
  //
  // CNT is special, because the pulses come for external devices (asynchronous pulse), PHI2 synchronises it.
  // CNT occurs somewhere else (seperate function), where the rising-edge is actually fired.
  // Reading whatever mode this function last stored and injects COUNT_A0 into Delay. 
  // When CNT occurs, we have a specl pulse, meaning the Timer stops listening to PHI2 and listens to CNT instead.
  // This function only sets the default pipeline state or default conditions for whichever mode is written.
  //
  // We add to the Delay and Feed pipelines, because a new value has been written for the Timer,
  // and we need to set the right conditions for it.
  //
  // CNT risig-edge is checked every cycle and sets COUNT_A0 accordingly.
  // We are setting default conditions, since we are setting the right conditions here for PHI2 & START (which is the default).
  //
  // Count_A0 in saved_feed in CNT Mode is always 0.
  //
  // We check whether if CNT-source bit is 0 and Start/Stop bit is 1. That combination runs the normal, default PHI2 pulse for the timers.
  if((data & (TIMER_POS_SLOPE_COUNT_CNT_CRA | START_STOP_CRA)) == START_STOP_CRA) {   
    set_pip_bit(&saved_delay, COUNT_A0, 1);
    set_pip_bit(&saved_delay, COUNT_A1, 1);
    set_pip_bit(&saved_feed, COUNT_A0, 1);
  }
  else {
    set_pip_bit(&saved_delay, COUNT_A0, 0);
    set_pip_bit(&saved_delay, COUNT_A1, 0);
    set_pip_bit(&saved_feed, COUNT_A0, 0);
  }

  if((data & TIMER_RESTART_CRA) != 0) {
    set_pip_bit(&saved_feed, ONE_SHOT_A0, 1);
  }
  else {
    set_pip_bit(&saved_feed, ONE_SHOT_A0, 0);
  }

  if((data & LOAD_LATCH_INTO_TIMER_CRA)) {
    set_pip_bit(&saved_delay, LOAD_A0, 1);
  }

  // When PB is set through the rising edge of Start/Stop (0x01)
  if(((c->cr[A] & START_STOP_CRA) == 0) && ((data & START_STOP_CRA) == START_STOP_CRA)) {
    
    // INDICATE_TIMER_UNDERFLOW_B_CRA checks for Output Mode (0x02) 
    // SET_TIMER_UNDERFLOW_B_CRA checks the Timer Mode (0x04)
    // The final output for pb67_timer_toggle is reliant on the current output.
    if((c->cr[A] & (INDICATE_TIMER_UNDERFLOW_B_CRA | SET_TIMER_UNDERFLOW_B_CRA))
      == (INDICATE_TIMER_UNDERFLOW_B_CRA | SET_TIMER_UNDERFLOW_B_CRA) &&
      (get_pb_bit(c->t->pb67_timer_toggle, PB_BIT6))) {
      pb67_changed = true;
    }
    set_pb_bit(&c->t->pb67_timer_toggle, PB_BIT6, 1);
  }

  // Checks when a flip flop from the current and previous data in Timer A output occurs.
  if((c->cr[A] & (INDICATE_TIMER_UNDERFLOW_B_CRA | SET_TIMER_UNDERFLOW_B_CRA)) !=
    (data & (INDICATE_TIMER_UNDERFLOW_B_CRA | SET_TIMER_UNDERFLOW_B_CRA))) {
      pb67_changed = true;
  }

  if(data & INDICATE_TIMER_UNDERFLOW_B_CRA) {
    set_pb_bit(&c->t->pb67_timer_mode, PB_BIT6, 1);

    // In the Wolfgang schematic, 0x04 comes from ~Q in the JK flip flop.
    // Pulse Mode
    if((data & SET_TIMER_UNDERFLOW_B_CRA) == 0) {
      set_pb_bit(&c->t->pb67_timer_output, PB_BIT6, get_pip_bit(saved_delay, PB_BIT6_LO));
    }
    else {
    // According to the Wolfgang schematic, the chain that creates the final output for PB bit is:
    // 
    // TOGGLE -> MODE (Waveform or Pulse) -> OUTPUT
    //
    // The JK flip-flop that has toggled (~Q), but 0x04 in the live CRA byte is 1.
    // meaning in order to set the live value for PB bit with the underlying conditions we do the following:
    //
    // Toggle has occurred -> OUTMODE in CRB is 1 -> Output is directly affected from the toggle.  

    set_pb_bit(&c->t->pb67_timer_output, PB_BIT6, get_pb_bit(c->t->pb67_timer_toggle, PB_BIT6)); 
    }
  }
  else {
    set_pb_bit(&c->t->pb67_timer_mode, PB_BIT6, 0);
  }

  if(pb67_changed) {
    set_pb67_bit(c);
  }

  c->cr[A] = data;

  if(saved_feed != c->t->feed || saved_delay != c->t->delay) {
    c->t->delay = saved_delay;
    c->t->feed  = saved_feed;
    check_idle_state(c);
  }
}

static inline void crb_write(c6526_t* c, uint8_t data) {


  uint32_t saved_delay = c->t->delay;
  uint32_t saved_feed = c->t->feed;

  bool pb67_changed = false;

  

  // We first check for both CNT of the Timer and Start/Stop.
  // Since CNT tells is a different pulse compared to PHI2, we need to distinguish them both.
  // Start/Stop without CNT source is PHI2.
  // Start/Stop with CNT source is CNT. 
  // When the source is PHI2 we feed COUNT_A0 and COUNT_A1 into saved_delay, because that is the default pulse,
  // while also feeding COUNT_A0 to saved_feed so it can be feeded into saved_delay later.
  //
  // CNT is special, because the pulses come for external devices (asynchronous pulse), PHI2 synchronises it.
  // CNT occurs somewhere else (seperate function), where the rising-edge is actually fired.
  // Reading whatever mode this function last stored and injects COUNT_A0 into Delay. 
  // When CNT occurs, we have a specl pulse, meaning the Timer stops listening to PHI2 and listens to CNT instead.
  // This function only sets the default pipeline state or default conditions for whichever mode is written.
  //
  // We add to the Delay and Feed pipelines, because a new value has been written for the Timer,
  // and we need to set the right conditions for it.
  //
  // CNT risig-edge is checked every cycle and sets COUNT_A0 accordingly.
  // We are setting default conditions, since we are setting the right conditions here for PHI2 & START (which is the default).
  //
  // Count_A0 in saved_feed in CNT Mode is always 0.
  //
  // We check whether if CNT-source bit is 0 and Start/Stop bit is 1. That combination runs the normal, default PHI2 pulse for the timers.
  if((data & (TIMER_COUNTS_UNDERFLOW_A_CNT_CRB | START_STOP_CRB)) == START_STOP_CRB) {   
    set_pip_bit(&saved_delay, COUNT_B0, 1);
    set_pip_bit(&saved_delay, COUNT_B1, 1);
    set_pip_bit(&saved_feed, COUNT_B0, 1);
  }
  else {
    set_pip_bit(&saved_delay, COUNT_B0, 0);
    set_pip_bit(&saved_delay, COUNT_B1, 0);
    set_pip_bit(&saved_feed, COUNT_B0, 0);
  }

  if((data & TIMER_RESTART_CRB) != 0) {
    set_pip_bit(&saved_feed, ONE_SHOT_B0, 1);
  }
  else {
    set_pip_bit(&saved_feed, ONE_SHOT_B0, 0);
  }

  if((data & LOAD_LATCH_INTO_TIMER_CRB)) {
    set_pip_bit(&saved_delay, LOAD_B0, 1);
  }

  // When PB is set through the rising edge of Start/Stop (0x01)
  if(((c->cr[B] & START_STOP_CRB) == 0) && ((data & START_STOP_CRB) == START_STOP_CRB)) {
    
    // INDICATE_TIMER_UNDERFLOW_B_CRB checks for Output Mode (0x02) 
    // SET_TIMER_UNDERFLOW_B_CRB checks the Timer Mode (0x04)
    // The final output for pb67_timer_toggle is reliant on the current output.
    if((c->cr[B] & (INDICATE_TIMER_UNDERFLOW_B_CRB | SET_TIMER_UNDERFLOW_B_CRB))
      == (INDICATE_TIMER_UNDERFLOW_B_CRB | SET_TIMER_UNDERFLOW_B_CRB) &&
      (get_pb_bit(c->t->pb67_timer_toggle, PB_BIT7))) {
      pb67_changed = true;
    }
    set_pb_bit(&c->t->pb67_timer_toggle, PB_BIT7, 1);
  }

  // Checks when a flip flop from the current and previous data in Timer B output occurs.
  if((c->cr[B] & (INDICATE_TIMER_UNDERFLOW_B_CRB | SET_TIMER_UNDERFLOW_B_CRB)) !=
    (data & (INDICATE_TIMER_UNDERFLOW_B_CRB | SET_TIMER_UNDERFLOW_B_CRB))) {
      pb67_changed = true;
  }

  if(data & INDICATE_TIMER_UNDERFLOW_B_CRB) {
    set_pb_bit(&c->t->pb67_timer_mode, PB_BIT7, 1);

    // Pulse Mode
    if((data & SET_TIMER_UNDERFLOW_B_CRB) == 0) {
      set_pb_bit(&c->t->pb67_timer_output, PB_BIT7, get_pip_bit(saved_delay, PB_BIT7_LO));
    }
    else {
    // 
    // TOGGLE -> MODE (Waveform or Pulse) -> OUTPUT
    //
    // The JK flip-flop that has toggled (~Q), but 0x04 in the live CRB byte is 1.
    // meaning in order to set the live value for PB bit with the underlying conditions we do the following:
    //
    // Toggle has occurred -> OUTMODE in CRB is 1 -> Output is directly affected from the toggle.  

    set_pb_bit(&c->t->pb67_timer_output, PB_BIT7, get_pb_bit(c->t->pb67_timer_toggle, PB_BIT7)); 
    }
  }
  else {
    set_pb_bit(&c->t->pb67_timer_mode, PB_BIT7, 0);
  }

  if(pb67_changed) {
    set_pb67_bit(c);
  }

  c->cr[B] = data;

  if(saved_feed != c->t->feed || saved_delay != c->t->delay) {
    c->t->delay = saved_delay;
    c->t->feed  = saved_feed;
    check_idle_state(c);
  }
}

static inline uint8_t read_icr(c6526_t* c) {
  uint8_t data = c->icr;



  return data;
}

static inline void write_icr(c6526_t* c, uint8_t data) {
  // The writing to the Interrupt Mask Register heavily depends on how the value is written.
  // If bit 7 (in the IMR S/C) is 1 (Set), then it sets the 0-4 Bits the corresponding bits to 1.
  // If bit 7 (in the IMR S/C) is 0 (Clear), then it clears the bits 0-4.
  if(c->imr & IRQ_SC) {
    c->imr = data & 0x1F;
  }
  else {
    c->imr &= ~0x1F;
  }
}

void c6526_write(c6526_t* c, uint16_t addr, uint8_t data) {
  uint8_t reg = c6526_get_rs(c);

  switch(reg) {
    case DATA_PORT_A: {
      break;                
    }
    case DATA_PORT_B: { 
      break;
    }
    case DATA_DIRECTION_PORT_A: 
    case DATA_DIRECTION_PORT_B: {

      break;
    }

    case TIMER_A_LOW_BYTE:
    case TIMER_B_LOW_BYTE: {
      break;                           
    }
    case TIMER_A_HIGH_BYTE:
    case TIMER_B_HIGH_BYTE: {
      break;
    }

    case REAL_TIME_CLOCK_TENTH: {
      break;
    } 
    case REAL_TIME_CLOCK_SECONDS: {
      break;
    } 
    case REAL_TIME_CLOCK_MINUTES: {
      break;
    } 
    case REAL_TIME_CLOCK_HOURS: {
      break;
    }

    case SERIAL_SHIFT_REGISTER: {
      break;
    } 

    case INTERRUPT_CONTROL_REGISTER: {
      if(c6526_check_pin(c, C6526_FLAG_PIN)) {
        c->icr |= FLAG;
      }
      break;
    } 

    case CONTROL_TIMER_A: {
      cra_write(c, data);
      break;
    }
    case CONTROL_TIMER_B: {
      break;
    } 
    default: {
      break;
    }
  }
}

uint8_t c6526_read(c6526_t* c, uint16_t addr) {

  uint8_t reg = c6526_get_rs(c);

  switch(reg) {
    case DATA_PORT_A: {
      return read_pra(c);
    } 
    case DATA_PORT_B: {
      return read_prb(c);
    }
    case DATA_DIRECTION_PORT_A: 
    case DATA_DIRECTION_PORT_B: {

      break;
    }

    case TIMER_A_LOW_BYTE:
    case TIMER_B_LOW_BYTE: {
      break;                           
    }
    case TIMER_A_HIGH_BYTE:
    case TIMER_B_HIGH_BYTE: {
      break;
    }

    case REAL_TIME_CLOCK_TENTH: {
      break;
    } 
    case REAL_TIME_CLOCK_SECONDS: {
      break;
    } 
    case REAL_TIME_CLOCK_MINUTES: {
      break;
    } 
    case REAL_TIME_CLOCK_HOURS: {
      break;
    }

    case SERIAL_SHIFT_REGISTER: {
      break;
    } 

    case INTERRUPT_CONTROL_REGISTER: {
      // An ICR read clears it contents.
      uint8_t data = c->icr;
      c->icr = 0;

      return data;
    } 

    case CONTROL_TIMER_A: {
      break;
    }
    case CONTROL_TIMER_B: {
      break;
    } 
    default: {
      break;
    }
  }
}

/*
 *
typedef enum C6526_CONTROL_REGISTER_A {

  START_STOP_CRA                 = 0x01,
  INDICATE_TIMER_UNDERFLOW_B_CRA = 0x02, // PORT B (PIN 6)
  SET_TIMER_UNDERFLOW_B_CRA      = 0x04, // PORT B (PIN 6)

  TIMER_RESTART_CRA              = 0x08,
  LOAD_LATCH_INTO_TIMER_CRA      = 0x10,

  TIMER_POS_SLOPE_COUNT_CNT_CRA  = 0x20,

  SERIAL_SHIFT_REGISTER_DIR_CRA  = 0x40, // Serial Shift Register (Input or Output)

  REAL_TIME_CLOCK_CRA            = 0x80,

} C6526_CONTROL_REGISTER_A;

typedef enum C6526_CONTROL_REGISTER_B {

  START_STOP_CRB                   = 0x01,
  INDICATE_TIMER_UNDERFLOW_B_CRB   = 0x02, // PORT B (PIN 7)
  SET_TIMER_UNDERFLOW_B_CRB        = 0x04, // PORT B (PIN 7)

  TIMER_RESTART_CRB                = 0x08,
  LOAD_LATCH_INTO_TIMER_CRB        = 0x10,

  TIMER_COUNTS_SYS_CYCLE_CRB       = 0x20, // If both bits are 0, the condition is true
  TIMER_COUNTS_POS_SLOVE_CNT_CRB   = 0x20,
  TIMER_COUNTS_UNDERFLOW_A_CRB     = 0x40,
  TIMER_COUNTS_UNDERFLOW_A_CNT_CRB = 0x60,

  TOD_REG_TIME_OR_ALARM_CRB        = 0x80,

} C6526_CONTROL_REGISTER_B;


Step-by-step guide;

1. Decrement Counter
2. Check for underflow of Counter
	1. Underflow event
	2. Underflow interrupt
	3. Check for PBX High (if not PBX between High or Low)
	4. output PBX new state
	5. Cascade Mode
	6. Load Counter X
3. Load Latch X
4. Write changes to PB
5. Set interrupt register and Line
6. Next Clock
7. Link out of clock chain if there are no more pending events ?
8. Set necessary ticks to counter (or idle)
9. delay = newdelay


typedef enum C6526_INTERRUPT_CONTROL {

  UNDERFLOW_TIMER_A     = 0x01,
  UNDERFLOW_TIMER_B     = 0x02,
  
  ALARM                 = 0x04,
  
  SERIAL_PORT           = 0x08,
  
  FLAG                  = 0x10, // If c2, it is for NMI interrupts
  
  UNUSED1               = 0x20,
  UNUSED2               = 0x40,
  
  IRQ_SC                = 0x80, // If READ: IRQ, if WRITE: SET/CLEAR

} C6526_INTERRUPT_CONTROL;


 */


static inline void tick_timer_pip(c6526_t* c) {

  bool pb67_changed = false;

  if(get_pip_bit(c->t->delay, COUNT_A3)) {
    c->t->treg->counter[A]--;
  }

  if(get_pip_bit(c->t->delay, COUNT_A2) && c->t->treg->counter[A] == 0) {
 
    // First condition checks 0x40 from CRB, which checks for a Timer Underflow in PHI2 mode.
    // Second condition checks whether if a Timer Underflow has occurred during the rising edge of CNT.
    // Since the position is synchronised, we inject COUNT_B1 into the FEED pipeline.
    // COUNT_B0 and COUNT_A0 are injected when a CNT rising-edge occurs and it is not known when that edge occurs.
    // In order to synchronise this asynchronous signal, it takes 2 clock cycles to stabilise it. (1st to fetch CNT, 2nd to synchronise it)
    // Both conditions give the same output for Timer B input.
    if((c->cr[B] & (TIMER_COUNTS_UNDERFLOW_A_CNT_CRB | START_STOP_CRB)) == (TIMER_COUNTS_UNDERFLOW_A_CRB | START_STOP_CRB) ||
       (((c->cr[B] & (TIMER_COUNTS_UNDERFLOW_A_CNT_CRB | START_STOP_CRA)) == (TIMER_COUNTS_UNDERFLOW_A_CNT_CRB | START_STOP_CRA)) && get_cnt_rising(c))) {
      set_pip_bit(&c->t->feed, COUNT_B1, 1);
    }

    set_pip_bit(&c->t->delay, LOAD_A1, 1);

    if(get_pip_bit(c->t->feed, ONE_SHOT_A0) || get_pip_bit(c->t->delay, ONE_SHOT_A0)) {
      c->cr[A] &= ~START_STOP_CRA;

      // One shot has already occurred, since we decremented COUNT_A3
      set_pip_bit(&c->t->delay, COUNT_A0, 0);
      set_pip_bit(&c->t->delay, COUNT_A1, 0);
      set_pip_bit(&c->t->delay, COUNT_A2, 0);

      set_pip_bit(&c->t->feed, COUNT_A0, 0);
    }

    // A JK Flip Flop has occurred
    // The status of timer A (START_STOP_CRA bit) can reflect on the output of PB6.
    toggle_pb_bit(&c->t->pb67_timer_toggle, PB_BIT6);

    if(c->cr[A] & INDICATE_TIMER_UNDERFLOW_B_CRA) {
      
      // Checks if it is Toggle Mode
      if(c->cr[A] & SET_TIMER_UNDERFLOW_B_CRA) {
        // Set during the rising-edge of the 0th bit of CRA.
        // Toggled between high and low.
        toggle_pb_bit(&c->t->pb67_timer_output, PB_BIT6);
      }
      // Pulse Mode
      else {
        // Next cycle, the BIT6 will be LOW.
        // This is the normal state of the PB6 Output during underflows.
        set_pb_bit(&c->t->pb67_timer_output, PB_BIT6, 1);  
        set_pip_bit(&c->t->delay, PB_BIT6_HI, 1);
        set_pip_bit(&c->t->delay, PB_BIT6_LO, 0);
      }

      pb67_changed = true;
    }

    c->icr |= UNDERFLOW_TIMER_A;

    // IMR & 0x1 and ICR & 0x1 go through an AND gate in real hardware. 
    // But since, we set the 0th bit of ICR, we just check for the 0th bit of IMR.
    if(c->imr & UNDERFLOW_TIMER_A) {
      // Interrupt is raised with one PHI2 delay.
      set_pip_bit(&c->t->delay, INT_ASSERT_0, 1);
    }
  }

  if(get_pip_bit(c->t->delay, LOAD_A1)) {
    c->t->treg->counter[A] = c->t->treg->latch[A];
    set_pip_bit(&c->t->delay, COUNT_A2, 0);
  }


  // The Timer output for B is identical to Timer A
  if(get_pip_bit(c->t->delay, COUNT_B3)) {
    c->t->treg->counter[B]--;
  }

  if(get_pip_bit(c->t->delay, COUNT_B2) && c->t->treg->counter[B] == 0) {

    set_pip_bit(&c->t->delay, LOAD_B1, 1);

    if(get_pip_bit(c->t->feed, ONE_SHOT_B0) || get_pip_bit(c->t->delay, ONE_SHOT_B0)) {
      c->cr[B] &= ~START_STOP_CRB;

      set_pip_bit(&c->t->delay, COUNT_B0, 0);
      set_pip_bit(&c->t->delay, COUNT_B1, 0);
      set_pip_bit(&c->t->delay, COUNT_B2, 0);
      set_pip_bit(&c->t->feed, COUNT_B0, 0);
    }

    toggle_pb_bit(&c->t->pb67_timer_mode, PB_BIT7);
    if(c->cr[B] & INDICATE_TIMER_UNDERFLOW_B_CRB) {

      // Toggle
      if(c->cr[B] & SET_TIMER_UNDERFLOW_B_CRB) {
        toggle_pb_bit(&c->t->pb67_timer_output, PB_BIT7);
      }
      // Pulse
      else {
        set_pb_bit(&c->t->pb67_timer_output, PB_BIT7, 1);
        set_pip_bit(&c->t->delay, PB_BIT7_HI, 1);
        set_pip_bit(&c->t->delay, PB_BIT7_LO, 0);
      }
      pb67_changed = true;
    }

    c->icr |= UNDERFLOW_TIMER_B;
    if(c->imr & UNDERFLOW_TIMER_B) {
      set_pip_bit(&c->t->delay, INT_ASSERT_0, 1);
    }
  }
  
  if(get_pip_bit(c->t->delay, LOAD_B1)) {
    c->t->treg->counter[B] = c->t->treg->latch[B];
    set_pip_bit(&c->t->delay, COUNT_B2, 0);
  }

  // The PB bits 6 or 7 Low after the next cycle
  if(get_pip_bit(c->t->delay, PB_BIT6_LO)) {
    set_pb_bit(&c->t->pb67_timer_output, PB_BIT6, 0);
    pb67_changed = true;
  } else if(get_pip_bit(c->t->delay, PB_BIT7_LO)) {
    set_pb_bit(&c->t->pb67_timer_output, PB_BIT7, 0);
    pb67_changed = true;
  }

  if(pb67_changed) {
    set_pb67_bit(c);
  }

  // In the schematic, a logic inversion occurs. Meaning that this is set to LOW.
  // Generating an interrupt signal to the CPU.
  if(get_pip_bit(c->t->delay, INT_ASSERT_1)) {
    c->icr |= IRQ_SC;
  }


}

c6526_t* c6526_init(void) {
  c6526_t *c = malloc(sizeof(c6526_t));
  memset(c, 0, sizeof(c6526_t));
}


void c6526_tick(c6526_t *c, uint16_t addr) {

  tick_timer_pip(c);

  // Phase 2 of the clock cycle
  if(c6526_check_pin(c, C6526_CS_PIN)) {
    if(c6526_check_pin(c, C6526_RW_PIN)) {
      c6526_set_dbus(c, c6526_read(c, addr));
    }
    else {
      c6526_write(c, addr, c6526_get_dbus(c));
    }
  }
}
