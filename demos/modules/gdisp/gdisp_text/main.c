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

int main(void) {
	coord_t		width, height;
	font_t		font1, font2, font3, font4;
	const char	*msg;

    /* Initialize and clear the display */
    gfxInit();

    // Get the screen size
    width = gdispGetWidth();
    height = gdispGetHeight();

    // Get the fonts we want to use
	font1 = gdispOpenFont("UI2");
	font2 = gdispOpenFont("UI2 Double");
	font3 = gdispOpenFont("UI2 Narrow");
	font4 = gdispOpenFont("LargeNumbers");

	// Display large numbers on the right (measuring the string)
	msg = "123456";
	gdispDrawString(width-gdispGetStringWidth(msg, font4)-3, 3, msg, font4, Green);
	
	// Display the font name under it.
	msg = gdispGetFontName(font4);
	gdispDrawString(width-gdispGetStringWidth(msg, font1)-3, 20, msg, font1, Green);

	// Demonstrate our other fonts
	gdispDrawString(10, 10, "Writing with Font 'UI2'", font1, Yellow);
	gdispFillString(10, 35, "Writing with Font 'UI2 Double'", font2, Red, White);
	gdispDrawStringBox(0, 50, width, 40, "Writing with Font 'UI2 Narrow'", font3, Red, justifyCenter);
	gdispFillStringBox(0, 90, width, 40, "Filled Centered", font3, Pink, Gray, justifyCenter);

	// Clean up the fonts
	gdispCloseFont(font1);
	gdispCloseFont(font2);
	gdispCloseFont(font3);
	gdispCloseFont(font4);

	// Wait forever
    while(TRUE) {
    	gfxSleepMilliseconds(500);
    }   
}

