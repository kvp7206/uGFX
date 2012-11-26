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
 *	Make sure you have the following stuff enabled in your halconf.h:
 *
 *				#define GFX_USE_GDISP		TRUE
 *				#define GFX_USE_GWIN		TRUE
 *				#define GDISP_NEED_SCROLL	TRUE	(optional but recommended)
 *				#define GDISP_NEED_CLIP		TRUE	(optional but recommended)
 *				#define GWIN_NEED_CONSOLE 	TRUE
 */

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "gdisp.h"
#include "gwin.h"

/* The handles for our three consoles */
GHandle GW1, GW2, GW3;

/* The streams for our three consoles */
BaseSequentialStream *S1, *S2, *S3;

int main(void) {
	uint8_t i;

	halInit();
	chSysInit();

	/* initialize and clear the display */
	gdispInit();
	gdispClear(Black);

	/* create the three console windows and set a font for each */
	GW1 = gwinCreateConsole(NULL, 0, 0, gdispGetWidth(), gdispGetHeight()/2, &fontUI2Double);
	GW2 = gwinCreateConsole(NULL, 0, gdispGetHeight()/2, gdispGetWidth()/2, gdispGetHeight(), &fontSmall);
	GW3 = gwinCreateConsole(NULL, gdispGetWidth()/2, gdispGetHeight()/2, gdispGetWidth(), gdispGetHeight(), &fontSmall);

	/* Set the fore- and background colors for each console */
	gwinSetColor(GW1, Green);
	gwinSetBgColor(GW1, Black);
	gwinSetColor(GW2, White);
	gwinSetBgColor(GW2, Blue);
	gwinSetColor(GW3, Black);
	gwinSetBgColor(GW3, Red);

	/* clear all console windows - to set background */
	gwinClear(GW1);
	gwinClear(GW2);
	gwinClear(GW3);

	/* receive the stream pointers of each console */
	S1 = gwinGetConsoleStream(GW1);
	S2 = gwinGetConsoleStream(GW2);
	S3 = gwinGetConsoleStream(GW3);

	/* Output some data on the first console */
	for(i = 0; i < 10; i++) {
		chprintf(S1, "Hello ChibiOS/GFX!\r\n");
	}

	/* Output some data on the second console */
	for(i = 0; i < 16; i++) {
		chprintf(S2, "Message Nr.: %d\r\n", i+1);
	}

	/* Output some data on the third console */
	for(i = 0; i < 18; i++) {
		chprintf(S3, "Message Nr.: %d\r\n", i+1);
	}

	while(TRUE) {
		chThdSleepMilliseconds(500);
	}
}

