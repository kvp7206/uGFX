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

#define widget(gh)				((GLabelWidget*)gh) 
#define GLABEL_FLG_WAUTO		(GWIN_FIRST_CONTROL_FLAG<<0)
#define GLABEL_FLG_HAUTO		(GWIN_FIRST_CONTROL_FLAG<<1)

static void gwinLabelDefaultDraw(GHandle gh) {
	//	if( check if auto flag is set )
	//		if(	call current size != font size )
	//			gwinResize();

	gdispFillString(	widget(gh)->w.g.x,
						widget(gh)->w.g.y,
						widget(gh)->w.txt,
						widget(gh)->w.g.font,
						widget(gh)->w.g.color,
						widget(gh)->w.g.bgcolor
					);

	gdispFillArea( widget(gh)->w.g.x, widget(gh)->w.g.y, widget(gh)->w.g.width, widget(gh)->w.g.height, Green);

	printf("Text: %s\r\n", widget(gh)->w.txt);
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
	{
		0,						// Process mose down events (NOT USED)
		0,						// Process mouse up events (NOT USED)
		0,						// Process mouse move events (NOT USED)
	},
	{
		0,						// No toggle role
		0,						// Assign Toggles (NOT USED)
		0,						// Get Toggles (NOT USED)
		0,						// Process toggle off event (NOT USED)
		0,						// Process toggle on event (NOT USED)
	},
	{
		0,						// No dial roles
		0,						// Assign Dials (NOT USED)
		0, 						// Get Dials (NOT USED)
		0,						// Procees dial move events (NOT USED)
	}
};

GHandle gwinLabelCreate(GLabelWidget *widget, GWidgetInit *pInit) {
	uint16_t flags = 0;

	// auto assign width
	if (pInit->g.width <= 0) {
		flags |= GLABEL_FLG_WAUTO;
		pInit->g.width = gdispGetStringWidth(pInit->text, gwinGetDefaultFont()); 
	}
 
	// auto assign height
	if (pInit->g.height <= 0) {
		flags |= GLABEL_FLG_HAUTO;
		pInit->g.height = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight);
	}

	if (!(widget = (GLabelWidget *)_gwidgetCreate(&widget->w, pInit, &labelVMT)))
		return 0;

	gwinLabelDefaultDraw((GHandle)widget);
	widget->w.g.flags |= flags;

	return (GHandle)widget;
}

#endif // GFX_USE_GWIN && GFX_NEED_LABEL

