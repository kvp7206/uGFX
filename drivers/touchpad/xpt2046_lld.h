#ifndef XPT2046_LLD_H
#define XPT2046_LLD_H

#include "glcd.h"
#include "touchpad.h"

#ifdef TOUCHPAD_USE_XPT2046

uint16_t lld_tpReadX(void);
uint16_t lld_tpReadY(void);
uint16_t lld_tpReadZ(void);

#endif
#endif

