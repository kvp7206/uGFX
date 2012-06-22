#ifndef XPT2046_LLD_H
#define XPT2046_LLD_H

#include "glcd.h"
#include "touchpad.h"

#ifdef TOUCHPAD_USE_XPT2046

uint16_t lld_readX(void);
uint16_t lld_readY(void);
uint16_t lld_readZ(void);

#endif
#endif

