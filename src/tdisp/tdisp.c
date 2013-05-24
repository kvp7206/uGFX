/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
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

void tdispCreateChar(uint8_t address, char *charmap) {
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

void tdispDrawStringLocation(coord_t col, coord_t row, char *s) {
	tdispSetCursor(col, row);
	tdispDrawString(s);
}	

void tdispControl(uint16_t what, void *value) {
	MUTEX_ENTER();
	tdisp_lld_control(what, value);
	MUTEX_LEAVE();
}

#endif /* GFX_USE_TDISP */
/** @} */
