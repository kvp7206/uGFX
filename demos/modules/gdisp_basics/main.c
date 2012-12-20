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
	coord_t		i, j;

    halInit();
    chSysInit();

    /* Initialize and clear the display */
    gdispInit();
    gdispClear(Black);

    // Get the screen size
    width = gdispGetWidth();
    height = gdispGetHeight();

    // Code Here
	gdispDrawBox(10, 10, width/2, height/2, Yellow);
    gdispFillArea(width/2, height/2, width/2-10, height/2-10, Blue);
    gdispDrawLine(5, 30, width-50, height-40, Red);
    
	for(i = 5, j = 0; i < width && j < height; i += 7, j += i/20)
    	gdispDrawPixel (i, j, White);

    while(TRUE) {
        chThdSleepMilliseconds(500);
    }   
}

