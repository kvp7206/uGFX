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

#include "gfx.h"

#define USE_IMAGE_CACHE			FALSE						// Only if you want to get performance at the expense of RAM
#define MY_BG_COLOR				RGB2COLOR(220, 220, 255)	// Pale blue so we can see the transparent parts

#ifdef WIN32
	#define USE_MEMORY_FILE		FALSE				// Can be true or false for Win32
#else
	#define USE_MEMORY_FILE		TRUE				// Non-Win32 - use the compiled in image
#endif

#define SHOW_ERROR(color)		gdispFillArea(swidth-10, 0, 10, sheight, color)

#if USE_MEMORY_FILE
	#include "testanim.h"
#endif

static gdispImage myImage;

/**
 * This demo display the animated gif (either directly from a file or from a
 * file encoded in flash.
 * To show the various stages of encoding it displays a colored bar down the
 * right hand side of the display to indicate what is happenning.
 * Green	- Image has completed and is displayed correctly
 * Red		- A really bad image fault. We couldn't open the image
 * Yellow	- Waiting to decode the next animation frame
 * Orange	- Decoding a frame has produced an error.
 */
int main(void) {
	coord_t			swidth, sheight;
	delaytime_t		delay;

	gfxInit();		// Initialize the display

	gdispClear(MY_BG_COLOR);

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Set up IO for our image
#if USE_MEMORY_FILE
	gdispImageSetMemoryReader(&myImage, testanim);
#else
	gdispImageSetFileReader(&myImage, "testanim.gif");
#endif

	if (gdispImageOpen(&myImage) == GDISP_IMAGE_ERR_OK) {
		gdispImageSetBgColor(&myImage, MY_BG_COLOR);
		while(1) {
			#if USE_IMAGE_CACHE
				gdispImageCache(&myImage);
			#endif
			if (gdispImageDraw(&myImage, 5, 5, myImage.width, myImage.height, 0, 0) != GDISP_IMAGE_ERR_OK) {
				SHOW_ERROR(Orange);
				break;
			}
			delay = gdispImageNext(&myImage);
			if (delay == TIME_INFINITE) {
				SHOW_ERROR(Green);
				break;
			}
			SHOW_ERROR(Yellow);
			if (delay != TIME_IMMEDIATE)
				gfxSleepMilliseconds(delay);
		}
		gdispImageClose(&myImage);
	} else
		SHOW_ERROR(Red);

	while(1) {
		gfxSleepMilliseconds(1000);
	}

	return 0;
}
