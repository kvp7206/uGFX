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

/**
 * @brief	TDISP cursor shape definitions
 */
typedef enum cursorshape_e {
	cursorOff,
	cursorBlock,
	cursorBlinkingBlock,
	cursorUnderline,
	cursorBlinkingUnderline,
	cursorBar,
	cursorBlinkingBar,
	} cursorshape;

/**
 * @name	TDISP control values
 * @note	The low level driver may define extra control values
 * @{
 */
#define	TDISP_CTRL_BACKLIGHT	0x0000
#define TDISP_CTRL_CURSOR		0x0001
/** @} */

/**
 * @brief	The TDISP structure definition
 */
typedef struct tdispStruct_t {
	coord_t		columns, rows;
	coord_t		charBitsX, charBitsY;
	uint16_t	maxCustomChars;
	} tdispStruct;

/**
 * @brief	The TDISP structure
 */
extern tdispStruct	TDISP;

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief	Clears the display
 */
void tdispClear(void);

/**
 * @brief	Sets the cursor to it's home position ( 0, 0 )
 */
void tdispHome(void);

/**
 * @brief	Set cursor to a specified position
 *
 * @param[in] col	The column	(x)
 * @param[in] row	The row		(y)
 */
void tdispSetCursor(coord_t col, coord_t row);

/**
 * @brief	Store a custom character into the display
 *
 * @note	This usually must be done after each power-up since most
 *			LCDs lose their RAM content.
 *
 * @param[in] address		On which address to store the character from 0 up to (@p tdispGetNumCustomChars() - 1)
 * @param[in] charmap		The character to be stored.
 *
 * @note					The charmap is made up of @p tdispGetCharBitHieght() data values. Each data value is
 * 							made up of @p tdispGetCharBitWidth() bits of data. Note that bits in multiple rows are not
 * 							packed.
 */
void tdispCreateChar(uint8_t address, uint8_t *charmap);

/**
 * @brief	Draws a single character at the current cursor position and advances the cursor
 *
 * @param[in] c		The character to be drawn
 *
 * @note			Writing past the end of a row leaves the cursor in an undefined position.
 */
void tdispDrawChar(char c);

/**
 * @brief	Draws a string at the current cursor position and advances the cursor
 *
 * @param[in] s		The string to be drawn
 *
 * @note			Any characters written past the end of a row may or may not be displayed on
 * 					the next row. The cursor is also left in an undefined position.
 */
void tdispDrawString(char *s);

/**
 * @brief	Control different display properties
 * @note	A wrapper macro exists for each option, please use them
 *			instead of this function manually unless calling a low
 *			level driver specific value.
 *
 * @param[in] what		What you want to control
 * @param[in] value		The value to be assigned
 */
void tdispControl(uint16_t what, void *value);

/**
 * @brief	Set the backlight level
 *
 * @param[in] percent	A percentage from 0 to 100%. 0% will turn off the display
 */
#define tdispSetBacklight(percent)		tdispControl(TDISP_CTRL_BACKLIGHT, (void *)((uint8_t)(percent)))

/**
 * @brief	Set the cursor shape
 *
 * @param[in] shape	The shape to set the cursor.
 *
 * @note			Not all shapes are necessarily supported. The driver will make a similar
 * 					choice if the one specified is not available.
 */
#define tdispSetCursorShape(shape)		tdispControl(TDISP_CTRL_CURSOR, (void *)((cursorshape)(shape)))

/**
 * @brief	Get the number of columns (width) in the display
 */
#define tdispGetColumns()				(TDISP.columns)

/**
 * @brief	Get the number of rows (height) in the display
 */
#define tdispGetRows()					(TDISP.columns)

/**
 * @brief	Get the number of bits in width of a character
 */
#define tdispGetCharBitWidth()			(TDISP.charBitsX)

/**
 * @brief	Get the number of bits in height of a character
 */
#define tdispGetCharBitHeight()			(TDISP.charBitsY)

/**
 * @brief	Get the number of custom characters
 */
#define tdispGetNumCustomChars()		(TDISP.maxCustomChars)

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_H */
/** @} */

