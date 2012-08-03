/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "glcd.h"

static GLCDDriver GLCDD1;

int main(void) {
	halInit();
	chSysInit();
	
	lcdInit(&GLCDD1);
	lcdClear(Black);
	lcdDrawString(100, 100, "Hello World", White, Black);

	// wait two seconds to see current LCD content
	chThdSleepSeconds(2);

	// brings LCD to sleep mode
	lcdSetPowerMode(sleepOn);

	// wait two seconds to see current LCD content
	chThdSleepSeconds(2);

	// brings LCD back from sleep mode
	// content displayed before gets displayed again
	lcdSetPowerMode(sleepOff);

	while (TRUE) {
		
		chThdSleepMilliseconds(200);	
	}

	return 0;
}

