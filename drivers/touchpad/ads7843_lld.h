#ifndef ADS7843_LLD_H
#define ADS7843_LLD_H

#include "glcd.h"
#include "touchpad.h"

#ifdef TOUCHPAD_USE_ADS7843

uint16_t lld_tpReadX(void);
uint16_t lld_tpReadY(void);
uint16_t lld_tpReadZ(void);

#endif
#endif

