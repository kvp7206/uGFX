/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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
 * @file    src/gdisp/image_gif.c
 * @brief   GDISP native image code.
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_GIF

#error "GIF support not implemented yet"

/* A pallete structure */
typedef struct gdispImagePallete {
	uint8_t		flags;
		#define GDISP_IMAGE_FLG_INT_TRANSPARENT		0x01
	uint8_t		idxtrans;		/* The transparent idx */
	uint8_t		maxidx;			/* The maximum index (0..255) */
	uint8_t		repidx;			/* The index to use if the image data > maxidx */
	color_t		pal[256];		/* The pallete entries - not all may actually be allocated */
} gdispImagePallete;

/* Draw a single palletized line (or partial line) */
static void gdispDrawPalleteLine(const gdispImagePallete *pal, const uint8_t *line, coord_t x, coord_t y, coord_t cx);

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE && GDISP_NEED_IMAGE_GIF */
/** @} */
