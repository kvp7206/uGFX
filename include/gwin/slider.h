/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    include/gwin/slider.h
 * @brief   GWIN Graphic window subsystem header file.
 *
 * @defgroup Slider Slider
 * @ingroup GWIN
 *
 * @details		Create sliders with different styles
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_SLIDER must be set to TRUE in your gfxconf.h
 * @{
 */

#ifndef _GWIN_SLIDER_H
#define _GWIN_SLIDER_H

#if GWIN_NEED_SLIDER || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_SLIDER				0x0004
#define GEVENT_GWIN_SLIDER		(GEVENT_GWIN_FIRST+1)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct GEventGWinSlider_t {
	GEventType		type;				// The type of this event (GEVENT_GWIN_BUTTON)
	GHandle			slider;				// The slider that is returning results
	int				position;
} GEventGWinSlider;

// There are currently no GEventGWinSlider listening flags - use 0

typedef struct GSliderDrawStyle_t {
	color_t				color_edge;
	color_t				color_thumb;
	color_t				color_active;
	color_t				color_inactive;
} GSliderDrawStyle;

typedef void (*GSliderDrawFunction)(GHandle gh, bool_t isVertical, coord_t thumbpos, const GSliderDrawStyle *pstyle, void *param);

// A slider window
typedef struct GSliderObject_t {
	GWindowObject		gwin;

	GSliderDrawStyle	style;
	bool_t				tracking;
	int					min;
	int					max;
	int					pos;
	GSliderDrawFunction	fn;
	void				*param;
	GListener			listener;
} GSliderObject;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Create a slider window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gb		The GSliderObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @note				The drawing color gets set to White and the background drawing color to Black.
 * @note				The dimensions and position may be changed to fit on the real screen.
 * @note				The slider is not automatically drawn. Call gwinSliderDraw() after changing the slider style.
 * @note				Sets the slider range from 0 to 100 with an initial position of 0
 *
 * @api
 */	
GHandle gwinCreateSlider(GSliderObject *gb, coord_t x, coord_t y, coord_t width, coord_t height);

/**
 * @brief   Set the slider range.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] min		The minimum value
 * @param[in] max		The maximum value
 * @note				Sets the position to the minimum value.
 * @note				The slider is not automatically drawn. Call gwinSliderDraw() after changing the range.
 *
 * @api
 */
void gwinSetSliderRange(GHandle gh, int min, int max);

/**
 * @brief   Set the slider position.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] pos		The new position
 * @note				If the new position is outside the slider range then the position
 * 						is set to the closest end of the range.
 * @note				The slider is not automatically drawn. Call gwinSliderDraw() after changing the position.
 *
 * @api
 */
void gwinSetSliderPosition(GHandle gh, int pos);

/**
 * @brief   Set the style of a slider.
 * @details	The slider style is defined by its colours.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] pStyle	The styling for the slider.
 *
 * @note				The slider is not automatically redrawn. Call gwinSliderDraw() after changing the slider style
 * @note				The slider style is copied into the internal slider structure - there is no need to
 * 						maintain a static style structure.
 *
 * @api
 */
void gwinSetSliderStyle(GHandle gh, const GSliderDrawStyle *pStyle);

/**
 * @brief   Redraw the slider.
 *
 * @param[in] gh		The window handle (must be a slider window)
 *
 * @api
 */
void gwinSliderDraw(GHandle gh);

/**
 * @brief	Enable or disable a button
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] enabled	Enable or disable the slider
 *
 * @api
 */
void gwinSliderSetEnabled(GHandle gh, bool_t enabled);

/**
 * @brief   Set the callback routine to perform a custom slider drawing.
 *
 * @param[in] gh		The window handle (must be a slider window)
 * @param[in] fn		The function to use to draw the slider
 * @param[in] param		A parameter to pass to the slider drawing function
 *
 * @api
 */
void gwinSetSliderCustom(GHandle gh, GSliderDrawFunction fn, void *param);

/**
 * @brief	Enable a slider
 *
 * @api
 */
#define gwinEnableSlider(gh)			gwinSliderSetEnabled( ((GSliderObject *)(gh)), TRUE)

/**
 * @brief	Disable a slider
 *
 * @api
 */
#define gwinDisableSlider(gh)			gwinSliderSetEnabled( ((GSliderObject *)(gh)), FALSE)

/**
 * @brief   Get the current slider position.
 * @return	The slider position
 *
 * @param[in] gh		The window handle (must be a slider window)
 *
 * @note	The use of a listener to get the slider position is recommended if you
 * 			want continuous updates on the slider position.
 *
 * @api
 */
#define gwinGetSliderPosition(gh)		(((GSliderObject *)(gh))->pos)

/**
 * @brief Get the source handle of a slider
 * @details Get the source handle of a slider so the application can listen for events
 *
 * @param[in] gh	The window handle
 *
 * @api
 */
#define gwinGetSliderSource(gh)		((GSourceHandle)(gh))

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	/**
	 * @brief	Attach a mouse source
	 * @details	Attach a mouse to a slider
	 *
	 * @param[in] gh		The slider handle
	 * @param[in] instance	The mouse instance
	 *
	 * @api
	 */
	bool_t gwinAttachSliderMouse(GHandle gh, uint16_t instance);
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	/**
	 * @brief	Attach a dial source
	 * @details	Attach a dial to a slider
	 *
	 * @param[in] gh		The dial handle
	 * @param[in] instance	The dial instance
	 *
	 * @api
	 */
	bool_t gwinAttachSliderDial(GHandle gh, uint16_t instance);
#endif

/**
 * @brief	Standard slider drawing routines
 * @details	This routine is called to draw the standard slider.
 *
 * @param[in] gh			The slider handle
 * @param[in] isVertical	The slider is vertically oriented instead of horizontal
 * @param[in] thumbpos		The position of the slider (0..cx-1 or cy-1..0)
 * @param[in] pstyle		The current drawing style
 * @param[in] param			A parameter passed in from the user
 *
 * @note				In your custom slider drawing function you may optionally call this
 * 						standard functions and then draw your extra details on top.
 * @note				The standard functions below ignore the param parameter. It is there
 * 						only to ensure the functions match the GSliderDrawFunction type.
 * @note				When called by a slider the framework ensure that it is
 * 						a slider object and sets up clipping to the slider object window. These
 * 						drawing routines then don't have to worry about explicitly doing that.
 *
 * @api
 */
void gwinSliderDraw_Std(GHandle gh, bool_t isVertical, coord_t thumbpos, const GSliderDrawStyle *pstyle, void *param);

#ifdef __cplusplus
}
#endif

#endif /* GWIN_NEED_SLIDER */

#endif /* _GWIN_SLIDER_H */
/** @} */

