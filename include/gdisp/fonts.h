/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gdisp/fonts.h
 * @brief   GDISP internal font definitions.
 * @details	This is not generally needed by an application. It is used
 * 			by the low level drivers that need to understand a font.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_FONTS_H
#define _GDISP_FONTS_H

/* Don't test against GFX_USE_GDISP as we may want to use this in other non-GDISP utilities. */

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
 *			to allow the tables to work across many different compilers.
 */
struct font {
	const char *		name;
	uint8_t				height;
	uint8_t				charPadding;
	uint8_t				lineSpacing;
	uint8_t				descenderHeight;
	uint8_t				minWidth;
	uint8_t				maxWidth;
	char				minChar;
	char				maxChar;
	uint8_t				xscale;
	uint8_t				yscale;
	const uint8_t		*widthTable;
	const uint16_t      *offsetTable;
	const fontcolumn_t  *dataTable;
};

#define _getCharWidth(f,c)		(((c) < (f)->minChar || (c) > (f)->maxChar) ? 0 : (f)->widthTable[(c) - (f)->minChar])
#define _getCharOffset(f,c)		((f)->offsetTable[(c) - (f)->minChar])
#define _getCharData(f,c)		(&(f)->dataTable[_getCharOffset(f, c)])

#endif /* _GDISP_FONTS_H */
/** @} */

