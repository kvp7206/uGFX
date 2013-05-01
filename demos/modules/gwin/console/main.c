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
#include "chprintf.h"
#include "gfx.h"

/* The handles for our three consoles */
GHandle GW1, GW2, GW3;

/* The streams for our three consoles */
BaseSequentialStream *S1, *S2, *S3;

int main(void) {
	uint8_t i;
	font_t	font1, font2;

	halInit();
	chSysInit();

	/* initialize and clear the display */
	gdispInit();
	gdispClear(Black);
	font1 = gdispOpenFont("UI2 Double");
	font2 = gdispOpenFont("Small");

	/* create the three console windows and set a font for each */
	GW1 = gwinCreateConsole(NULL, 0, 0, gdispGetWidth(), gdispGetHeight()/2, font1);
	GW2 = gwinCreateConsole(NULL, 0, gdispGetHeight()/2, gdispGetWidth()/2, gdispGetHeight(), font2);
	GW3 = gwinCreateConsole(NULL, gdispGetWidth()/2, gdispGetHeight()/2, gdispGetWidth(), gdispGetHeight(), font2);

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

