#pragma once


typedef struct {

  uint64_t cia_pins;

  uint8_t pra;
  uint8_t prb;

  uint8_t ddra;
  uint8_t ddrb;

  uint16_t talo;
  uint16_t tahi;
  uint16_t tblo;
  uint16_t tbhi;

  uint8_t tod_10th;
  uint8_t tod_sec;
  uint8_t tod_min;
  uint8_t tod_hr;

  uint8_t sdr;
  uint8_t icr;

  uint8_t cra;
  uint8_t crb;

} cia_t;
