/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
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
#include "chprintf.h"

/* The handles for our three consoles */
GHandle GW1, GW2, GW3;

/* The streams for our three consoles */
BaseSequentialStream *S1, *S2, *S3;

int main(void) {
	uint8_t i;
	font_t	font1, font2;

	/* initialize and clear the display */
	gfxInit();
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
		gfxSleepMilliseconds(500);
	}
}

