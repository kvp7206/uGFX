/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gwin/list.h
 * @brief	GWIN list widget header file.
 *
 * @defgroup List List
 * @ingroup GWIN
 *
 * @{
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_LIST

#include "gwin/class_gwin.h"

static void gwinListDefaultDraw(GWidgetObject* gw, void* param) {

}

static const gwidgetVMT listVMT = {
	{
		"List",					// The class name
		sizeof(GListObject),	// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinListDefaultDraw,		// default drawing routine
	#if GWINPUT_NEED_MOUSE
		{
			0,
			0,
			0,
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			0,
			0,
			0,
			0,
			0,
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,
			0,
			0,
			0,
		},
	#endif
};

GHandle gwinListCreate(GListObject* widget, GWidgetInit* pInit) {
	if (!(widget = (GListObject *)_gwidgetCreate(&widget->w, pInit, &listVMT)))
		return 0;

	gwinSetVisible(&widget->w.g, pInit->g.show);

	return (GHandle)widget;
}

#endif // GFX_USE_GWIN && GWIN_NEED_LIST
/** @} */

