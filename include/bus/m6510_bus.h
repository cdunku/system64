#pragma once 

#include "m6510.h"
#include "bus_template.h"

typedef enum M6510_PINOUT {

  M6510_ABUS_SHIFT = 0,
  M6510_DBUS_SHIFT = 16,

  M6510_ABUS_MASK  = 0xFFFFULL << M6510_ABUS_SHIFT,
  M6510_DBUS_MASK  = 0xFFULL   << M6510_DBUS_SHIFT,

  // control pins start AFTER DBUS
  M6510_RDY_PIN = 24,
  M6510_IRQ_PIN,
  M6510_NMI_PIN,
  M6510_SYNC_PIN,
  M6510_RES_PIN,
  M6510_RW_PIN,

  M6510_AEC_PIN,
  M6510_P0_PIN,
  M6510_P1_PIN,
  M6510_P2_PIN,
  M6510_P3_PIN,
  M6510_P4_PIN,
  M6510_P5_PIN,

} M6510_PINOUT;


#define PINMASK(pin) (1ULL << (pin))

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

static const uint64_t M651_AEC  = PINMASK(M6510_AEC_PIN);
static const uint64_t M6510_P0   = PINMASK(M6510_P0_PIN);
static const uint64_t M6510_P1   = PINMASK(M6510_P1_PIN);
static const uint64_t M6510_P2   = PINMASK(M6510_P2_PIN);
static const uint64_t M6510_P3   = PINMASK(M6510_P3_PIN);
static const uint64_t M6510_P4   = PINMASK(M6510_P4_PIN);
static const uint64_t M6510_P5   = PINMASK(M6510_P5_PIN);


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
