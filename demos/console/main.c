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

/*
 * 	!!! IMPORTANT !!!
 *
 *	This example dosen't compile yet, there's a problem in the console source.
 *  This issue will be fixed soon.
 */

/*
 * Please add the following to your halconf.h
 *
 * #define GFX_USE_CONSOLE TRUE
 * #define GDISP_NEED_SCROLL TRUE
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "chprintf.h"
#include "console.h"

static GConsole CON1;

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispClear(Lime);

	gfxConsoleInit(&CON1, 0, 0, gdispGetWidth(), gdispGetHeight(), &fontLarger, Black, White);
	chprintf((BaseSequentialStream *)&CON1, "Hello the time is %d\nGoodbye.", chTimeNow());


	while (TRUE) {
		
		chThdSleepMilliseconds(100);
	}
}

