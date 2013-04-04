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

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#ifdef WIN32
	#define USE_MEMORY_FILE		FALSE				// Can be true or false for Win32
#else
	#define USE_MEMORY_FILE		TRUE				// Non-Win32 - use the compiled in image
#endif

#if USE_MEMORY_FILE
	#include "test-pal8.h"
#endif

static gdispImage myImage;

int main(void) {
	coord_t			swidth, sheight;

	halInit();			// Initialize the Hardware
	chSysInit();		// Initialize the OS
	gdispInit();		// Initialize the display

	gdispClear(Black);

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Set up IO for our image
#if USE_MEMORY_FILE
	gdispImageSetMemoryReader(&myImage, test_pal8);
#else
	gdispImageSetSimulFileReader(&myImage, "test-pal8.bmp");
#endif

	gdispImageOpen(&myImage);
	gdispImageDraw(&myImage, 0, 0, swidth, sheight, 0, 0);
	gdispImageClose(&myImage);

	while(1) {
		chThdSleepMilliseconds(1000);
	}

	return 0;
}
