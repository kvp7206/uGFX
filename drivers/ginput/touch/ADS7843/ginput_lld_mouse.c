/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/ginput/touch/ADS7843/ginput_lld_mouse.c
 * @brief   GINPUT Touch low level driver source for the ADS7843.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_MOUSE) /*|| defined(__DOXYGEN__)*/

#include "ginput/lld/mouse.h"

#include "ginput_lld_mouse_board.h"

#if defined(GINPUT_MOUSE_YX_INVERTED) && GINPUT_MOUSE_YX_INVERTED
	#define CMD_X 0x91
	#define CMD_Y 0xD1
#else
	#define CMD_X 0xD1
	#define CMD_Y 0x91
#endif


static uint16_t sampleBuf[7];
static coord_t	lastx, lasty;

/**
 * @brief   7-point median filtering code for touch samples
 *
 * @note    This is an internally used routine only.
 *
 * @notapi
 */
static void filter(void) {
	uint16_t temp;
	int i,j;

	for(i = 0; i < 4; i++) {
		for(j = i; j < 7; j++) {
			if(sampleBuf[i] > sampleBuf[j]) {
				/* Swap the values */
				temp = sampleBuf[i];
				sampleBuf[i] = sampleBuf[j];
				sampleBuf[j] = temp;
			}
		}
	}
}

/**
 * @brief   Initialise the mouse/touch.
 *
 * @notapi
 */
void ginput_lld_mouse_init(void) {
	init_board();
}

/**
 * @brief   Read the mouse/touch position.
 *
 * @param[in] pt	A pointer to the structure to fill
 *
 * @note			For drivers that don't support returning a position
 *					when the touch is up (most touch devices), it should
 *					return the previous position with the new Z value.
 *					The z value is the pressure for those touch devices
 *					that support it (-100 to 100 where > 0 is touched)
 *					or, 0 or 100 for those drivers that don't.
 *
 * @notapi
 */
void ginput_lld_mouse_get_reading(MouseReading *pt) {
	uint16_t i;

	// If touch-off return the previous results
	if (!getpin_pressed()) {
		pt->x = lastx;
		pt->y = lasty;
		pt->z = 0;
		pt->buttons = 0;
		return;
	}
	
	// Read the port to get the touch settings
	aquire_bus();

	/* Get the X value
	 * Discard the first conversion - very noisy and keep the ADC on hereafter
	 * till we are done with the sampling. Note that PENIRQ is disabled while reading.
	 * Finally switch on PENIRQ once again - perform a dummy read.
	 * Once we have the readings, find the medium using our filter function
 	 */
	read_value(CMD_X);
	for(i = 0; i < 7; i++)
		sampleBuf[i] = read_value(CMD_X);
	read_value(CMD_X-1);
	filter();
	lastx = (coord_t)sampleBuf[3];

	/* Get the Y value using the same process as above */
	read_value(CMD_Y);
	for(i = 0; i < 7; i++)
		sampleBuf[i] = read_value(CMD_Y);
	read_value(CMD_Y-1);
	filter();
	lasty = (coord_t)sampleBuf[3];

	// Release the bus
	release_bus();
	
	// Return the results
	pt->x = lastx;
	pt->y = lasty;
	pt->z = 100;
	pt->buttons = GINPUT_TOUCH_PRESSED;
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
/** @} */
