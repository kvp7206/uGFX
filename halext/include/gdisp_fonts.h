/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> Framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/**
 * @file    gdisp_fonts.h
 * @brief   GDISP internal font definitions.
 * @details	This is not generally needed by an application. It is used
 * 			by the low level drivers that need to understand a font.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_FONTS_H
#define _GDISP_FONTS_H

/**
 * @brief   The maximum height of a font.
 * @details	Either 16 or 32. Defaults to 16
 * @note	Setting this to 32 causes the font tables to take
 *			twice the internal program memory.
 */
#ifndef GDISP_MAX_FONT_HEIGHT
	#define GDISP_MAX_FONT_HEIGHT	16
#endif

/**
 * @brief   The type of a font column.
 * @note	Set by defining @p GDISP_MAX_FNT_HEIGHT appropriately.
 */
#if GDISP_MAX_FONT_HEIGHT == 16
	typedef uint16_t	fontcolumn_t;
#elif GDISP_MAX_FONT_HEIGHT == 32
	typedef uint32_t	fontcolumn_t;
#else
	#error "GDISP: GDISP_MAX_FONT_HEIGHT must be either 16 or 32"
#endif

/**
 * @brief   Internal font structure.
 * @note	This structure is followed by:
 *				1. An array of character widths (uint8_t)
 *				2. An array of column data offsets (relative to the font structure)
 *				3. Each characters array of column data (fontcolumn_t)
 *			Each sub-structure must be padded to a multiple of 8 bytes
 *			to allow the tables to work accross many different compilers.
 */
struct font {
	uint8_t		height;
	uint8_t		charPadding;
	uint8_t		lineSpacing;
	uint8_t		descenderHeight;
	uint8_t		minWidth;
	uint8_t		maxWidth;
	char		minChar;
	char		maxChar;
	uint16_t	offsetTableOffset;
	uint16_t	unused1;			/* ensure next field is padded to 8 byte boundary */
	uint8_t		widthTable[];
	};

/**
 * @brief   Macro's to get to the complex parts of the font structure.
 */
#define _getFontPart(f,o,t)		((t)(&((const uint8_t *)(f))[(o)]))
#define _getCharWidth(f,c)		(((c) < (f)->minChar || (c) > (f)->maxChar) ? 0 : (f)->widthTable[c - (f)->minChar])
#define _getCharOffset(f,c)		(_getFontPart((f), (f)->offsetTableOffset, const uint16_t *)[c - (f)->minChar])
#define _getCharData(f,c)		_getFontPart((f), _getCharOffset((f),(c)), const fontcolumn_t *)

#endif /* _GDISP_FONTS_H */
/** @} */
