/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gfx.h"

#define USE_IMAGE_CACHE			FALSE						// Only if you want to get performance at the expense of RAM
#define MY_BG_COLOR				RGB2COLOR(220, 220, 255)	// Pale blue so we can see the transparent parts

#ifdef WIN32
	#define USE_MEMORY_FILE		TRUE				// Can be true or false for Win32
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
