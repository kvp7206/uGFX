/*
    ChibiOS/GFX - Copyright (C) 2012, 2013, 2013
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
 * This demo demonstrates the use of the GAUDIN module to read audio channel 0.
 * The audio channel gets read to display a very simple oscilloscope.
 *
 * It also demonstrates how to write your own custom GWIN window type.
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

/* Include our custom gwin audio oscilloscope */
#include "gwinosc.h"

/* Specify our timing parameters */
#define	MY_AUDIO_FREQUENCY	4000			/* 4khz */
#define MY_AUDIO_CHANNEL	0				/* Use channel 0 */

/* Data */
static GScopeObject			gScopeWindow;

/*
 * Application entry point.
 */
int main(void) {
	GHandle					ghScope;
	coord_t					swidth, sheight;

	halInit();
	chSysInit();
	gdispInit();
	gdispClear(Black);

	/* Get the screen dimensions */
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	/* Set up the scope window to fill the screen */
	ghScope = gwinCreateScope(&gScopeWindow, 0, 0, swidth, sheight, MY_AUDIO_CHANNEL, MY_AUDIO_FREQUENCY);
	gwinSetBgColor(ghScope, White);
	gwinSetColor(ghScope, Red);
	gwinClear(ghScope);

	/* Just keep displaying the scope traces */
	while (TRUE) {
		gwinWaitForScopeTrace(ghScope);
	}
}
