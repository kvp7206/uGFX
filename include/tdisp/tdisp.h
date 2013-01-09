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

/**
 * @file    include/tdisp/tdisp.h
 * @brief   TDISP Graphic Driver subsystem header file.
 *
 * @addtogroup TDISP
 *
 * @details		The TDISP module provides high level abstraction to interface pixel oriented graphic displays.
 *
 * @pre			GFX_USE_TDISP must be set to TRUE in gfxconf.h
 *
 * @{
 */

#ifndef _TDISP_H
#define _TDISP_H

#include "gfx.h"

#if GFX_USE_TDISP || defined(__DOXYGEN__)

/* Include the low level driver information */
#include "tdisp/lld/tdisp_lld.h"

bool_t tdispInit(void);
void tdispClear(void);
void tdispHome(void);
void tdispGotoXY(coord_t col, coord_t row);
void tdispDrawChar(char c);
void tdispDrawString(char *s);
void tdispDrawCharLocation(coord_t x, coord_t y, char c);
void tdispDrawStringLocation(coord_t x, coord_t y, char *s);

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_H */

