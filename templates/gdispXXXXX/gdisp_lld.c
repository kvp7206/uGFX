/*
    ChibiOS/RT - Copyright (C) 2012
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
 * @file    templates/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source template.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if HAL_USE_GDISP || defined(__DOXYGEN__)

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"

#if GDISP_NEED_TEXT && GDISP_HARDWARE_TEXT
	#include "gdisp_fonts.h"
#endif

/* All the board specific code should go in these include file so the driver
 * can be ported to another board just by creating a suitable file.
 */
#if defined(BOARD_YOURBOARDNAME)
	#include "gdisp_lld_board_yourboardname.h"
#else
	/* Include the user supplied board definitions */
	#include "gdisp_lld_board.h"
#endif

/*===========================================================================*/
/* Driver interrupt handlers.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
	The following 2 routines are required.
	All other routines are optional.
*/

/**
 * @brief   Low level GDISP driver initialisation.
 * @return	TRUE if successful, FALSE on error.
 *
 * @notapi
 */
bool_t GDISP_LLD(init)(void) {
	/* Initialise your display */

	/* Initialise the GDISP structure to match */
	GDISP.Width = SCREEN_WIDTH;
	GDISP.Height = SCREEN_HEIGHT;
	GDISP.Orientation = portrait;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		GDISP.clipx0 = 0;
		GDISP.clipy0 = 0;
		GDISP.clipx1 = GDISP.Width;
		GDISP.clipy1 = GDISP.Height;
	#endif
	return TRUE;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif
	/* Code here */
}

/* ---- Optional Routines ---- */
/*
	All the below routines are optional.
	Defining them will increase speed but everything
	will work if they are not defined.
	If you are not using a routine - turn it off using
	the appropriate GDISP_HARDWARE_XXXX macro.
	Don't bother coding for obvious similar routines if
	there is no performance penalty as the emulation software
	makes a good job of using similar routines.
		eg. If fillarea() is defined there is little
			point in defining clear() unless the
			performance bonus is significant.
	For good performance it is suggested to implement
		fillarea() and blitarea().
*/

#if GDISP_HARDWARE_CLEARS || defined(__DOXYGEN__)
	/**
	 * @brief   Clear the display.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] color    The color of the pixel
	 *
	 * @notapi
	 */
	void GDISP_LLD(clear)(color_t color) {
		/* Code here */
	}
#endif

#if GDISP_HARDWARE_LINES || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a line.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x0, y0   The start of the line
	 * @param[in] x1, y1   The end of the line
	 * @param[in] color    The color of the line
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		/* Code here */
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] srcx, srcy   The bitmap position to start the fill from
	 * @param[in] srccx    The width of a line in the bitmap.
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		/* Code here */
	}
#endif

/* Circular Drawing Functions */
#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawcircle)(coord_t x, coord_t y, coord_t radius, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_CIRCLE && GDISP_HARDWARE_CIRCLEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled circle.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the circle
	 * @param[in] radius   The radius of the circle
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillcircle)(coord_t x, coord_t y, coord_t radius, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSES) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_ELLIPSE && GDISP_HARDWARE_ELLIPSEFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled ellipse.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the ellipse is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the ellipse
	 * @param[in] a, b     The dimensions of the ellipse
	 * @param[in] color    The color of the ellipse
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillellipse)(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

/* Arc Drawing Functions */
#if (GDISP_NEED_ARC && GDISP_HARDWARE_ARCS) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an arc.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the arc circle
	 * @param[in] radius   The radius of the arc circle
	 * @param[in] startangle, endangle   The start and end angles for the arc (0..359)
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_ARC && GDISP_HARDWARE_ARCFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Create a filled arc.
	 * @note    Optional - The high level driver can emulate using software.
	 * @note    If GDISP_NEED_CLIPPING is defined this routine MUST behave
	 *          correctly if the circle is over the edges of the screen.
	 *
	 * @param[in] x, y     The centre of the arc circle
	 * @param[in] radius   The radius of the arc circle
	 * @param[in] startangle, endangle   The start and end angles for the arc (0..359)
	 * @param[in] color    The color of the circle
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarc)(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXT) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a transparent background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawchar)(coord_t x, coord_t y, char c, font_t font, color_t color) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_TEXT && GDISP_HARDWARE_TEXTFILLS) || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a character using a filled background.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The top-left corner of the text
	 * @param[in] c        The character to print
	 * @param[in] color    The color of the character
	 * @param[in] bgcolor  The background color
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillchar)(coord_t x, coord_t y, char c, font_t font, color_t color, color_t bgcolor) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			/* Code here */
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 * @return	The color of the specified pixel.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		/* Code here */
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @detail	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
	 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
	 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
	 * 											this value.
	 *
	 * @param[in] what		What to do.
	 * @param[in] value		The value to use (always cast to a void *).
	 *
	 * @notapi
	 */
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
		case GDISP_CONTROL_POWER:
			if (GDISP.Powermode == (gdisp_powermode_t)value)
				return;
			switch((gdisp_powermode_t)value) {
				case powerOff:
					/* 	Code here */
					break;
				case powerOn:
					/* 	Code here */
					/* You may need this ---
						if (GDISP.Powermode != powerSleep)
							GDISP_LLD(init)();
					*/
					break;
				case powerSleep:
					/* 	Code here */
					break;
				default:
					return;
			}
			GDISP.Powermode = (gdisp_powermode_t)value;
			return;
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			switch((gdisp_orientation_t)value) {
				case portrait:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscape:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				case portraitInv:
					/* 	Code here */
					GDISP.Height = SCREEN_HEIGHT;
					GDISP.Width = SCREEN_WIDTH;
					break;
				case landscapeInv:
					/* 	Code here */
					GDISP.Height = SCREEN_WIDTH;
					GDISP.Width = SCREEN_HEIGHT;
					break;
				default:
					return;
			}
			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#if (GDISP_NEED_QUERY && GDISP_HARDWARE_QUERY) || defined(__DOXYGEN__)
/**
 * @brief   Query a driver value.
 * @detail	Typecase the result to the type you want.
 * @note	GDISP_QUERY_WIDTH			- (coord_t)	Gets the width of the screen
 * 			GDISP_QUERY_HEIGHT			- (coord_t)	Gets the height of the screen
 * 			GDISP_QUERY_POWER			- (gdisp_powermode_t) Get the current powermode
 * 			GDISP_QUERY_ORIENTATION		- (gdisp_orientation_t) Get the current screen orientation
 * 			GDISP_QUERY_BACKLIGHT		- (coord_t) Get the backlight state (0 to 100)
 * 			GDISP_QUERY_CONTRAST		- (coord_t) Get the contrast (0 to 100).
 * 			GDISP_QUERY_LLD				- Low level driver control constants start at
 * 											this value.
 *
 * @param[in] what     What to Query
 *
 * @notapi
 */
void *GDISP_LLD(query)(unsigned what) {
	switch(what) {
	case GDISP_QUERY_WIDTH:			return (void *)(unsigned)GDISP.Width;
	case GDISP_QUERY_HEIGHT:		return (void *)(unsigned)GDISP.Height;
	case GDISP_QUERY_POWER:			return (void *)(unsigned)GDISP.Powermode;
	case GDISP_QUERY_ORIENTATION:	return (void *)(unsigned)GDISP.Orientation;
	case GDISP_QUERY_BACKLIGHT:		return (void *)(unsigned)GDISP.Backlight;
	case GDISP_QUERY_CONTRAST:		return (void *)(unsigned)GDISP.Contrast;
	case GDISP_QUERY_LLD+0:
		/* Code here */
	default:						return (void *)-1;
	}
}
#endif

#if GDISP_NEED_CLIP && GDISP_HARDWARE_CLIP
	void GDISP_LLD(setclip)(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		#if GDISP_NEED_VALIDATION
			if (x >= GDISP.Width || y >= GDISP.Height || cx < 0 || cy < 0)
				return;
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x+cx > GDISP.Width) cx = GDISP.Width - x;
			if (y+cy > GDISP.Height) cy = GDISP.Height - y;
		#endif
		GDISP.clipx0 = x;
		GDISP.clipy0 = y;
		GDISP.clipx1 = x+cx;
		GDISP.clipy1 = y+cy;
		/* Code here to set hardware clipping */
	}
#endif

#endif /* HAL_USE_GDISP */
/** @} */
