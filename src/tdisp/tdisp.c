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

/**
 * @file    src/tdisp/tdisp.c
 * @brief   TDISP Driver code.
 *
 * @addtogroup TDISP
 * @{
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_TDISP || defined(__DOXYGEN__)

#include "tdisp/lld/tdisp_lld.h"

#if TDISP_NEED_MULTITHREAD
	#if !CH_USE_MUTEXES
		#error "TDISP: CH_USE_MUTEXES must be defined in chconf.h because TDISP_NEED_MULTITHREAD is defined"
	#endif

	static Mutex			tdispMutex;

	#define MUTEX_INIT()	chMtxInit(&tdispMutex)
	#define MUTEX_ENTER()	chMtxLock(&tdispMutex)
	#define MUTEX_LEAVE()	chMtxUnlock()

#else

	#define MUTEX_INIT()
	#define MUTEX_ENTER()
	#define MUTEX_LEAVE()

#endif

bool_t tdispInit(void) {
	bool_t		res;

	MUTEX_INIT();

	MUTEX_ENTER();
	res = tdisp_lld_init();
	MUTEX_LEAVE();

	return res;
}

void tdispClear(void) {
	MUTEX_ENTER();
	tdisp_lld_clear();
	MUTEX_LEAVE();
}

void tdispHome(void) {
	MUTEX_ENTER();
	tdisp_lld_set_cursor(0, 0);
	MUTEX_LEAVE();
}

void tdispSetCursor(coord_t col, coord_t row) {
	/* Keep the input range valid */
	if (row >= TDISP.rows)
		row = TDISP.rows - 1;
	MUTEX_ENTER();
	tdisp_lld_set_cursor(col, row);
	MUTEX_LEAVE();
}

void tdispCreateChar(uint8_t address, uint8_t *charmap) {
	/* make sure we don't write somewhere we're not supposed to */
	if (address < TDISP.maxCustomChars) {
		MUTEX_ENTER();
		tdisp_lld_create_char(address, charmap);
		MUTEX_LEAVE();
	}
}

void tdispDrawChar(char c) {
	MUTEX_ENTER();
	tdisp_lld_draw_char(c);
	MUTEX_LEAVE();
}

void tdispDrawString(char *s) {
	MUTEX_ENTER();
	while(*s)
		tdisp_lld_draw_char(*s++);
	MUTEX_LEAVE();
}

void tdispControl(uint16_t what, void *value) {
	MUTEX_ENTER();
	tdisp_lld_control(what, value);
	MUTEX_LEAVE();
}

#endif /* GFX_USE_TDISP */
/** @} */
