/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gwin/list.h
 * @brief	GWIN list widget header file
 *
 * @defgroup List List
 * @ingroup GWIN
 *
 * @details		GWIN allows it to create a list widget.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_LIST must be set to TRUE in your gfxconf.h
 * @pre			The font you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_LIST_H
#define _GWIN_LIST_H

// This file is included within "gwin/gwin.h"

/**
 * @brief	The event type for a list event
 */
#define GEVENT_GWIN_LIST	(GEVENT_GWIN_FIRST+4)

/**
 * @brief	A list event
 */
typedef struct GEventGWinList {
	GEventType		type;			// The type of this event (GEVENT_GWIN_LIST)
	GHandle			list;			// THe list that has generated the event
} GEventGWinList;

// A list window
typedef struct GListObject {
	GWidgetObject	w;
} GListObject;

#ifdef __cplusplus
extern "C" {
#endif

GHandle gwinListCreate(GListObject *widget, GWidgetInit *pInit);

#ifdef __cplusplus
}
#endif

#endif // _GWIN_LIST_H
/** @} */

