#pragma once 

#include "m6510.h"
#include "bus_template.h"

#define LORAM 0x1
#define HIRAM 0x2
#define CHAREN 0x4

typedef struct m65xx_t m65xx_t;


typedef enum M6510_PINOUT {

  M6510_ABUS_SHIFT = 0,
  M6510_DBUS_SHIFT = 16,
  M6510_PORT_SHIFT = 30,

  M6510_ABUS_MASK  = 0xFFFFULL << M6510_ABUS_SHIFT,
  M6510_DBUS_MASK  = 0xFFULL   << M6510_DBUS_SHIFT,
  M6510_PORT_MASK  = 0x3FULL   << M6510_PORT_SHIFT,

  // control pins start AFTER DBUS
  M6510_RDY_PIN = 24,
  M6510_IRQ_PIN,
  M6510_RW_PIN,
  M6510_AEC_PIN,
  
  M6510_NMI_PIN,
  M6510_SYNC_PIN,
  M6510_RES_PIN,

  M6510_P0_PIN,
  M6510_P1_PIN,
  M6510_P2_PIN,
  M6510_P3_PIN,
  M6510_P4_PIN,
  M6510_P5_PIN,

} M6510_PINOUT;


/*
 *
 *  6510 PINOUT
 *
 */

static const uint64_t M6510_RDY  = PINMASK(M6510_RDY_PIN);
static const uint64_t M6510_IRQ  = PINMASK(M6510_IRQ_PIN);
static const uint64_t M6510_NMI  = PINMASK(M6510_NMI_PIN);
static const uint64_t M6510_SYNC = PINMASK(M6510_SYNC_PIN);
static const uint64_t M6510_RES  = PINMASK(M6510_RES_PIN);
static const uint64_t M6510_RW   = PINMASK(M6510_RW_PIN);

static const uint64_t M6510_AEC  = PINMASK(M6510_AEC_PIN);
static const uint64_t M6510_P0   = PINMASK(M6510_P0_PIN);
static const uint64_t M6510_P1   = PINMASK(M6510_P1_PIN);
static const uint64_t M6510_P2   = PINMASK(M6510_P2_PIN);
static const uint64_t M6510_P3   = PINMASK(M6510_P3_PIN);
static const uint64_t M6510_P4   = PINMASK(M6510_P4_PIN);
static const uint64_t M6510_P5   = PINMASK(M6510_P5_PIN);

// This are the floating or unused bits (6 and 7).
static const uint8_t FLOATING_IO_PINS_MASK = 0xC0;
// The bits of the port pins that are being used.
static const uint8_t USED_IO_PINS_MASK     = 0x3F;

typedef uint8_t (*m6510_input_t)(void* data);
typedef void (*m6510_output_t)(uint8_t byte, void *data);

// Output Register Bits:
// 
// 1 = OUTPUT
// 0 = INPUT 
// 
// BIT 0 - LORAM & OUTPUT (Control for RAM/ROM at 0xA000-0xBFFF -> BASIC)
// BIT 1 - HIRAM & OUTPUT (Control for RAM/ROM at 0xE000-0xFFFF -> KERNAL)
// BIT 2 - CHAREN & OUTPUT (Control for I/O ROM at 0xD000-0xDFFF)
// BIT 3 - OUTPUT (Cassette write line)
// BIT 4 - INPUT (Cassette switch sense)
// BIT 5 - OUTPUT Cassette motor control 
typedef struct m6510_port_t {

  // For thhe Output Register (0x0001)
  // The in_or -> temporary storage for the last byte for the raw byte fetched from the motherboard
  // The out_or -> temporarily stores the last byte the CPU tried to send out
  uint8_t in_or, out_or;
  // For the Data-Direction Register (0x0000)
  uint8_t io_ddr;

  // Function that reads data from external devices
  m6510_input_t in_extern_device;
  // Function that sends out data to external devices
  m6510_output_t out_extern_device;

  // Which CPU lines (bits) are currently active as output.
  uint8_t active_lines_out; 

  // This the final state of the port pins of the CPU.
  uint8_t final_lines_state;

  // This helps us to fetch the current state and the data of the emulator we are emulating.
  // This is helpful because if we were to include the c64_t into the *_extern_device() and m6510_check_io_requests functions,
  // this could cause circular-dependency hell.
  void *user_data;

} m6510_port_t;


/*
 *
 * 6510 helper functions that help controlling the pin-out and abus/dbus.
 *
 */

void m6510_set_abus(m65xx_t* m, uint16_t addr);
void m6510_set_dbus(m65xx_t* m, uint8_t data);
void m6510_set_abus_dbus(m65xx_t* m, uint16_t addr, uint8_t data);

uint16_t m6510_get_abus(const m65xx_t* m);
uint8_t m6510_get_dbus(const m65xx_t* m);

void m6510_pin_on(m65xx_t *m, uint64_t bit);
void m6510_pin_off(m65xx_t *m, uint64_t bit);

void m6510_set_port(m65xx_t *m);
void m6510_check_io_requests(m65xx_t* const m);
