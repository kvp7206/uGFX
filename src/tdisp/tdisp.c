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

static uint8_t _displaycontrol;

bool_t tdispInit(void) {
	bool_t ret;

	ret = tdisp_lld_init();

	return ret;
}

void tdispControl(uint16_t what, void *value) {
	tdisp_lld_control(what, value);
}

void tdispClear(void) {
	tdisp_lld_clear();
}

void tdispHome(void) {
	tdisp_lld_home();
}

void tdispCreateChar(uint8_t address, char *charmap) {
	tdisp_lld_create_char(address, charmap);
}

void tdispSetCursor(coord_t col, coord_t row) {
	tdisp_lld_set_cursor(col, row);
}

void tdispDrawChar(char c) {
	tdisp_lld_write_data(c);
}

void tdispDrawString(char *s) {
	while(*s)
		tdispDrawChar(*s++);
}

void tdispDrawCharLocation(coord_t col, coord_t row, char c) {
	tdispSetCursor(col, row);
	tdispDrawChar(c);
}

void tdispDrawStringLocation(coord_t col, coord_t row, char *s) {
	tdispSetCursor(col, row);
	tdispDrawString(s);
}

#endif /* GFX_USE_TDISP */
/** @} */

