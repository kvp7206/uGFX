/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/tdisp/tdisp.c
 * @brief   TDISP Driver code.
 *
 * @addtogroup TDISP
 * @{
 */
#include "gfx.h"

#if GFX_USE_TDISP || defined(__DOXYGEN__)

#include "tdisp/lld/tdisp_lld.h"

/* cursor controllers */
#define TDISP_CURSOR		1
#define TDISP_CURSOR_ON		0
#define TDISP_CURSOR_OFF

#if TDISP_NEED_MULTITHREAD
	static gfxMutex			tdispMutex;

	#define MUTEX_INIT()	gfxMutexInit(&tdispMutex)
	#define MUTEX_ENTER()	gfxMutexEnter(&tdispMutex)
	#define MUTEX_LEAVE()	gfxMutexExit(&tdispMutex)

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

void tdispControl(uint16_t what, uint16_t value) {
	MUTEX_ENTER();
	tdisp_lld_control(what, value);
	MUTEX_LEAVE();
}

void tdispScroll(uint16_t direction, uint16_t amount, uint16_t delay) {
	MUTEX_ENTER();
	tdisp_lld_scroll(direction, amount, delay);
	MUTEX_LEAVE();
}

#if TDISP_USE_BACKLIGHT
void tdispSetBacklight(uint16_t percentage) {
	if (percentage > 100)
	  percentage = 100;
	MUTEX_ENTER();
	tdisp_lld_set_backlight(percentage);
	MUTEX_LEAVE();
}
#endif

#endif /* GFX_USE_TDISP */
/** @} */
