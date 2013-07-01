/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file    src/gwin/checkbox.c
 * @brief   GWIN sub-system button code.
 *
 * @defgroup Checkbox Checkbox
 * @ingroup GWIN
 *
 * @{
 */

#include "gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_CHECKBOX) || defined(__DOXYGEN__)

#include "gwin/class_gwin.h"

// Our checked state
#define GCHECKBOX_FLG_CHECKED		(GWIN_FIRST_CONTROL_FLAG<<0)

// Prototypes for button VMT functions
static void MouseDown(GWidgetObject *gw, coord_t x, coord_t y);
static void ToggleOn(GWidgetObject *gw, uint16_t role);
static void ToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance);
static uint16_t ToggleGet(GWidgetObject *gw, uint16_t role);

// The button VMT table
static const gwidgetVMT checkboxVMT = {
	{
		"Checkbox",				// The classname
		sizeof(GCheckboxObject),// The object size
		_gwidgetDestroy,		// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinCheckboxDraw_CheckOnLeft,	// The default drawing routine
	{
		MouseDown,				// Process mouse down events
		0,						// Process mouse up events (NOT USED)
		0,						// Process mouse move events (NOT USED)
	},
	{
		1,						// 1 toggle role
		ToggleAssign,			// Assign Toggles
		ToggleGet,				// Get Toggles
		0,						// Process toggle off events (NOT USED)
		ToggleOn,				// Process toggle on events
	},
	{
		0,						// No dial roles
		0,						// Assign Dials (NOT USED)
		0,						// Get Dials (NOT USED)
		0,						// Process dial move events (NOT USED)
	}
};

static const GCheckboxColors defaultColors = {
	Black,	// border
	Grey,	// selected
	White,	// background
	Black,	// text
};

// Send the checkbox event
static void SendCheckboxEvent(GWidgetObject *gw) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pce			((GEventGWinCheckbox *)pe)

	// Trigger a GWIN Checkbox Event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pce->type = GEVENT_GWIN_CHECKBOX;
		pce->checkbox = &gw->g;
		pce->isChecked = (gw->g.flags & GCHECKBOX_FLG_CHECKED) ? TRUE : FALSE;
		geventSendEvent(psl);
	}

	#undef pce
}

// A mouse down has occurred over the checkbox
static void MouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
	(void) x; (void) y;
	gw->g.flags ^= GCHECKBOX_FLG_CHECKED;
	_gwidgetRedraw((GHandle)gw);
	SendCheckboxEvent(gw);
}

// A toggle on has occurred
static void ToggleOn(GWidgetObject *gw, uint16_t role) {
	(void) role;
	gw->g.flags ^= GCHECKBOX_FLG_CHECKED;
	_gwidgetRedraw((GHandle)gw);
	SendCheckboxEvent(gw);
}

static void ToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
	(void) role;
	((GCheckboxObject *)gw)->toggle = instance;
}

static uint16_t ToggleGet(GWidgetObject *gw, uint16_t role) {
	(void) role;
	return ((GCheckboxObject *)gw)->toggle;
}

GHandle gwinCreateCheckbox(GCheckboxObject *gb, const GWidgetInit *pInit) {
	if (!(gb = (GCheckboxObject *)_gwidgetCreate(&gb->w, pInit, &checkboxVMT)))
		return 0;

	gb->toggle = (uint16_t) -1;
	gb->c = defaultColors;			// assign the default colors
	gwinSetVisible((GHandle)gb, pInit->g.show);
	return (GHandle)gb;
}

bool_t gwinIsCheckboxChecked(GHandle gh) {
	if (gh->vmt != (gwinVMT *)&checkboxVMT)
		return FALSE;

	return (gh->flags & GCHECKBOX_FLG_CHECKED) ? TRUE : FALSE;
}

void gwinCheckboxSetColors(GHandle gh, GCheckboxColors *pColors) {
	if (gh->vmt != (gwinVMT *)&checkboxVMT)
		return;

	((GCheckboxObject *)gh)->c = *pColors;
}

void gwinCheckboxDraw_CheckOnLeft(GWidgetObject *gw, void *param) {
	#define gcw		((GCheckboxObject *)gw)
	coord_t	ld, df;
	(void) param;

	if (gw->g.vmt != (gwinVMT *)&checkboxVMT)
		return;

	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;
	gdispFillArea(gw->g.x+1, gw->g.y+1, ld, ld-2, gcw->c.color_bg);
	gdispDrawBox(gw->g.x, gw->g.y, ld, ld, gcw->c.color_border);

	df = ld < 4 ? 1 : 2;
	if (gw->g.flags & GCHECKBOX_FLG_CHECKED)
		gdispFillArea(gw->g.x+df, gw->g.y+df, ld-2*df, ld-2*df, gcw->c.color_checked);

	gdispFillStringBox(gw->g.x+ld+1, gw->g.y, gw->g.width-ld-1, gw->g.height, gw->txt, gw->g.font, gcw->c.color_txt, gcw->c.color_bg, justifyLeft);
	#undef gcw
}

void gwinCheckboxDraw_CheckOnRight(GWidgetObject *gw, void *param) {
	#define gcw		((GCheckboxObject *)gw)
	coord_t	ep, ld, df;
	(void) param;

	if (gw->g.vmt != (gwinVMT *)&checkboxVMT)
		return;

	ld = gw->g.width < gw->g.height ? gw->g.width : gw->g.height;
	ep = gw->g.width-ld-1;
	gdispFillArea(gw->g.x+ep-1, gw->g.y+1, ld, ld-2, gcw->c.color_bg);
	gdispDrawBox(gw->g.x+ep, gw->g.y, ld, ld, gcw->c.color_border);

	df = ld < 4 ? 1 : 2;
	if (gw->g.flags & GCHECKBOX_FLG_CHECKED)
		gdispFillArea(gw->g.x+ep+df, gw->g.y+df, ld-2*df, ld-2*df, gcw->c.color_checked);

	gdispFillStringBox(gw->g.x, gw->g.y, ep, gw->g.height, gw->txt, gw->g.font, gcw->c.color_txt, gcw->c.color_bg, justifyRight);
	#undef gcw
}

#endif /* (GFX_USE_GWIN && GWIN_NEED_CHECKBOX) */
/** @} */

