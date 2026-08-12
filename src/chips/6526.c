#include "6526.h"

#include "6526_bus.h"


void c6526_write_reg(c6526_t* cia, uint16_t addr, uint8_t data) {

  switch(c6526_rs_get(cia)) {
    case DATA_PORT_A: {
      break;
    }
    case DATA_PORT_B: {
      break;
    }
    case DATA_DIRECTION_PORT_A: {
      break;                              
    }
    case DATA_DIRECTION_PORT_B: {
      break;
    }

    case TIMER_A_LOW_BYTE: {
      break;                           
    }
    case TIMER_A_HIGH_BYTE: {
      break;
    }
    case TIMER_B_LOW_BYTE: {
      break;                           
    }
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
      break;
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

void c6526_read_reg(c6526_t* cia, uint16_t addr) {

  switch(c6526_rs_get(cia)) {
    case DATA_PORT_A: {
      break;
    }
    case DATA_PORT_B: {
      break;
    }
    case DATA_DIRECTION_PORT_A: {
      break;                              
    }
    case DATA_DIRECTION_PORT_B: {
      break;
    }

    case TIMER_A_LOW_BYTE: {
      break;                           
    }
    case TIMER_A_HIGH_BYTE: {
      break;
    }
    case TIMER_B_LOW_BYTE: {
      break;                           
    }
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
      break;
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
