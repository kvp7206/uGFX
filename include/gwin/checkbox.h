/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/checkbox.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Checkbox Checkbox
 * @ingroup GWIN
 *
 * @details		GWIN allows it to easily create a group of checkbox buttons.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_CHECKBOX must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_CHECKBOX_H
#define _GWIN_CHECKBOX_H

/* This file is included within "gwin/gwidget.h" */

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GEVENT_GWIN_CHECKBOX		(GEVENT_GWIN_FIRST+2)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct GEventGWinCheckbox_t {
	GEventType		type;			// The type of this event (GEVENT_GWIN_CHECKBOX)
	GHandle			checkbox;		// The checkbox that has been depressed (actually triggered on release)
	bool_t			isChecked;		// Is the checkbox currently checked or unchecked?
} GEventGWinCheckbox;

typedef struct GCheckboxColors {
	color_t	color_border;
	color_t color_checked;
	color_t color_bg;
	color_t color_txt;
} GCheckboxColors;

/* A Checkbox window */
typedef struct GCheckboxObject_t {
	GWidgetObject			w;
	GCheckboxColors			c;
} GCheckboxObject;

/**
 * @brief				Create a checkbox window.
 * @return				NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gb		The GCheckboxObject structure to initialise. If this is NULL, the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the top left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 *
 * @note				The drawing color gets set to White and the background drawing color to Black.
 * @note				Don't forget to set the font using @p gwinSetFont() or @p gwinSetDefaultFont()
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The checkbox is not automatically drawn. Call gwinDraw() to draw it.
 *
 * @api
 */
GHandle gwinCreateCheckbox(GCheckboxObject *gb, coord_t x, coord_t y, coord_t width, coord_t height);

/**
 * @brief	Get the state of a checkbox
 * @return	TRUE if the checkbox is currently checked
 *
 * @param[in] gh	The window handle (must be a checkbox window)
 *
 * @api
 */
bool_t gwinIsCheckboxChecked(GHandle gh);

/**
 * @brief	Set the colors used to draw the checkbox
 *
 * @param[in] gh		The window handle (must be a checkbox window)
 * @param[in] pColors	The colors to use
 *
 * @api
 */
void gwinCheckboxSetColors(GHandle gh, GCheckboxColors *pColors);

/**
 * @brief	Some custom checkbox drawing routines
 * @details	These function may be passed to @p gwinSetCustomDraw() to get different checkbox drawing styles
 *
 * @param[in] gw			The widget (which must be a checkbox)
 * @param[in] param			A parameter passed in from the user
 *
 * @note				In your custom checkbox drawing function you may optionally call this
 * 						standard functions and then draw your extra details on top.
 * @note				The standard functions below ignore the param parameter.
 * @note				These custom drawing routines don't have to worry about setting clipping as the framework
 * 						sets clipping to the object window prior to calling these routines.
 *
 * @api
 * @{
 */
void gwinCheckboxDraw_CheckOnLeft(GWidgetObject *gw, void *param);
void gwinCheckboxDraw_CheckOnRight(GWidgetObject *gw, void *param);
/* @} */

#endif /* _GWIN_CHECKBOX_H */
/** @} */

