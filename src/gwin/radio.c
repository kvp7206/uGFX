/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gwin/radio.c
 * @brief   GWIN sub-system radio button code.
 *
 * @defgroup RadioButton RadioButton
 * @ingroup GWIN
 *
 * @{
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_RADIO

#include "gwin/class_gwin.h"

// Our pressed state
#define GRADIO_FLG_PRESSED		(GWIN_FIRST_CONTROL_FLAG<<0)

// Send the button event
static void SendRadioEvent(GWidgetObject *gw) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pbe			((GEventGWinRadio *)pe)

	// Trigger a GWIN Button Event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pbe->type = GEVENT_GWIN_RADIO;
		pbe->radio = (GHandle)gw;
		pbe->group = ((GRadioObject *)gw)->group;
		geventSendEvent(psl);
	}

	#undef pbe
}

#if GINPUT_NEED_MOUSE
	// A mouse down has occurred over the button
	static void MouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
		(void) x; (void) y;

		gwinRadioPress((GHandle)gw);
	}
#endif

#if GINPUT_NEED_TOGGLE
	// A toggle on has occurred
	static void ToggleOn(GWidgetObject *gw, uint16_t role) {
		(void) role;

		gwinRadioPress((GHandle)gw);
	}

	static void ToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		(void) role;
		((GRadioObject *)gw)->toggle = instance;
	}

	static uint16_t ToggleGet(GWidgetObject *gw, uint16_t role) {
		(void) role;
		return ((GRadioObject *)gw)->toggle;
	}
#endif

// The radio button VMT table
static const gwidgetVMT radioVMT = {
	{
		"Radio",				// The classname
		sizeof(GRadioObject),	// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinRadioDraw_Radio,		// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			MouseDown,				// Process mouse down events
			0,						// Process mouse up events (NOT USED)
			0,						// Process mouse move events (NOT USED)
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			1,						// 1 toggle role
			ToggleAssign,			// Assign Toggles
			ToggleGet,				// Get Toggles
			0,						// Process toggle off events (NOT USED)
			ToggleOn,				// Process toggle on events
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials (NOT USED)
			0,						// Get Dials (NOT USED)
			0,						// Process dial move events (NOT USED)
		},
	#endif
};

GHandle gwinRadioCreate(GRadioObject *gw, const GWidgetInit *pInit, uint16_t group) {
	if (!(gw = (GRadioObject *)_gwidgetCreate(&gw->w, pInit, &radioVMT)))
		return 0;

	#if GINPUT_NEED_TOGGLE
		gw->toggle = GWIDGET_NO_INSTANCE;
	#endif
	gw->group = group;
	gwinSetVisible((GHandle)gw, pInit->g.show);
	return (GHandle)gw;
}

void gwinRadioPress(GHandle gh) {
	GHandle		gx;

	if (gh->vmt != (gwinVMT *)&radioVMT || (gh->flags & GRADIO_FLG_PRESSED))
		return;

	if ((gx = gwinRadioGetActive(((GRadioObject *)gh)->group))) {
		gx->flags &= ~GRADIO_FLG_PRESSED;
		_gwidgetRedraw(gx);
	}
	gh->flags |= GRADIO_FLG_PRESSED;
	_gwidgetRedraw(gh);
	SendRadioEvent((GWidgetObject *)gh);
}

bool_t gwinRadioIsPressed(GHandle gh) {
	if (gh->vmt != (gwinVMT *)&radioVMT)
		return FALSE;

	return (gh->flags & GRADIO_FLG_PRESSED) ? TRUE : FALSE;
}

GHandle gwinRadioGetActive(uint16_t group) {
	const gfxQueueASyncItem *	qi;
	GHandle						gh;

	for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {
		gh = QItem2GWindow(qi);
		if (gh->vmt == (gwinVMT *)&radioVMT && ((GRadioObject *)gh)->group == group && (gh->flags & GRADIO_FLG_PRESSED))
			return gh;
	}
	return 0;
}

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

static const GColorSet *getDrawColors(GWidgetObject *gw) {
	if (!(gw->g.flags & GWIN_FLG_ENABLED))	return &gw->pstyle->disabled;
	if ((gw->g.flags & GRADIO_FLG_PRESSED))	return &gw->pstyle->pressed;
	return &gw->pstyle->enabled;
}

void gwinRadioDraw_Radio(GWidgetObject *gw, void *param) {
	#define gcw			((GRadioObject *)gw)
	coord_t				ld, df;
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&radioVMT) return;
	pcol = getDrawColors(gw);

	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;

	#if GDISP_NEED_CIRCLE
		df = (ld-1)/2;
		gdispFillArea(gw->g.x, gw->g.y, ld, ld, gw->pstyle->background);
		gdispDrawCircle(gw->g.x+df, gw->g.y+df, df, pcol->edge);

		if (gw->g.flags & GRADIO_FLG_PRESSED)
			gdispFillCircle(gw->g.x+df, gw->g.y+df, df <= 2 ? 1 : (df-2), pcol->fill);
	#else
		gdispFillArea(gw->g.x+1, gw->g.y+1, ld, ld-2, gw->pstyle->background);
		gdispDrawBox(gw->g.x, gw->g.y, ld, ld, pcol->edge);

		df = ld < 4 ? 1 : 2;
		if (gw->g.flags & GRADIO_FLG_PRESSED)
			gdispFillArea(gw->g.x+df, gw->g.y+df, ld-2*df, ld-2*df, pcol->fill);
	#endif

	gdispFillStringBox(gw->g.x+ld+1, gw->g.y, gw->g.width-ld-1, gw->g.height, gw->text, gw->g.font, pcol->text, gw->pstyle->background, justifyLeft);
	#undef gcw
}

void gwinRadioDraw_Button(GWidgetObject *gw, void *param) {
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&radioVMT) return;
	pcol = getDrawColors(gw);
	
	gdispFillStringBox(gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
	gdispDrawLine(gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
	gdispDrawLine(gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
}

void gwinRadioDraw_Tab(GWidgetObject *gw, void *param) {
	const GColorSet *	pcol;
	(void)				param;

	if (gw->g.vmt != (gwinVMT *)&radioVMT)	return;
	pcol = getDrawColors(gw);
	
	if ((gw->g.flags & GRADIO_FLG_PRESSED)) {
		gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->edge);
		gdispFillStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
	} else {
		gdispFillStringBox(gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->text, gw->g.font, pcol->text, pcol->fill, justifyCenter);
		gdispDrawLine(gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->edge);
		gdispDrawLine(gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->edge);
	}
}

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
/** @} */

