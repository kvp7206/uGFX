/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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

/**
 * @file    drivers/ginput/dial/GADC/ginput_lld_dial.c
 * @brief   GINPUT Dial low level driver source for GADC hardware.
 *
 * @defgroup Dial Dial
 * @ingroup GINPUT
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL

#include "ginput/lld/dial.h"

#if GINPUT_DIAL_NUM_PORTS >= 5
	#error "GINPUT: Dial - GADC driver currently only supports 4 devices"
#endif

static adcsample_t samplebuf[GINPUT_DIAL_NUM_PORTS];

#if GINPUT_DIAL_CYCLE_POLL
	static uint16_t		cycle;
#endif

static void GADCCallback(adcsample_t *buffer, void *param) {
	((DialCallbackFn)param)(buffer-samplebuf, buffer[0]);
}

void ginput_lld_dial_init(void) {
	#if GINPUT_DIAL_CYCLE_POLL
		cycle = 0;
	#endif
}

void ginput_lld_dial_poll(DialCallbackFn fn) {
	#if GINPUT_DIAL_CYCLE_POLL
		switch(cycle++) {
		#if GINPUT_DIAL_NUM_PORTS >= 1
			case 0:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE0, samplebuf+0, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 2
			case 1:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE1, samplebuf+1, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 3
			case 2:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE2, samplebuf+2, GADCCallback, (void *)fn);
				break;
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 4
			case 3:
				gadcLowSpeedStart(GINPUT_DIAL_DEVICE3, samplebuf+3, GADCCallback, (void *)fn);
				break;
		#endif
		}
		if (cycle >= GINPUT_DIAL_NUM_PORTS)
			cycle = 0;
	#else
		#if GINPUT_DIAL_NUM_PORTS >= 1
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE0, samplebuf+0, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 2
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE1, samplebuf+1, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 3
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE2, samplebuf+2, GADCCallback, (void *)fn);
		#endif
		#if GINPUT_DIAL_NUM_PORTS >= 4
			gadcLowSpeedStart(GINPUT_DIAL_DEVICE3, samplebuf+3, GADCCallback, (void *)fn);
		#endif
	#endif
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_DIAL */
/** @} */
