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

/**
 * @brief   Create a button window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gb		The GButtonObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @param[in] font		The font to use
 * @param[in] type		The type of button
 * @note				The drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The button is not automatically drawn. Call gwinButtonDraw() after changing the button style or setting the text.
 *
 * @api
 */	
GHandle gwinCreateButton(GButtonObject *gb, coord_t x, coord_t y, coord_t width, coord_t height, font_t font, GButtonType type);

/**
 * @brief   Set the style of a button.
 * @details	The button style is defined by its shape and colours.
 *
 * @param[in] gh		The window handle (must be a button window)
 * @param[in] style		The button style to set.
 * @note				The button is not automatically redrawn. Call gwinButtonDraw() after changing the button style
 *
 * @api
 */
void gwinSetButtonStyle(GHandle gh, const GButtonStyle *style);

/**
 * @brief   Set the text of a button.
 *
 * @param[in] gh		The window handle (must be a button window)
 * @param[in] txt		The button text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 * @note				The button is not automatically redrawn. Call gwinButtonDraw() after changing the button text.
 *
 * @api
 */
void gwinSetButtonText(GHandle gh, const char *txt, bool_t useAlloc);

/**
 * @brief   Redraw the button.
 *
 * @param[in] gh		The window handle (must be a button window)
 *
 * @api
 */
void gwinButtonDraw(GHandle gh);

#define gwinGetButtonState(gh)		(((GButtonObject *)(gh))->state)

/**
 * @brief Get the source handle of a button
 * @details Get the source handle of a button so the application can listen for events
 *
 * @param[in] gh	The Hanlde
 */
#define gwinGetButtonSource(gh)		((GSourceHandle)(gh))

#if defined(GINPUT_NEED_MOUSE) && GINPUT_NEED_MOUSE
	/**
	 * @brief	Attach a mouse source
	 * @details	Attach a mouse source to a given button
	 *
	 * @param[in] gh	The button handle
	 * @param[in] gsh	The source handle
	 *
	 * @return
	 */
	bool_t gwinAttachButtonMouseSource(GHandle gh, GSourceHandle gsh);
#endif

#if defined(GINPUT_NEED_TOGGLE) && GINPUT_NEED_TOGGLE
	/**
	 * @brief	Attach a toggle source
	 * @details	Attach a toggle source to this button
	 *
	 * @gh		The button handle
	 * @gsh		The source handle
	 *
	 * @return
	 */
	bool_t gwinAttachButtonToggleSource(GHandle gh, GSourceHandle gsh);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_BUTTON */

#endif /* _GWIN_BUTTON_H */
/** @} */
