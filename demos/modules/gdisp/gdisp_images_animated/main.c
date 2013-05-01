/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

#include "ch.h"
#include "hal.h"
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
	systime_t		delay;

	halInit();			// Initialize the Hardware
	chSysInit();		// Initialize the OS
	gdispInit();		// Initialize the display

	gdispClear(MY_BG_COLOR);

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Set up IO for our image
#if USE_MEMORY_FILE
	gdispImageSetMemoryReader(&myImage, testanim);
#else
	gdispImageSetSimulFileReader(&myImage, "testanim.gif");
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
				chThdSleepMilliseconds(delay);
		}
		gdispImageClose(&myImage);
	} else
		SHOW_ERROR(Red);

	while(1) {
		chThdSleepMilliseconds(1000);
	}

	return 0;
}
