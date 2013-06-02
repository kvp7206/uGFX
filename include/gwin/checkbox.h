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

#if GWIN_NEED_CHECKBOX || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_CHECKBOX				0x0005
#define GEVENT_GWIN_CHECKBOX		(GEVENT_GWIN_FIRST+2)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct GEventGWinCheckbox_t {
	GEventType		type;				// The type of this event (GEVENT_GWIN_CHECKBOX)
	GHandle			checkbox;			// The checkbox that has been depressed (actually triggered on release)
} GEventGWinCheckbox;

typedef enum GCheckboxShape_e {
	GCHBX_NORMAL, GCKBX_CUSTOM
} GCheckboxShape;

typedef enum GCheckboxState_e {
	GCHBX_UNCHECKED, GCHBX_CHECKED
} GCheckboxState;

typedef struct GCheckboxColor_t {
	color_t	border;
	color_t checked;
	color_t bg;
} GCheckboxColor;

/* custom rendering interface */
typedef void (*GCheckboxDrawFunction)(GHandle gh, bool_t enabled, bool_t state, void* param);

/* A Checkbox window */
typedef struct GCheckboxObject_t {
	GWindowObject			gwin;
	GListener				listener;

	GCheckboxDrawFunction	fn;
	GCheckboxColor			*colors;
	bool_t					state;
	void					*param;
} GCheckboxObject;

/**
 * @brief	Create a checkbox window.
 *
 * @param[in] gb		The GCheckboxObject structure to initialise. If this is NULL, the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 *
 * @note				The checkbox is not automatically drawn. Call gwinCheckboxDraw() after changing the checkbox style.
 *
 * @return				NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @api
 */
GHandle gwinCheckboxCreate(GCheckboxObject *gb, coord_t x, coord_t y, coord_t width, coord_t height);

/**
 * @brief	Redraw a checkbox
 *
 * @param[in] gh		The window handle (must be a checkbox window)
 *
 * @api
 */
void gwinCheckboxDraw(GHandle gh);

/**
 * @brief	Enable or disable a button
 *
 * @param[in] gh		The window handle (must be a checkbox window)
 * @param[in] enabled	Enable or disable the button
 *
 * @api
 */
void gwinCheckboxSetEnabled(GHandle gh, bool_t enabled);

/**
 * @brief Enable a checkbox
 *
 * @api
 */
#define gwinCheckboxEnable(gh)		gwinCheckboxSetEnabled( ((GCheckboxObject *)(gh)), TRUE)

/**
 * @brief Disable a checkbox
 *
 * @api
*/
#define gwinCheckboxDisable(gh)		gwinCheckboxSetEnabled( ((GCheckboxObject *)(gh)), FALSE)

/**
 * @brief	Get the state of a checkbox
 *
 * @param[in] gh	The window handle (must be a checkbox window)
 *
 * @return	The state of the checkbox (GCHBX_CHECKED or GCHBX_UNCHECKED)
 *
 * @api
 */
#define gwinCheckboxGetState(gh)	(((GCheckboxObject *)(gh))->state)

/**
 * @brief 	Get the source handle of a checkbox
 * @details	Get the source handle of a checkbox so the application can listen for events
 *
 * @param[in] gh	The window handle (must be a checkbox window)
 *
 * @api
 */
#define gwinCheckboxGetSource(gh)     ((GSourceHandle)(gh))

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	/**
	 * @brief	Attach a mouse to a checkbox
	 *
	 * @param[in] gh		The checkbox handle
	 * @param[in] instance	The mouse instance
	 *
	 * @api
	 */
	bool_t gwinCheckboxAttachMouse(GHandle gh, uint16_t instance);
#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

#endif /* _GWIN_NEED_CHECKBOX */

#endif /* _GWIN_CHECKBOX_H */
/** @} */

