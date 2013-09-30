/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/toggle/Pal/ginput_lld_toggle_board_olimexsam7ex256.h
 * @brief   GINPUT Toggle low level driver source for the ChibiOS PAL hardware on the Olimex SAM7EX256 board.
 *
 * @defgroup Toggle Toggle
 * @ingroup GINPUT
 * @{
 */

#ifndef _GDISP_LLD_TOGGLE_BOARD_H
#define _GDISP_LLD_TOGGLE_BOARD_H

#include "hal.h"

#define GINPUT_TOGGLE_NUM_PORTS			4			// The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES	1			// The total number of GToggleConfig entries

#define GINPUT_TOGGLE_LEFT		GPIOD_TOUCH_LEFT    // Touch Left
#define GINPUT_TOGGLE_DOWN	    GPIOD_TOUCH_DOWN    // Touch Down
#define GINPUT_TOGGLE_UP		GPIOD_TOUCH_UP		// Touch Up
#define GINPUT_TOGGLE_RIGHT	    GPIOD_TOUCH_RIGHT	// Touch Right


#define GINPUT_TOGGLE_DECLARE_STRUCTURE()											\
	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = {	\
		{GPIOD,								/* Switch 1 and Switch 2 */	\
			GPIOD_TOUCH_LEFT|GPIOD_TOUCH_DOWN,							\
			GPIOD_TOUCH_UP|GPIOD_TOUCH_RIGHT,							\
			PAL_MODE_INPUT},											\
	}

#endif /* _GDISP_LLD_TOGGLE_BOARD_H */
/** @} */
