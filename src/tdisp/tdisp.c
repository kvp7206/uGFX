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

	ret = TDISP_LLD(init)();

	return ret;
}

void tdispSetAttributes(uint8_t attributes) {
	switch(attributes) {
		case TDISP_ON:
			_displaycontrol |= 0x04;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
		case TDISP_OFF:
			_displaycontrol &=~ 0x04;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
		case TDISP_CURSOR_ON:
			_displaycontrol |= 0x02;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
		case TDISP_CURSOR_OFF:
			_displaycontrol &=~ 0x02;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
		case TDISP_CURSOR_BLINK_ON:
			_displaycontrol |= 0x00;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
		case TDISP_CURSOR_BLINK_OFF:
			_displaycontrol &=~ 0x00;
			TDISP_LLD(write_cmd)(0x08 | _displaycontrol);
			break;
	}
}

void tdispClear(void) {
	TDISP_LLD(write_cmd)(0x01);
}

void tdispHome(void) {
	TDISP_LLD(write_cmd)(0x02);
}

void tdispGotoXY(coord_t col, coord_t row) {
	uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };

	if(row >= TDISP_ROWS)
		row = TDISP_ROWS - 1;

	TDISP_LLD(write_cmd)(0x80 | (col + row_offsets[row]));
}

void tdispDrawChar(char c) {
	TDISP_LLD(write_data)(c);
}

void tdispDrawString(char *s) {
	char c;

	while(c = *s++)
		tdispDrawChar(c);	
}

void tdispDrawCharLocation(coord_t col, coord_t row, char c) {
	tdispGotoXY(col, row);
	tdispDrawChar(c);
}

void tdispDrawStringLocation(coord_t col, coord_t row, char *s) {
	tdispGotoXY(col, row);
	tdispDrawString(s);
}

#endif /* GFX_USE_TDISP */
/** @} */

