#ifndef ADS7843_LLD_H
#define ADS7843_LLD_H

#include "glcd.h"
#include "touchpad.h"

#ifdef TOUCHPAD_USE_ADS7843

uint16_t lld_readX(void);
uint16_t lld_readY(void);
uint16_t lld_readZ(void);

#endif
#endif

