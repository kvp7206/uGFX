/*
    ChibiOS-LCD-Driver - Copyright (C) 2012
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

/**
 * @file	touchpad.c
 * @brief	Touchpad Driver code.
 *
 * @addgroup TOUCHPAD
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchpad.h"

#if HAL_USE_TOUCHPAD || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/
volatile static struct cal cal = {
    1, 1, 0, 0
};

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

    for(i = 0; i < CONVERSIONS; i++) {
        tp_lld_read_x(); /* dummy, reduce noise on SPI */
        results += tp_lld_read_x();
    }   

    x = (((SCREEN_WIDTH-1) * (results/CONVERSIONS)) / 2048);

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

    for(i = 0; i < CONVERSIONS; i++) {
        tp_lld_read_y(); /* dummy, reduce noise on SPI */
        results += tp_lld_read_y();
    }   

    y = (((SCREEN_HEIGHT-1) * (results/CONVERSIONS)) / 2048);

    return y;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief	Touchpad Driver initialization.
 * @note    This function is NOT currently implicitly invoked by @p halInit().
 *          It must be called manually.
 *
 * @init
 */
void tpInit(TOUCHPADDriver *tp) {
	/* Initialise Mutex */
	//MUTEX_INIT

	/* Initialise driver */
	//MUTEX_ENTER
	tp_lld_init(tp);
	//MUTEX_EXIT
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

	x = cal.xm * _tpReadRealX() + cal.xn;
	y = cal.ym * _tpReadRealY() + cal.yn;

	/*
	switch(gdispGetOrientation()) { // implement gdispGetOrientation()  
		case portrait:
			return x;
		case landscape:
			return SCREEN_HEIGHT - y;
		case portraitInv:
			return SCREEN_WIDTH - x;
		case landscapeInv:
			return y;
	}   
	*/

	return x;
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

	x = cal.xm * _tpReadRealX() + cal.xn;
	y = cal.ym * _tpReadRealY() + cal.yn;

	/*
	switch(gdispGetOrientation()) { // implement gdispGetOrientation()
		case portrait:
			return y;
		case landscape:
			return x;
		case portraitInv:
			return SCREEN_HEIGHT - y;
		case landscapeInv:
			return SCREEN_WIDTH - x;
	}
	*/

	return y;
}

#if TOUCHPAD_PRESSURE || defined(__DOXYGEN__)
	/**
	 * @brief	Get the pressure.
	 *
	 * @return	The pressure.
	 *
	 * @api
	 */
	uint16_t tpReadZ(void) {
		/* ToDo */
		return (tp_lld_read_z());
	}
#endif

#endif /* HAL_USE_TOUCHPAD */
/** @} */

