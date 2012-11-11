/*
    ChibiOS/GFX - Copyright (C) 2012
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
 * @file    drivers/touchscreen/MCU/touchscreen_lld.c
 * @brief   Touchscreen Driver subsystem low level driver source.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "touchscreen.h"

#if GFX_USE_TOUCHSCREEN /*|| defined(__DOXYGEN__)*/


/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

#if !defined(__DOXYGEN__)
    /* Local copy of the current touchpad driver */
    static const TouchscreenDriver *tsDriver;

#endif

/**
 * @brief   Low level Touchscreen driver initialization.
 *
 * @param[in] ts	The touchscreen driver struct
 *
 * @notapi
 */
void ts_lld_init(const TouchscreenDriver *ts) {
	tsDriver = ts;

	/* set pins to analog input */
	palSetPadMode(ts->ts_yd_port, ts->ts_yd_pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ts->ts_yu_port, ts->ts_yu_pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ts->ts_xl_port, ts->ts_xl_pin, PAL_MODE_INPUT_ANALOG);
	palSetPadMode(ts->ts_xr_port, ts->ts_xr_pin, PAL_MODE_INPUT_ANALOG);
}


/**
 * @brief   Reads a conversion from the touchscreen
 *
 * @param[in] cmd    The command bits to send to the touchscreen
 *
 * @return  The read value 12-bit right-justified
 *
 * @note    This function only reads data, it is assumed that the pins are
 *          configured properly and the bus has been acquired beforehand
 *
 * @notapi
 */
uint16_t ts_lld_read_value(uint8_t cmd) {
	return 0;
}

/**
 * @brief   7-point median filtering code for touchscreen samples
 *
 * @note    This is an internally used routine only.
 *
 * @notapi
 */
static void ts_lld_filter(void) {
	return 0;
}

/**
 * @brief   Reads out the X direction.
 *
 * @note    The samples are median filtered for greater noise reduction
 *
 * @notapi
 */
uint16_t ts_lld_read_x(void) {
	return 0;
}

/**
 * @brief	Reads out the Y direction.
 *
 * @notapi
 */
uint16_t ts_lld_read_y(void) {
	return 0;
}

/*
 * @brief	Checks if touchscreen is pressed or not.
 *
 * @return	1 if pressed, 0 otherwise
 *
 * @notapi
 */
uint8_t ts_lld_pressed(void) {
	return 0;
}


#endif /* GFX_USE_TOUCHSCREEN */
/** @} */

