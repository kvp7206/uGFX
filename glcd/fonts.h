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

#include <stdint.h>

#ifndef _FONT_
#define _FONT_

#define FONT_TABLE_HEIGHT_IDX 				0
#define FONT_TABLE_PAD_AFTER_CHAR_IDX 		1
#define FONT_TABLE_LINE_SPACING_IDX 		2
#define FONT_TABLE_DECENDERS_HEIGHT_IDX 	3
#define FONT_TABLE_UNUSED_IDX				4
#define FONT_TABLE_CHAR_LOOKUP_IDX			5

extern const uint8_t font_Small[];
extern const uint8_t font_Larger[];
//extern const uint8_t font_Medium[];
extern const uint8_t font_MediumBold[];
extern const uint8_t font_LargeNumbers[];

#endif
