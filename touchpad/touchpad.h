/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TOUCHPAD_H
#define TOUCHPAD_H

#include "ch.h"
#include "hal.h"
#include "glcd.h"
#include "glcdconf.h"
#include "ads7843_lld.h"
#include "xpt2046_lld.h"

#define CONVERSIONS 3

#define TP_CS_HIGH		palSetPad(TP_CS_PORT, TP_CS)
#define TP_CS_LOW		palClearPad(TP_CS_PORT, TP_CS)

struct cal {
	float xm;
	float ym;
	float xn;
	float yn;
};

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Description: initializes touchpad (SPI)
 *
 * param: SPI driver
 *
 * return: none
 */
void tpInit(SPIDriver *spip);

/*
 * Description: reads out PEN_IRQ from touchpad controller
 *
 * param: none
 *
 * return: 1 = touchpad pressed / 0 = touchpad not pressed
 */
uint8_t tpIRQ(void);

/*
 * Description: reads-out X coordinate, calibrated
 *
 * param: none
 *
 * return: X coordinate, relative to screen zero-point
 */
uint16_t tpReadX(void);

/*
 * Description: reads-out Y coordinate, calibrated
 *
 * param: none
 *
 * return: Y coordinate, relative to screen zero-point
 */
uint16_t tpReadY(void);

/*
 * Description: reads-out Z value / pressure
 *				only available when controller supports, returns
 *				zero otherwise.
 *
 * param:		none
 *
 * return:		pressure on the touchpad
 */
uint16_t tpReadZ(void);

/*
 * Description: calibration routine
 *
 * param: none
 *
 * return: none
 */
void tpCalibrate(void);

#ifdef __cplusplus
}
#endif

#endif

