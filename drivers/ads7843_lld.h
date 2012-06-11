#ifndef ADS7843_LLD_H
#define ADS7843_LLD_H

#include "glcdconf.h"
#include "touchpad.h"

#ifdef TOUCHPAD_USE_ADS7843

uint16_t lld_readX(void);
uint16_t lld_readY(void);

#endif
#endif

