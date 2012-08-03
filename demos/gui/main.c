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

#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

// GLCD driver object
static GLCDDriver GLCDD1;

int main(void) {
	uint8_t setActive, setState, clearActive, clearState;

	halInit();
	chSysInit();

	// Initializes the LCD
    lcdInit(&GLCDD1);

	// Initializes the touchpad
	tpInit(&SPID1);

	// clear the entire LCD screen
    lcdClear(Black);

	// Initializes the GUI thread
	// 10ms interval
	// HIGHPRIO thread priority level
	guiInit(10, HIGHPRIO);

	// set the following buttons to active
	// buttons wouldn't have any effect if you set these variables to 'inactive'
	setActive = active;
	clearActive = active;
	
	// draw a button to set, and one to clear the LED
	guiDrawButton(10, 10, 60, 60, "Set", font_Larger, Black, Yellow, 6, "SetButton", &setActive, &setState);
	guiDrawButton(70, 10, 120, 60, "Clear", font_Larger, Black, Red, 6, "ClearButton", &clearActive, &clearState);

	// you can delete a GUI element at any time from the GUI. You have to pass the GUI element name here.
	// please note that you have to redraw the screen to delete the element yourself.
	// guiDeleteElement("SetButton");
	// guiDeleteElement("ClearButton");
	
	while (TRUE) {

		// check if button 'set' is pressed
		if(setState)
			palSetPad(GPIOD, GPIOD_LED3);

		// check if button 'clear' is pressed
		if(clearState)
			palClearPad(GPIOD, GPIOD_LED3);
	
		chThdSleepMilliseconds(200);	
	}

	return 0;
}

