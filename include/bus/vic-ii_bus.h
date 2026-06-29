#pragma once

#include "vic-ii.h"
#include "bus_template.h"

typedef enum VIC_II_PINOUT {

  VIC_II_ABUS_SHIFT = 0,
  VIC_II_MEM_DBUS_SHIFT = 12,
  VIC_II_COLOR_DBUS_SHIFT = 20,

  VIC_II_ABUS_MASK  = 0xFFFULL << VIC_II_ABUS_SHIFT,
  VIC_II_MEM_DBUS_MASK  = 0xFFULL   << VIC_II_MEM_DBUS_SHIFT,
  VIC_II_COLOR_DBUS_MASK = 0xFULL << VIC_II_COLOR_DBUS_SHIFT,

  VIC_II_BA_PIN = 24,
  VIC_II_IRQ_PIN,
  VIC_II_RW_PIN,
  VIC_II_AEC_PIN,

  VIC_II_LP_PIN,
  VIC_II_CS_PIN,

} VIC_II_PINOUT;

#define VIC_II_PINOUT_MASK ~(1ULL << 30) 

/* 
 *
 * VIC-II PINOUT
 *
 */

static const uint64_t VIC_II_IRQ  = PINMASK(VIC_II_IRQ_PIN);
static const uint64_t VIC_II_LP   =  PINMASK(VIC_II_LP_PIN);
static const uint64_t VIC_II_CS   =  PINMASK(VIC_II_CS_PIN);
static const uint64_t VIC_II_RW   =  PINMASK(VIC_II_RW_PIN);
static const uint64_t VIC_II_BA   =  PINMASK(VIC_II_BA_PIN);
static const uint64_t VIC_II_AEC  = PINMASK(VIC_II_AEC_PIN);

/*
 *
 * VIC-II helper functions that help controlling the pin-out and abus/dbus.
 *
 */

void vic_write_color_ram(vic_ii_t *vic, uint16_t addr, uint8_t val);
uint8_t vic_read_color_ram(vic_ii_t *vic, uint16_t addr);

void vic_set_abus(vic_ii_t* vic, uint16_t addr);
void vic_set_mem_dbus(vic_ii_t* vic, uint8_t data);
void vic_set_color_dbus(vic_ii_t* vic, uint8_t data);

uint16_t vic_get_abus(const vic_ii_t* vic);
uint8_t vic_get_mem_dbus(const vic_ii_t* vic);
uint8_t vic_get_color_dbus(const vic_ii_t* vic);

void vic_pin_on(vic_ii_t *vic, uint64_t bit);
void vic_pin_off(vic_ii_t *vic, uint64_t bit);

void vic_ii_bank_switching(vic_ii_t *vic);


