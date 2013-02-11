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

#include "ch.h"
#include "hal.h"
#include "gfx.h"

int main(void) {
	coord_t		width, height;
	font_t		font1, font2, font3, font4;
	const char	*msg;

    halInit();
    chSysInit();

    /* Initialize and clear the display */
    gdispInit();
    gdispClear(Black);

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
        chThdSleepMilliseconds(500);
    }   
}

