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
 * @file    include/gwin/button.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Button Button
 * @ingroup GWIN
 *
 * @details		GWIN allows it to easily create buttons with different styles
 *				and check for different meta states such as: PRESSED, CLICKED,
 *				RELEASED etc.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_BUTTON must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_BUTTON_H
#define _GWIN_BUTTON_H

#if GWIN_NEED_BUTTON || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_BUTTON				0x0002
#define GEVENT_GWIN_BUTTON		(GEVENT_GWIN_FIRST+0)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct GEventGWinButton_t {
	GEventType		type;				// The type of this event (GEVENT_GWIN_BUTTON)
	GHandle			button;				// The button that has been depressed (actually triggered on release)
} GEventGWinButton;

// There are currently no GEventGWinButton listening flags - use 0

typedef enum GButtonShape_e {
	GBTN_3D, GBTN_SQUARE, GBTN_ROUNDED, GBTN_ELLIPSE
} GButtonShape;

typedef struct GButtonStyle_t {
	GButtonShape		shape;
	color_t				color_up_edge;
	color_t				color_up_fill;
	color_t				color_up_txt;
	color_t				color_dn_edge;
	color_t				color_dn_fill;
	color_t				color_dn_txt;
} GButtonStyle;

typedef enum GButtonType_e {
	GBTN_NORMAL, GBTN_TOGGLE
} GButtonType;

typedef enum GButtonState_e {
	GBTN_UP, GBTN_DOWN
} GButtonState;

// A button window
typedef struct GButtonObject_t {
	GWindowObject		gwin;

	GButtonStyle		style;
	GButtonState		state;
	GButtonType			type;
	const char *		txt;
	GListener			listener;
} GButtonObject;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	GHandle gwinCreateButton(GButtonObject *gb, coord_t x, coord_t y, coord_t width, coord_t height, font_t font, GButtonType type);
	void gwinSetButtonStyle(GHandle gh, const GButtonStyle *style);
	void gwinSetButtonText(GHandle gh, const char *txt, bool_t useAlloc);
	void gwinButtonDraw(GHandle gh);
	#define gwinGetButtonState(gh)		(((GButtonObject *)(gh))->state)

	// Get the source handle so the application can listen for events
	#define gwinGetButtonSource(gh)		((GSourceHandle)(gh))

	#if defined(GINPUT_NEED_MOUSE) && GINPUT_NEED_MOUSE
		// Attach a mouse source to this button.
		bool_t gwinAttachButtonMouseSource(GHandle gh, GSourceHandle gsh);
	#endif

	#if defined(GINPUT_NEED_TOGGLE) && GINPUT_NEED_TOGGLE
		// Attach a toggle source to this button.
		bool_t gwinAttachButtonToggleSource(GHandle gh, GSourceHandle gsh);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_BUTTON */

#endif /* _GWIN_BUTTON_H */
/** @} */
