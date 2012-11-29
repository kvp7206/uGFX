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
 * @file    drivers/gdisp/VMT/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for VMT.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

#define GDISP_LLD_NO_STRUCT

/* Include the emulation code for things we don't support */
#include "lld/gdisp/emulation.c"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#define GDISP_LLD1(x)	GDISP_VMT_NAME1(gdisp_lld_##x##_)
#define GDISP_LLD2(x)	GDISP_VMT_NAME2(gdisp_lld_##x##_)

/* Prototypes for lld driver functions */
bool_t GDISP_LLD1(init)(void);
void GDISP_LLD1(clear)(color_t color);
void GDISP_LLD1(drawpixel)(coord_t x, coord_t y, color_t color);
void GDISP_LLD1(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
void GDISP_LLD1(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
void GDISP_LLD1(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
#if GDISP_NEED_CLIP
	void GDISP_LLD1(setclip)(coord_t x, coord_t y, coord_t cx, coord_t cy);
#endif
#if GDISP_NEED_CIRCLE
	void GDISP_LLD1(drawcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
	void GDISP_LLD1(fillcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
#endif
#if GDISP_NEED_ELLIPSE
	void GDISP_LLD1(drawellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	void GDISP_LLD1(fillellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
#endif
#if GDISP_NEED_ARC
	void GDISP_LLD1(drawarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	void GDISP_LLD1(fillarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
#endif
#if GDISP_NEED_TEXT
	void GDISP_LLD1(drawchar)(coord_t x, coord_t y, char c, font_t font, color_t color);
	void GDISP_LLD1(fillchar)(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
#endif
#if GDISP_NEED_PIXELREAD
	color_t GDISP_LLD1(getpixelcolor)(coord_t x, coord_t y);
#endif
#if GDISP_NEED_SCROLL
	void GDISP_LLD1(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
#endif
#if GDISP_NEED_CONTROL
	void GDISP_LLD1(control)(unsigned what, void *value);
#endif
#if GDISP_NEED_QUERY
	void *GDISP_LLD1(query)(unsigned what);
#endif

bool_t GDISP_LLD2(init)(void);
void GDISP_LLD2(clear)(color_t color);
void GDISP_LLD2(drawpixel)(coord_t x, coord_t y, color_t color);
void GDISP_LLD2(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
void GDISP_LLD2(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
void GDISP_LLD2(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
#if GDISP_NEED_CLIP
	void GDISP_LLD2(setclip)(coord_t x, coord_t y, coord_t cx, coord_t cy);
#endif
#if GDISP_NEED_CIRCLE
	void GDISP_LLD2(drawcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
	void GDISP_LLD2(fillcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
#endif
#if GDISP_NEED_ELLIPSE
	void GDISP_LLD2(drawellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	void GDISP_LLD2(fillellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
#endif
#if GDISP_NEED_ARC
	void GDISP_LLD2(drawarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	void GDISP_LLD2(fillarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
#endif
#if GDISP_NEED_TEXT
	void GDISP_LLD2(drawchar)(coord_t x, coord_t y, char c, font_t font, color_t color);
	void GDISP_LLD2(fillchar)(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
#endif
#if GDISP_NEED_PIXELREAD
	color_t GDISP_LLD2(getpixelcolor)(coord_t x, coord_t y);
#endif
#if GDISP_NEED_SCROLL
	void GDISP_LLD2(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
#endif
#if GDISP_NEED_CONTROL
	void GDISP_LLD2(control)(unsigned what, void *value);
#endif
#if GDISP_NEED_QUERY
	void *GDISP_LLD2(query)(unsigned what);
#endif

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/* Our VMT table variables */
void GDISP_LLD_VMT(clear)(color_t color);
void GDISP_LLD_VMT(drawpixel)(coord_t x, coord_t y, color_t color);
void GDISP_LLD_VMT(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
void GDISP_LLD_VMT(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
void GDISP_LLD_VMT(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

#if GDISP_NEED_CIRCLE
void GDISP_LLD_VMT(drawcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
void GDISP_LLD_VMT(fillcircle)(coord_t x, coord_t y, coord_t radius, color_t color);
#endif

#if GDISP_NEED_ELLIPSE
void GDISP_LLD_VMT(drawellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
void GDISP_LLD_VMT(fillellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
#endif

/* Text Rendering Functions */
#if GDISP_NEED_TEXT
void GDISP_LLD_VMT(drawchar)(coord_t x, coord_t y, char c, font_t font, color_t color);
void GDISP_LLD_VMT(fillchar)(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor);
#endif

/* Pixel readback */
#if GDISP_NEED_PIXELREAD
color_t GDISP_LLD_VMT(getpixelcolor)(coord_t x, coord_t y);
#endif

/* Scrolling Function - clears the area scrolled out */
#if GDISP_NEED_SCROLL
void GDISP_LLD_VMT(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
#endif

/* Set driver specific control */
#if GDISP_NEED_CONTROL
void GDISP_LLD_VMT(control)(unsigned what, void *value);
#endif
/* Set driver specific control */
#if GDISP_NEED_QUERY
void *GDISP_LLD_VMT(query)(unsigned what);
#endif
/* Clipping Functions */
#if GDISP_NEED_CLIP
void GDISP_LLD_VMT(setclip)(coord_t x, coord_t y, coord_t cx, coord_t cy);
#endif


/*===========================================================================*/
/* Driver local variables.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

bool_t	gdisp_lld_init_VMT(void) {
	if (GDISP_VMT_NAME1(gdisp_lld_init_)()) {
		gdisp_lld_clear_VMT					= GDISP_VMT_NAME1(gdisp_lld_clear_);
		gdisp_lld_drawpixel_VMT				= GDISP_VMT_NAME1(gdisp_lld_drawpixel_);
		gdisp_lld_fillarea_VMT				= GDISP_VMT_NAME1(gdisp_lld_fillarea_);
		gdisp_lld_blitareaex_VMT			= GDISP_VMT_NAME1(gdisp_lld_blitareaex_);
		gdisp_lld_drawline_VMT				= GDISP_VMT_NAME1(gdisp_lld_drawline_);
		#if GDISP_NEED_CIRCLE
			gdisp_lld_drawcircle_VMT		= GDISP_VMT_NAME1(gdisp_lld_drawcircle_);
			gdisp_lld_fillcircle_VMT		= GDISP_VMT_NAME1(gdisp_lld_fillcircle_);
		#endif
		#if GDISP_NEED_ELLIPSE
			gdisp_lld_drawellipse_VMT		= GDISP_VMT_NAME1(gdisp_lld_drawellipse_);
			gdisp_lld_fillellipse_VMT		= GDISP_VMT_NAME1(gdisp_lld_fillellipse_);
		#endif
		#if GDISP_NEED_ARC
			gdisp_lld_drawarc_VMT			= GDISP_VMT_NAME1(gdisp_lld_drawarc_);
			gdisp_lld_fillarc_VMT			= GDISP_VMT_NAME1(gdisp_lld_fillarc_);
		#endif
		#if GDISP_NEED_TEXT
			gdisp_lld_drawchar_VMT			= GDISP_VMT_NAME1(gdisp_lld_drawchar_);
			gdisp_lld_fillchar_VMT			= GDISP_VMT_NAME1(gdisp_lld_fillchar_);
		#endif
		#if GDISP_NEED_PIXELREAD
			gdisp_lld_getpixelcolor_VMT		= GDISP_VMT_NAME1(gdisp_lld_pixelread_);
		#endif
		#if GDISP_NEED_SCROLL
			gdisp_lld_verticalscroll_VMT	= GDISP_VMT_NAME1(gdisp_lld_scroll_);
		#endif
		#if GDISP_NEED_CONTROL
			gdisp_lld_control_VMT			= GDISP_VMT_NAME1(gdisp_lld_control_);
		#endif
		#if GDISP_NEED_QUERY
			gdisp_lld_query_VMT				= GDISP_VMT_NAME1(gdisp_lld_query_);
		#endif
		#if GDISP_NEED_CLIP
			gdisp_lld_setclip_VMT			= GDISP_VMT_NAME1(gdisp_lld_setclip_);
		#endif

		return TRUE;
	}

	if (GDISP_VMT_NAME2(gdisp_lld_init_)()) {
		gdisp_lld_clear_VMT					= GDISP_VMT_NAME2(gdisp_lld_clear_);
		gdisp_lld_drawpixel_VMT				= GDISP_VMT_NAME2(gdisp_lld_drawpixel_);
		gdisp_lld_fillarea_VMT				= GDISP_VMT_NAME2(gdisp_lld_fillarea_);
		gdisp_lld_blitareaex_VMT			= GDISP_VMT_NAME2(gdisp_lld_blitareaex_);
		gdisp_lld_drawline_VMT				= GDISP_VMT_NAME2(gdisp_lld_drawline_);
		#if GDISP_NEED_CIRCLE
			gdisp_lld_drawcircle_VMT		= GDISP_VMT_NAME2(gdisp_lld_drawcircle_);
			gdisp_lld_fillcircle_VMT		= GDISP_VMT_NAME2(gdisp_lld_fillcircle_);
		#endif
		#if GDISP_NEED_ELLIPSE
			gdisp_lld_drawellipse_VMT		= GDISP_VMT_NAME2(gdisp_lld_drawellipse_);
			gdisp_lld_fillellipse_VMT		= GDISP_VMT_NAME2(gdisp_lld_fillellipse_);
		#endif
		#if GDISP_NEED_ARC
			gdisp_lld_drawarc_VMT			= GDISP_VMT_NAME2(gdisp_lld_drawarc_);
			gdisp_lld_fillarc_VMT			= GDISP_VMT_NAME2(gdisp_lld_fillarc_);
		#endif
		#if GDISP_NEED_TEXT
			gdisp_lld_drawchar_VMT			= GDISP_VMT_NAME2(gdisp_lld_drawchar_);
			gdisp_lld_fillchar_VMT			= GDISP_VMT_NAME2(gdisp_lld_fillchar_);
		#endif
		#if GDISP_NEED_PIXELREAD
			gdisp_lld_getpixelcolor_VMT		= GDISP_VMT_NAME2(gdisp_lld_pixelread_);
		#endif
		#if GDISP_NEED_SCROLL
			gdisp_lld_verticalscroll_VMT	= GDISP_VMT_NAME2(gdisp_lld_scroll_);
		#endif
		#if GDISP_NEED_CONTROL
			gdisp_lld_control_VMT			= GDISP_VMT_NAME2(gdisp_lld_control_);
		#endif
		#if GDISP_NEED_QUERY
			gdisp_lld_query_VMT				= GDISP_VMT_NAME2(gdisp_lld_query_);
		#endif
		#if GDISP_NEED_CLIP
			gdisp_lld_setclip_VMT			= GDISP_VMT_NAME2(gdisp_lld_setclip_);
		#endif

		return TRUE;
	}
	return FALSE;
}

#endif /* GFX_USE_GDISP */
/** @} */

