/* ChibiOS/GFX - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file	touchpad.c
 * @brief	Touchpad Driver code.
 *
 * @addtogroup TOUCHPAD
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchpad.h"

#if GFX_USE_TOUCHPAD || defined(__DOXYGEN__)

#if TOUCHPAD_STORE_CALIBRATION
extern void tp_store_calibration_lld(struct cal_t *cal);
extern struct cal_t *tp_restore_calibration_lld(void);
#endif

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
static struct cal_t *cal;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief	returns the uncalibrated readout of the X direction from the controller
 *
 * @noapi
 */
static uint16_t _tpReadRealX(void) {
    uint32_t results = 0;
    uint16_t i, x;

    /* Median filtering is already done in LLD */
    for(i = 0; i < CONVERSIONS; i++) {
        results += tp_lld_read_x();
    }

	/* Take the average of the readings */
    x = results / CONVERSIONS;

    return x;
}

/**
 * @brief	return the uncalibrated readout of the Y-direction from the controller
 *
 * @noapi
 */
static uint16_t _tpReadRealY(void) {
    uint32_t results = 0;
    uint16_t i, y;

    /* Median filtering is already done in LLD */
    for(i = 0; i < CONVERSIONS; i++) {
        results += tp_lld_read_y();
    }

    /* Take the average of the readings */
    y = results / CONVERSIONS;

    return y;
}

/**
 * @brief	draws a cross. Used for calibration.
 *
 * @noapi
 */
static void _tpDrawCross(uint16_t x, uint16_t y) {
    gdispDrawLine(x-15, y, x-2, y, 0xffff);
    gdispDrawLine(x+2, y, x+15, y, 0xffff);
    gdispDrawLine(x, y-15, x, y-2, 0xffff);
    gdispDrawLine(x, y+2, x, y+15, 0xffff);
  
    gdispDrawLine(x-15, y+15, x-7, y+15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x-15, y+7, x-15, y+15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x-15, y-15, x-7, y-15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x-15, y-7, x-15, y-15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x+7, y+15, x+15, y+15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x+15, y+7, x+15, y+15, RGB565CONVERT(184,158,131));

    gdispDrawLine(x+7, y-15, x+15, y-15, RGB565CONVERT(184,158,131));
    gdispDrawLine(x+15, y-15, x+15, y-7, RGB565CONVERT(184,158,131));    
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief	Touchpad Driver initialization.
 * @note    This function is NOT currently implicitly invoked by @p halInit().
 *          It must be called manually.
 *
 * @param[in] tp	The touchpad driver struct
 *
 * @api
 */
void tpInit(const TOUCHPADDriver *tp) {
	cal = (struct cal_t*)chHeapAlloc(NULL, sizeof(struct cal_t));
	if(cal == NULL)
		return;

	/* Initialise Mutex */
	//MUTEX_INIT

	/* Initialise driver */
	//MUTEX_ENTER
	tp_lld_init(tp);
	//MUTEX_EXIT

	#if TOUCHPAD_STORE_CALIBRATION
		cal = tp_restore_calibration_lld();
		if(cal == NULL) {
			cal = (struct cal_t*)chHeapAlloc(NULL, sizeof(struct cal_t));
			tpCalibrate();
		}
	#endif
}

/**
 * @brief	Get the X-Coordinate, relative to screen zero point.
 *
 * @return	The X position in pixels.
 *
 * @api
 */
uint16_t tpReadX(void) {
	uint16_t x, y;

#if TOUCHPAD_XY_INVERTED == TRUE
	x = cal->xm * _tpReadRealY() + cal->xn;
	y = cal->ym * _tpReadRealX() + cal->yn;
#else
	x = cal->xm * _tpReadRealX() + cal->xn;
	y = cal->ym * _tpReadRealY() + cal->yn;
#endif

	switch(gdispGetOrientation()) {  
		case GDISP_ROTATE_0:
			return x;
		case GDISP_ROTATE_90:
			return y;
		case GDISP_ROTATE_180:
			return GDISP_SCREEN_WIDTH - x - 1;
		case GDISP_ROTATE_270:
			return GDISP_SCREEN_HEIGHT - y - 1;
	}

	return 0;
}
	
/**
 * @brief	Get the X-Coordinate, relative to screen zero point.
 *
 * @return	The Y position in pixels.
 *
 * @api
 */
uint16_t tpReadY(void) {
	uint16_t x, y;

#if TOUCHPAD_XY_INVERTED == TRUE
	x = cal->xm * _tpReadRealY() + cal->xn;
	y = cal->ym * _tpReadRealX() + cal->yn;
#else
	x = cal->xm * _tpReadRealX() + cal->xn;
	y = cal->ym * _tpReadRealY() + cal->yn;
#endif

	switch(gdispGetOrientation()) { 
		case GDISP_ROTATE_0:
			return y;
		case GDISP_ROTATE_90:
			return GDISP_SCREEN_WIDTH - x - 1;
		case GDISP_ROTATE_180:
			return GDISP_SCREEN_HEIGHT - y - 1;
		case GDISP_ROTATE_270:
			return x;
	}

	return 0;
}

void tpCalibrate(void) {
	const uint16_t h = gdispGetHeight();
	const uint16_t w = gdispGetWidth();
	const uint16_t cross[2][2] = {{(w/8), (h/8)}, {(w-(w/8)) , (h-(h/8))}};
	uint16_t points[2][2];
	uint8_t i;

	gdispSetOrientation(GDISP_ROTATE_0);
	gdispClear(Red);
	gdispFillStringBox(0, 10, gdispGetWidth(), 30, "Calibration", &fontUI2Double,  White, Red, justifyCenter);

	for(i = 0; i < 2; i++) {
		_tpDrawCross(cross[i][0], cross[i][1]);
		while(!tpIRQ());
		points[i][0] = _tpReadRealX();
		points[i][1] = _tpReadRealY();
		chThdSleepMilliseconds(100);
		while(tpIRQ());
		gdispFillArea(cross[i][0]-15, cross[i][1]-15, 42, 42, Red);
	}
	
	cal->xm = ((float)cross[1][0] - (float)cross[0][0]) / ((float)points[1][0] - (float)points[0][0]);
	cal->ym = ((float)cross[1][1] - (float)cross[0][1]) / ((float)points[1][1] - (float)points[0][1]);

	cal->xn = (float)cross[0][0] - cal->xm * (float)points[0][0];
	cal->yn = (float)cross[0][1] - cal->ym * (float)points[0][1];
	
	#if TOUCHPAD_STORE_CALIBRATION
		tp_store_calibration_lld(cal);
	#endif
}

/**
 * @brief	returns if touchpad is pressed or not
 *
 * @return	1 if pressed, 0 otherwise
 *
 * @api
 */
#if TOUCHPAD_HAS_IRQ || defined(__DOXYGEN__)
	uint8_t tpIRQ(void) {
		return tp_lld_irq();
	}
#endif

/**
 * @brief	Get the pressure.
 *
 * @return	The pressure.
 *
 * @api
 */
#if TOUCHPAD_HAS_PRESSURE || defined(__DOXYGEN__)
	uint16_t tpReadZ(void) {
		/* ToDo */
		return (tp_lld_read_z());
	}
#endif

#endif /* GFX_USE_TOUCHPAD */
/** @} */

