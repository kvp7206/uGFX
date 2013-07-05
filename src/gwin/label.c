/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file	include/gwin/label.h
 * @brief	GWIN label widget header file.
 *
 * @defgroup Label Label
 * @ingroup GWIN
 *
 * @{
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_LABEL

#include "gwin/class_gwin.h"

#define GLABEL_FLG_WAUTO		(GWIN_FIRST_CONTROL_FLAG<<0)
#define GLABEL_FLG_HAUTO		(GWIN_FIRST_CONTROL_FLAG<<1)

// Simple: single line with no wrapping
static coord_t getwidth(const char *txt, font_t font, coord_t maxwidth) {
	(void) maxwidth;
	return gdispGetStringWidth(txt, font)+2;		// Allow one pixel of padding on each side
}

// Simple: single line with no wrapping
static coord_t getheight(const char *txt, font_t font, coord_t maxwidth) {
	(void) txt;
	(void) maxwidth;

	return gdispGetFontMetric(font, fontHeight);
}

static void gwinLabelDefaultDraw(GWidgetObject *gw, void *param) {
	(void) param;
	coord_t	w, h;

	w = (gw->g.flags & GLABEL_FLG_WAUTO) ? getwidth(gw->txt, gw->g.font, gdispGetWidth() - gw->g.x) : gw->g.width;
	h = (gw->g.flags & GLABEL_FLG_HAUTO) ? getheight(gw->txt, gw->g.font, gdispGetWidth() - gw->g.x) : gw->g.height;

	if (gw->g.width != w || gw->g.height != h) {
		gwinResize(&gw->g, w, h);
		return;
	}

	gdispFillStringBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->txt, gw->g.font, gw->g.color, gw->g.bgcolor, justifyLeft);
}

static const gwidgetVMT labelVMT = {
	{
		"Label",				// The class name
		sizeof(GLabelWidget),	// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw, 		// The redraw routine
		0,						// The after-clear routine
	},
	gwinLabelDefaultDraw,		// default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			0,						// Process mose down events (NOT USED)
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			0,						// No toggle role
			0,						// Assign Toggles (NOT USED)
			0,						// Get Toggles (NOT USED)
			0,						// Process toggle off event (NOT USED)
			0,						// Process toggle on event (NOT USED)
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials (NOT USED)
			0, 						// Get Dials (NOT USED)
			0,						// Procees dial move events (NOT USED)
		},
	#endif
};

GHandle gwinLabelCreate(GLabelWidget *widget, GWidgetInit *pInit) {
	uint16_t flags = 0;

	// auto assign width
	if (pInit->g.width <= 0) {
		flags |= GLABEL_FLG_WAUTO;
		pInit->g.width = getwidth(pInit->text, gwinGetDefaultFont(), gdispGetWidth() - pInit->g.x);
	}
 
	// auto assign height
	if (pInit->g.height <= 0) {
		flags |= GLABEL_FLG_HAUTO;
		pInit->g.height = getheight(pInit->text, gwinGetDefaultFont(), gdispGetWidth() - pInit->g.x);
	}

	if (!(widget = (GLabelWidget *)_gwidgetCreate(&widget->w, pInit, &labelVMT)))
		return 0;

	widget->w.g.flags |= flags;

	gwinSetVisible(&widget->w.g, pInit->g.show);
	return (GHandle)widget;
}

#endif // GFX_USE_GWIN && GFX_NEED_LABEL

