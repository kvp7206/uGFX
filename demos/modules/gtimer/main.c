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

GTimer GT1, GT2;
 
void callback1(void* arg) {
    (void)arg;

    palTogglePad(GPIOD, GPIOD_LED3);
}
 
void callback2(void* arg) {
    (void)arg;

    palSetPad(GPIOD, GPIOD_LED4);
}
 
int main(void) {
    halInit();
    chSysInit();
 
    /* initialize the timers */
    gtimerInit(&GT1);
    gtimerInit(&GT2);
 
    /* continious mode - callback1() called without any argument every 250ms */
    gtimerStart(&GT1, callback1, NULL, TRUE, 250);
 
    /* single shot mode - callback2() called without any argument once after 1s */
    gtimerStart(&GT2, callback2, NULL, FALSE, 1000);

	while(TRUE) {
		chThdSleepMilliseconds(500);
	}

	return 0;
}

