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
 *				Due the TDISP module is completely encapsulated from the other modules, it's very fast and lightweight.
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

/**
 * @name	TDISP display attributes
 * @{
 */
#define	TDISP_ON				0x01
#define TDISP_OFF				0x02
#define TDISP_CURSOR_ON			0x03
#define TDISP_CURSOR_OFF		0x04
#define TDISP_CURSOR_BLINK_ON	0x05
#define TDISP_CURSOR_BLINK_OFF	0x06
/** @} */

/**
 * @brief	TDISP driver initialisation
 * @note	This function is not implicitly invoked by @p halInit().
 *			It must be called manually.
 *
 * @return	TRUE if success, FALSE otherwise
 *
 * @init
 */
bool_t tdispInit(void);

/**
 * @brief	Control different display properties
 * @note	Multiple attributes can be passed using the OR operator.
 * @note	Example: tdispSetAttributes(TDISP_DISPLAY_ON | TDISP_CURSOR_BLINK)
 *
 * @param[in] attributes	The attributes
 */
void tdispSetAttributes(uint8_t attributes);

/**
 * @brief	Clears the display
 */
void tdispClear(void);

/**
 * @brief	Sets the cursor to it's home position ( 0/0 )
 */
void tdispHome(void);

/**
 * @brief	Set cursor to a certain position
 *
 * @param[in] col	The column
 * @param[in] row	The row
 */
void tdispGotoXY(coord_t col, coord_t row);

/**
 * @brief	Store a custom character in RAM
 *
 * @note	This usually must be done after each power-up since most
 *			LCDs lose their RAM content.
 *
 * @param[in] location		On which address to store the character (from 0 up to max)
 * @param[in] charmap		The character to be stored.
 */
void tdispCreateChar(uint8_t location, char *charmap);

/**
 * @brief	Draws a single character at the current cursor position
 *
 * @param[in] c		The character to be drawn
 */
void tdispDrawChar(char c);

/**
 * @brief	Draws a string at the current cursor position
 *
 * @param[in] s		The string to be drawn
 */
void tdispDrawString(char *s);

/**
 * @brief	Draws a single character at a given position
 * @note	This function manipulates the cursor position and it will not be
 *			reset to it's original state
 *
 * @param[in] col	The column
 * @param[in] row	The row
 * @param[in] c		The character to be drawn
 */
void tdispDrawCharLocation(coord_t col, coord_t row, char c);

/**
 * @brief	Draws a string at a given position
 * @note	This function manipulates the cursor position and it will not be
 *			reset to it's original state
 *
 * @param[in] col	The column
 * @param[in] row	The row
 * @param[in] s		The string to be drawn
 */
void tdispDrawStringLocation(coord_t col, coord_t row, char *s);

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_H */
/** @} */

