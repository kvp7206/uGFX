/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "gfx.h"

int main(void) {
	uint8_t charmap[8];
	uint8_t n;

	halInit();
	chSysInit();

	tdispInit();

	/* reset cursor position and clear the screen */
 	tdispHome();
	tdispClear();

	/* draw a string to a given location */
	tdispSetCursor(0, 0);
	tdispDrawChar('C');
	chThdSleepMilliseconds(500);
	tdispDrawChar('h');
	chThdSleepMilliseconds(500);
	tdispDrawChar('i');
	chThdSleepMilliseconds(500);
	tdispDrawChar('b');
	chThdSleepMilliseconds(500);
	tdispDrawChar('i');
	chThdSleepMilliseconds(500);
	tdispDrawChar('O');
	chThdSleepMilliseconds(500);
	tdispDrawChar('S');
	chThdSleepMilliseconds(500);
	tdispDrawChar('/');
	chThdSleepMilliseconds(500);
	tdispDrawChar('G');
	chThdSleepMilliseconds(500);
	tdispDrawChar('F');
	chThdSleepMilliseconds(500);
	tdispDrawChar('X');
	chThdSleepMilliseconds(500);

	// After 2 seconds blank the screen and
	// print tekst on the next line en 
	// enable the display again
	chThdSleepMilliseconds(1000);
	tdispDisplayMode(displayOff);
	tdispSetCursor(1, 1);
	tdispDrawString("TDISP/HD44780");
	chThdSleepMilliseconds(1000);
	tdispDisplayMode(displayOn);


	/* create and display a custom made character after 1S*/
	chThdSleepMilliseconds(1000);
	charmap[0] = 0b00100;
	charmap[1] = 0b01110;
	charmap[2] = 0b01110;
	charmap[3] = 0b01110;
	charmap[4] = 0b11111;
	charmap[5] = 0b00000;
	charmap[6] = 0b00100;
	charmap[7] = 0b00000;
	tdispCreateChar(0, charmap);
	tdispSetCursor(0, 1);
	tdispDrawChar(0);
	

	// after 1 second turn to 50%
	chThdSleepMilliseconds(1000);
	tdispSetBacklight(50);

	while(TRUE) {
	  tdispSetCursor(0, 3);
	  tdispDrawString("Cursor   OFF: ");
	  tdispSetCursorShape(cursorOff);
	  chThdSleepMilliseconds(5000);

	  tdispSetCursor(0, 3);
	  tdispDrawString("Cursor    ON: ");
	  tdispSetCursorShape(cursorBlock);
	  chThdSleepMilliseconds(5000);

	  tdispSetCursor(0, 3);
	  tdispDrawString("Cursor BLINK: ");
	  tdispSetCursorShape(cursorBlinkingBlock);
	  chThdSleepMilliseconds(5000);
	}
}
