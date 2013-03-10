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
	GBTN_3D, GBTN_SQUARE, GBTN_ROUNDED, GBTN_ELLIPSE, GBTN_CUSTOM,
	GBTN_ARROW_UP, GBTN_ARROW_DOWN, GBTN_ARROW_LEFT, GBTN_ARROW_RIGHT,
} GButtonShape;

typedef struct GButtonDrawStyle_t {
	color_t				color_edge;
	color_t				color_fill;
	color_t				color_txt;
} GButtonDrawStyle;

typedef enum GButtonType_e {
	GBTN_NORMAL, GBTN_TOGGLE
} GButtonType;

typedef enum GButtonState_e {
	GBTN_UP, GBTN_DOWN
} GButtonState;

typedef void (*GButtonDrawFunction)(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);

// A button window
typedef struct GButtonObject_t {
	GWindowObject		gwin;

	GButtonDrawStyle	up;
	GButtonDrawStyle	dn;
	GButtonState		state;
	GButtonType			type;
	const char			*txt;
	GButtonDrawFunction	fn;
	void				*param;
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
 * @param[in] shape		The shape of the button.
 * @param[in] pUp		The styling for the button when in the up state.
 * @param[in] pDown		The styling for the button when in the down state.
 *
 * @note				The button is not automatically redrawn. Call gwinButtonDraw() after changing the button style
 * @note				The button style is copied into the internal button structure - there is no need to
 * 						maintain a static style structures.
 * @note				The pUp and pDown parameters can be NULL. If they are then the existing color styles
 * 						are not changed for that button state.
 *
 * @api
 */
void gwinSetButtonStyle(GHandle gh, GButtonShape shape, const GButtonDrawStyle *pUp, const GButtonDrawStyle *pDown);

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

/**
 * @brief   Set the callback routine to perform a custom button drawing.
 *
 * @param[in] gh		The window handle (must be a button window)
 * @param[in] fn		The function to use to draw the button
 * @param[in] param		A parameter to pass to the button drawing function
 *
 * @api
 */
void gwinSetButtonCustom(GHandle gh, GButtonDrawFunction fn, void *param);

#define gwinGetButtonState(gh)		(((GButtonObject *)(gh))->state)

/**
 * @brief Get the source handle of a button
 * @details Get the source handle of a button so the application can listen for events
 *
 * @param[in] gh	The window handle
 *
 * @api
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
	 * @api
	 */
	bool_t gwinAttachButtonMouseSource(GHandle gh, GSourceHandle gsh);
#endif

#if defined(GINPUT_NEED_TOGGLE) && GINPUT_NEED_TOGGLE
	/**
	 * @brief	Attach a toggle source
	 * @details	Attach a toggle source to this button
	 *
	 * @param[in] gh	The button handle
	 * @param[in] gsh	The source handle
	 *
	 * @api
	 */
	bool_t gwinAttachButtonToggleSource(GHandle gh, GSourceHandle gsh);
#endif

/**
 * @brief	Standard button drawing routines
 * @details	These routines are called to draw the standard button styles.
 *
 * @param[in] gh		The button handle
 * @param[in] isdown	Is the button currently down (depressed)
 * @param[in] pstyle	The current drawing style for the state we are in
 * @param[in] param		A parameter passed in from the user
 *
 * @note				In your custom button drawing function you may optionally call these
 * 						standard functions and then draw your extra details on top.
 * @note				The standard functions below ignore the param parameter. It is there
 * 						only to ensure the functions match the GButtonDrawFunction type.
 * @note				When called by a button press/release the framework ensure that it is
 * 						a button object and sets up clipping to the button object window. These
 * 						drawing routines then don't have to worry about explicitly doing that.
 *
 * @api
 * @{
 */
void gwinButtonDraw_3D(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
void gwinButtonDraw_Square(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
#if GDISP_NEED_ARC || defined(__DOXYGEN__)
	void gwinButtonDraw_Rounded(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
#endif
#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
	void gwinButtonDraw_Ellipse(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
#endif
#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
	void gwinButtonDraw_ArrowUp(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
	void gwinButtonDraw_ArrowDown(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
	void gwinButtonDraw_ArrowLeft(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
	void gwinButtonDraw_ArrowRight(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param);
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_BUTTON */

#endif /* _GWIN_BUTTON_H */
/** @} */
