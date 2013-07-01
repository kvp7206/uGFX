/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/gwidget.h
 * @brief   GWIN Widgets header file.
 */

#ifndef _GWIDGET_H
#define _GWIDGET_H

/* This file is included within "gwin/gwin.h" */

/**
 * @defgroup Widget Widget
 * @ingroup GWIN
 *
 * @details		A Widget is a GWindow that supports interacting with the user
 * 				via an input device such as a mouse or toggle buttons. It is the
 * 				base class for widgets such as buttons and sliders.
 *
 * @pre		GFX_USE_GWIN and GWIN_NEED_WIDGET must be set to TRUE in your gfxconf.h
 * @{
 */

// Forward definition
struct GWidgetObject;

/**
 * @brief	Defines a custom drawing function for a widget
 */
typedef void (*CustomWidgetDrawFunction)(struct GWidgetObject *gw, void *param);

/**
 * @brief	The GWIN Widget structure
 * @note	A widget is a GWIN window that accepts user input.
 * 			It also has a number of other properties such as its ability
 * 			to redraw itself (a widget maintains drawing state).
 * @note	Do not access the members directly. Treat it as a black-box and use the method functions.
 *
 * @{
 */
typedef struct GWidgetObject {
	GWindowObject				g;					// @< This is still a GWIN
	const char *				txt;				// @< The widget text
	CustomWidgetDrawFunction	fnDraw;				// @< The current draw function
	void *						fnParam;			// @< A parameter for the current draw function
} GWidgetObject;
/* @} */

/**
 * @brief	The structure to initialise a widget.
 *
 * @note	Some widgets may have extra parameters.
 * @note	The text element must be static string (not stack allocated). If you want to use
 * 			a dynamic string (eg a stack allocated string) use NULL for this member and then call
 * 			@p gwinSetText() with useAlloc set to TRUE.
 *
 * @{
 */
typedef struct GWidgetInit {
	GWindowInit		g;								// @< The GWIN initializer
	const char *	text;							// @< The initial text
} GWidgetInit;
/* @} */

/**
 * A comment/rant on the above structure:
 * We would really like the GWindowObject member to be anonymous. While this is
 * allowed under the C11, C99, GNU and various other standards which have been
 * around forever - compiler support often requires special flags e.g
 * gcc requires the -fms-extensions flag (no wonder the language and compilers have
 * not really progressed in 30 years). As portability is a key requirement
 * we unfortunately won't use this useful feature in case we get a compiler that
 * won't support it even with special flags.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Enable or disable a widget
 *
 * @param[in] gh		The widget handle
 * @param[in] enabled	Enable or disable the widget
 *
 * @note				The widget is automatically redrawn.
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetEnabled(GHandle gh, bool_t enabled);

/**
 * @brief   Set the text of a widget.
 *
 * @param[in] gh		The widget handle
 * @param[in] txt		The text to set. This must be a constant string unless useAlloc is set.
 * @param[in] useAlloc	If TRUE the string specified will be copied into dynamically allocated memory.
 *
 * @note				The widget is automatically redrawn
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetText(GHandle gh, const char *txt, bool_t useAlloc);

/**
 * @brief   Get the text of a widget.
 * @return	The widget text or NULL if it isn't a widget
 *
 * @param[in] gh		The widget handle
 *
 * @api
 */
const char *gwinGetText(GHandle gh);

/**
 * @brief   Set the routine to perform a custom widget drawing.
 *
 * @param[in] gh		The widget handle
 * @param[in] fn		The function to use to draw the widget
 * @param[in] param		A parameter to pass to the widget drawing function
 *
 * @note				The widget is not automatically redrawn. Call @p gwinDraw() to redraw the widget.
 * @note				Non-widgets will ignore this call.
 *
 * @api
 */
void gwinSetCustomDraw(GHandle gh, CustomWidgetDrawFunction fn, void *param);

/**
 * @brief	Attach a Listener to listen for widget events
 * @return	TRUE on success
 *
 * @param[in] pl		The listener
 *
 * @api
 */
bool_t gwinAttachListener(GListener *pl);

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	/**
	 * @brief	Set the mouse to be used to control the widgets
	 * @return	TRUE on success
	 *
	 * @param[in] instance	The mouse instance
	 *
	 * @note	Every widget uses the same mouse.
	 *
	 * @api
	 */
	bool_t gwinAttachMouse(uint16_t instance);
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	/**
	 * @brief	Attach a toggle to a widget
	 * @return	TRUE on success
	 *
	 * @param[in] gh		The widget handle
	 * @param[in] role		The function the toggle will perform for the widget
	 * @param[in] instance	The toggle instance
	 *
	 * @note		See the documentation on the specific widget to see the possible
	 * 				values for the role parameter. If it is out of range, this function
	 * 				will return FALSE
	 *
	 * @api
	 */
	bool_t gwinAttachToggle(GHandle gh, uint16_t role, uint16_t instance);
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	/**
	 * @brief	Attach a toggle to a widget
	 * @return	TRUE on success
	 *
	 * @param[in] gh		The widget handle
	 * @param[in] role		The function the dial will perform for the widget
	 * @param[in] instance	The dial instance
	 *
	 * @note		See the documentation on the specific widget to see the possible
	 * 				values for the role parameter. If it is out of range, this function
	 * 				will return FALSE
	 *
	 * @api
	 */
	bool_t gwinAttachDial(GHandle gh, uint16_t role, uint16_t instance);
#endif

#ifdef __cplusplus
}
#endif

/* Include extra widget types */
#if GWIN_NEED_BUTTON || defined(__DOXYGEN__)
	#include "gwin/button.h"
#endif
#if GWIN_NEED_SLIDER || defined(__DOXYGEN__)
	#include "gwin/slider.h"
#endif
#if GWIN_NEED_CHECKBOX || defined(__DOXYGEN__)
	#include "gwin/checkbox.h"
#endif

#endif /* _GWIDGET_H */
/** @} */
