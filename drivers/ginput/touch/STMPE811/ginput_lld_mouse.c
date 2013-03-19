/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
 * @file    drivers/ginput/touch/STMPE811/ginput_lld_mouse.c
 * @brief   GINPUT Touch low level driver source for the STMPE811.
 *
 * @defgroup Mouse Mouse
 * @ingroup GINPUT
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#include "stmpe811.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_MOUSE) /*|| defined(__DOXYGEN__)*/

#include "ginput/lld/mouse.h"

	#include "ginput_lld_mouse_board_embest_dmstf4bb.h"

static coord_t lastx, lasty, lastz;

/* set the active window of the stmpe811. bl is bottom left, tr is top right */
static void setActiveWindow(uint16_t bl_x, uint16_t bl_y, uint16_t tr_x, uint16_t tr_y) {
	write_reg(STMPE811_REG_WDW_TR_X, 2, tr_x);
	write_reg(STMPE811_REG_WDW_TR_Y, 2, tr_y);
	write_reg(STMPE811_REG_WDW_BL_X, 2, bl_x);
	write_reg(STMPE811_REG_WDW_BL_Y, 2, bl_y);
}

/**
 * @brief   Initialise the mouse/touch.
 *
 * @notapi
 */
void ginput_lld_mouse_init(void) {
	init_board();

	write_reg(STMPE811_REG_SYS_CTRL1, 		1, 0x02);	// software chip reset
	write_reg(STMPE811_REG_SYS_CTRL2, 		1, 0x04);	// temp. sensor clock on, GPIO clock off, touch clock on, ADC clock on
	write_reg(STMPE811_REG_INT_EN, 			1, 0x03);  //0x03
	write_reg(STMPE811_REG_ADC_CTRL1, 		1, 0x49);  //ADC conversion time = 80 clock ticks, 12-bit ADC, internacl voltage refernce

	chThdSleepMicroseconds(1000);

	write_reg(STMPE811_REG_ADC_CTRL2,  		1, 0x01);  //ADC speed 3.25MHz
	write_reg(STMPE811_REG_GPIO_AF,   		1, 0x00);  //GPIO alternate function - OFF
	write_reg(STMPE811_REG_TSC_CFG,   		1, 0xA3);  //avaraging 4, Touch detect delay 1ms, Panel driver settling time 1ms
	write_reg(STMPE811_REG_FIFO_TH,   		1, 0x01);  //FIFO trashold =1
	write_reg(STMPE811_REG_FIFO_STA,  		1, 0x01);  //FIFO reset enable
	write_reg(STMPE811_REG_FIFO_STA, 		1, 0x00);  //FIFO reset disable
	write_reg(STMPE811_REG_TSC_FRACT_XYZ,	1, 0x07); //Z axis data format
	write_reg(STMPE811_REG_TSC_I_DRIVE,    	1, 0x01); //50mA touchscreen line current 
	write_reg(STMPE811_REG_TSC_CTRL,  		1, 0x03);  //X&Y only, TSC enable
	write_reg(STMPE811_REG_INT_STA,   		1, 0xFF);  //clear all interrupts
	write_reg(STMPE811_REG_INT_CTRL,  		1, 0x01);  //level interrupt, enable intrrupts 
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
	uint16_t buf;

	// if not touched, return the previous results
	if(!getpin_pressed()) {
		pt->x = lastx;
		pt->y = lasty;
		pt->z = 0;
		pt->buttons = 0;
		return;
	}

	/* Get the X value */
	buf = read_reg(STMPE811_REG_TSC_DATA_X, 2);
	lastx = (coord_t)(buf);

	/* Get the Y value */
	buf = read_reg(STMPE811_REG_TSC_DATA_Y, 2);
	lasty = (coord_t)(buf);

	/* Get the Z value */
	buf = read_reg(STMPE811_REG_TSC_DATA_Z, 1);
	lastz = (buf & 0x00FF);

	// Return the results
	pt->x = lastx;
	pt->y = lasty;
	pt->z = 100;
	pt->buttons = GINPUT_TOUCH_PRESSED;
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
/** @} */

