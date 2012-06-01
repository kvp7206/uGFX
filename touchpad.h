#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include "ch.h"
#include "hal.h"

#define SET_CS(a)		(TP_PORT->BSRR = 1 << (TP_CS + (a ? 0 : 16)))

void tpInit(void);
void tpWriteData(uint8_t data);
uint16_t tpReadData(void);
uint16_t tpReadX(void);
uint16_t tpReadY(void);

#endif
