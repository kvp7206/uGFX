/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/class_gwin.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Internal Internal
 * @ingroup GWIN
 *
 * @note	These definitions are normally not used by an application program. They are useful
 * 			only if you want to create your own custom GWIN window or widget.
 * @note	To access these definitions you must include "gwin/class_gwin.h" in your source file.
 *
 * @{
 */
#ifndef _CLASS_GWIN_H
#define _CLASS_GWIN_H

#if GFX_USE_GWIN || defined(__DOXYGEN__)

/**
 * @brief	The Virtual Method Table for a GWIN window
 * @{
 */
typedef struct gwinVMT {
	const char *classname;							// @< The GWIN classname
	void (*Destroy)(GWindowObject *gh);				// @< The GWIN Destroy function (optional)
	void (*AfterClear)(GWindowObject *gh);			// @< The GWIN After-Clear function (optional)
} gwinVMT;
/* @} */

/**
 * @brief	The Virtual Method Table for a widget
 * @note	A widget must have a destroy function. Either use @p _gwidgetDestroy() or use your own function
 * 			which internally calls @p _gwidgetDestroy().
 * @note	If no MouseDown(), MouseUp() or MouseMove() function is provided, the widget will not accept being attached to a mouse input source.
 * @note	If no ToggleOn() or ToggleOff() function is provided, the widget will not accept being attached to a toggle input source.
 * @note	If no DialMove() function is provided, the widget will not accept being attached to a dial input source.
 * @note	AssignToggle() and AssignDial() enable a widget to handle more than one toggle/dial device attached to the widget.
 * 			For example, a slider might accept two toggles, one for slider-down and one for slider-up.
 * 			The function enables the widget to record that a particular device instance performs each particular role.
 * 			(eg toggle0 = slider-down, toggle1 = slider-up).
 * @{
 */
typedef struct gwidgetVMT {
	struct gwinVMT			g;														// @< This is still a GWIN
	void (*DefaultDraw)		(GWidgetObject *gw, void *param);						// @< The default drawing routine (mandatory)
	void (*MouseDown)		(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse down events (optional)
	void (*MouseUp)			(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse up events (optional)
	void (*MouseMove)		(GWidgetObject *gw, coord_t x, coord_t y);				// @< Process mouse move events (optional)
	void (*ToggleOff)		(GWidgetObject *gw, uint16_t instance);					// @< Process toggle off events (optional)
	void (*ToggleOn)		(GWidgetObject *gw, uint16_t instance);					// @< Process toggle on events (optional)
	void (*DialMove)		(GWidgetObject *gw, uint16_t instance, uint16_t value);	// @< Process dial move events (optional)
	void (*AllEvents)		(GWidgetObject *gw, GEvent *pe);						// @< Process all events (optional)
	bool_t (*AssignToggle)	(GWidgetObject *gw, uint16_t role, uint16_t instance);	// @< Test the role and save the toggle instance handle (optional)
	bool_t (*AssignDial)	(GWidgetObject *gw, uint16_t role, uint16_t instance);	// @< Test the role and save the dial instance handle (optional)
} gwidgetVMT;
/* @} */

/**
 * @brief	The predefined flags for a GWIN and a Widget
 * @{
 */
#define GWIN_FLG_DYNAMIC				0x0001			// @< The GWIN structure is allocated
#define GWIN_FLG_WIDGET					0x0002			// @< This is a widget
#define GWIN_FLG_ENABLED				0x0002			// @< The widget is enabled
#define GWIN_FLG_ALLOCTXT				0x0008			// @< The widget text is allocated
#define GWIN_FLG_MOUSECAPTURE			0x0010			// @< The widget has captured the mouse
#define GWIN_FIRST_CONTROL_FLAG			0x0100			// @< Free for GWINs and Widgets to use
/* @} */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	Initialise (and allocate if necessary) the base GWIN object
 *
 * @param[in]	pgw		The GWindowObject structure. If NULL one is allocated from the heap
 * @param[in]	x, y	The top left corner of the GWIN relative to the screen
 * @param[in]	w, h	The width and height of the GWIN window
 * @param[in]	size	The size of the GWIN object to allocate
 * @param[in]	vmt		The virtual method table for the GWIN object
 *
 * @notapi
 */
GHandle _gwinInit(GWindowObject *pgw, coord_t x, coord_t y, coord_t w, coord_t h, size_t size, const gwinVMT *vmt);

/**
 * @brief	Initialise (and allocate if necessary) the base Widget object
 *
 * @param[in]	pgw		The GWidgetObject structure. If NULL one is allocated from the heap
 * @param[in]	x, y	The top left corner of the Widget relative to the screen
 * @param[in]	w, h	The width and height of the Widget window
 * @param[in]	size	The size of the Widget object to allocate
 * @param[in]	vmt		The virtual method table for the Widget object
 *
 * @notapi
 */
GHandle _gwidgetInit(GWidgetObject *pgw, coord_t x, coord_t y, coord_t w, coord_t h, size_t size, const gwidgetVMT *vmt);

/**
 * @brief	Destroy the Widget object
 *
 * @param[in]	gw		The widget to destroy
 *
 * @notapi
 */
void _gwidgetDestroy(GHandle gh);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GWIN */

#endif /* _CLASS_GWIN_H */
/** @} */
