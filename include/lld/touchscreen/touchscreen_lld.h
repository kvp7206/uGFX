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
 * @file	include/lld/touchscreen/touchscreen_lld.h
 * @brief	TOUCHSCREEN Driver subsystem low level driver header.
 *
 * @addtogroup TOUCHSCREEN
 * @{
 */

#ifndef TOUCHSCREEN_LLD_H
#define TOUCHSCREEN_LLD_H

#if GFX_USE_TOUCHSCREEN || defined(__DOXYGEN__)

/*===========================================================================*/
/* Include the low level driver configuration information                    */
/*===========================================================================*/

#include "touchscreen_lld_config.h"

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

#ifndef TOUCHSCREEN_NEED_MULTITHREAD
	#define TOUCHSCREEN_NEED_MULTITHREAD	FALSE
#endif

#ifndef TOUCHSCREEN_XY_INVERTED
	#define TOUCHSCREEN_XY_INVERTED	FALSE
#endif

#ifndef TOUCHSCREEN_HAS_PRESSED
	#define TOUCHSCREEN_HAS_PRESSED	FALSE
#endif

#ifndef TOUCHSCREEN_HAS_PRESSURE
	#define TOUCHSCREEN_HAS_PRESSURE	FALSE
#endif

#ifndef TOUCHSCREEN_SPI_PROLOGUE
    #define TOUCHSCREEN_SPI_PROLOGUE()
#endif

#ifndef TOUCHSCREEN_SPI_EPILOGUE
    #define TOUCHSCREEN_SPI_EPILOGUE()
#endif

#ifndef TOUCHSCREEN_STORE_CALIBRATION
	#define TOUCHSCREEN_STORE_CALIBRATION FALSE
#endif

#ifndef TOUCHSCREEN_VERIFY_CALIBRATION
	#define TOUCHSCREEN_VERIFY_CALIBRATION FALSE
#endif

#ifndef TOUCHSCREEN_CONVERSIONS
	#define	TOUCHSCREEN_CONVERSIONS 3
#endif

/*===========================================================================*/
/* Driver types.                                                             */
/*===========================================================================*/

/**
 * @brief	Structure representing a touchscreen driver. Hardware dependant.
 */
typedef struct TouchscreenDriver TouchscreenDriver;

// Forward declaration
struct cal_t;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/


#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	void ts_lld_init(const TouchscreenDriver *ts);

	uint16_t ts_lld_read_value(uint8_t cmd);
	uint16_t ts_lld_read_x(void);
	uint16_t ts_lld_read_y(void);

	#if TOUCHSCREEN_HAS_PRESSED
	uint8_t ts_lld_pressed(void);
	#endif

	#if TOUCHSCREEN_HAS_PRESSURE
	uint16_t ts_lld_read_z(void);
	#endif

	#if TOUCHSCREEN_STORE_CALIBRATION
	// These may be defined by the low level driver or by the application
	void ts_store_calibration_lld(struct cal_t *cal);
	struct cal_t *ts_restore_calibration_lld(void);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TOUCHSCREEN */

#endif /* _TOUCHSCREEN_LLD_H */
/** @} */

