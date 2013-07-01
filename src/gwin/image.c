/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file	src/gwin/image.c
 * @brief	GWIN sub-system image code.
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_IMAGE

#include "gwin/class_gwin.h"

static void _destroy(GWindowObject *gh) {
	(void)gh;

	return;
}

static void _redraw(GWindowObject *gh) {
	(void)gh;

	return;
}

static void _afterClear(GWindowObject *gh) {
	((GImageWidget *)gh)->cx = 0;
	((GImageWidget *)gh)->cy = 0;

	return;
}

static const gwinVMT imageVMT = {
	"Image",					// The class name
	sizeof(GImageWidget),		// The object size
	_destroy,					// The destroy routine
	_redraw,					// The redraw routine
	_afterClear,				// The after-clear routine
};

GHandle gwinImageCreate(GImageWidget *widget, GWindowInit *pInit) {
	if (!(widget = (GImageWidget *)_gwindowCreate(&widget->g, pInit, &imageVMT, 0)))
		return 0;

	widget->cx = 0;
	widget->cy = 0;
	gwinSetVisible((GHandle)widget, pInit->show);

	return (GHandle)widget;
}

void gwinImageDisplay(GImageWidget *widget, gdispImage *image) {

}

#endif // GFX_USE_GWIN && GWIN_NEED_IMAGE
/** @} */

