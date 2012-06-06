#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include "ch.h"
#include "hal.h"

#define CONVERSIONS 3

#define SET_CS(a)		(TP_PORT->BSRR = 1 << (TP_CS + (a ? 0 : 16)))

volatile struct cal {
	float xm;
	float ym;
	float xn;
	float yn;
};

void tpInit(void);
uint8_t tpIRQ(void);
uint16_t tpReadX(void);
uint16_t tpReadY(void);
uint16_t tpReadRealX(void);
uint16_t tpReadRealY(void);
void tpCalibrate(void);
void tpCalibrate2(void);
void tpDrawCross(uint16_t x, uint16_t y);

#endif
