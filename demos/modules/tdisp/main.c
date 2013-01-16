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
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "gfx.h"

int main(void) {
	char charmap[8];

	halInit();
	chSysInit();

	tdispInit();

	/* reset cursor position and clear the screen */
	tdispHome();
	tdispClear();

	/* set cursor position and draw single characters */
	tdispSetCursor(4, 0);
	tdispDrawChar('H');
	tdispDrawChar('D');
	tdispDrawChar('4');
	tdispDrawChar('4');
	tdispDrawChar('7');
	tdispDrawChar('8');
	tdispDrawChar('0');

	/* draw a string to a given location */
	tdispDrawStringLocation(0, 1, "chibios-gfx.com");

	/* create and display a custom made character */
	charmap[0] = 0b00000;
	charmap[1] = 0b00100;
	charmap[2] = 0b00010;
	charmap[3] = 0b11111;
	charmap[4] = 0b00010;
	charmap[5] = 0b00100;
	charmap[6] = 0b00000;
	charmap[7] = 0b00000;
	tdispCreateChar(0, charmap);
	tdispHome();
	tdispDrawChar(0);

	while(TRUE) {
		chThdSleepMilliseconds(250);	
	}
}

