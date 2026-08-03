#pragma once


typedef enum {

  CIA_RS_SHIFT = 4,
  CIA_DBUS_SHIFT = 16,

  CIA_RS_MASK    =  0x0FULL << CIA_RS_SHIFT, 
  CIA_DBUS_MASK  =  0xFFULL << CIA_DBUS_SHIFT,

  CIA_CS,
  CIA_RW,

  CIA_FLAG,
  CIA_IRQ,
  CIA_RES,
  
  CIA_SP,
  CIA_CNT,
  CIA_TOD,
  CIA_PC,

} CIA_PINOUT;
