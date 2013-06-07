/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_WIDGET

#include <string.h>

#include "gwin/class_gwin.h"

/* We use these everywhere in this file */
#define gw		((GWidgetObject *)gh)
#define wvmt	((gwidgetVMT *)gh->vmt)

static void gwidgetCallback(void *param, GEvent *pe) {
	#define gh		((GWindowObject *)param)
	#define pme		((GEventMouse *)pe)
	#define pte		((GEventToggle *)pe)
	#define pde		((GEventDial *)pe)

	// check if widget is disabled
	if ((gw->g.flags & (GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE)) != (GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE))
		return;

	// Process via AllEvents() if it is defined
	if (wvmt->AllEvents)
		wvmt->AllEvents(gw, pe);

	// Process various events
	switch (pe->type) {

	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	case GEVENT_MOUSE:
	case GEVENT_TOUCH:
		// Are we captured?
		if ((gw->g.flags & GWIN_FLG_MOUSECAPTURE)) {
			if ((pme->last_buttons & ~pme->current_buttons & GINPUT_MOUSE_BTN_LEFT)) {
				gw->g.flags &= ~GWIN_FLG_MOUSECAPTURE;
				if (wvmt->MouseUp)
					wvmt->MouseUp(gw, pme->x - gw->g.x, pme->y - gw->g.y);
				return;
			} else if (wvmt->MouseMove)
				wvmt->MouseMove(gw, pme->x - gw->g.x, pme->y - gw->g.y);

		// We are not captured - look for mouse downs over the widget
		} else if ((~pme->last_buttons & pme->current_buttons & GINPUT_MOUSE_BTN_LEFT)
				&& pme->x >= gw->g.x && pme->x < gw->g.x + gw->g.width
				&& pme->y >= gw->g.y && pme->y < gw->g.y + gw->g.height) {
			gw->g.flags |= GWIN_FLG_MOUSECAPTURE;
			if (wvmt->MouseDown)
				wvmt->MouseDown(gw, pme->x - gw->g.x, pme->y - gw->g.y);
		}
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	case GEVENT_TOGGLE:
		if (pte->on) {
			if (wvmt->ToggleOn)
				wvmt->ToggleOn(gw, pte->instance);
		} else {
			if (wvmt->ToggleOff)
				wvmt->ToggleOff(gw, pte->instance);
		}
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	case GEVENT_DIAL:
		if (wvmt->DialMove)
			wvmt->DialMove(gw, pde->instance, pde->value);
		break;
	#endif

	default:
		break;
	}
	#undef gh
	#undef pme
	#undef pte
	#undef pde
}

GHandle _gwidgetInit(GWidgetObject *pgw, coord_t x, coord_t y, coord_t width, coord_t height, size_t size, const gwidgetVMT *vmt) {
	if (!(pgw = (GWidgetObject *)_gwindowInit((GWindowObject *)pgw, x, y, width, height, size, (const gwinVMT *)vmt, GWIN_FLG_WIDGET|GWIN_FLG_ENABLED)))
		return 0;

	pgw->txt = "";
	pgw->fnDraw = vmt->DefaultDraw;
	pgw->fnParam = 0;
	geventListenerInit(&pgw->listener);
	geventRegisterCallback(&pgw->listener, gwidgetCallback, pgw);

	return (GHandle)pgw;
}

void _gwidgetDestroy(GHandle gh) {
	// Deallocate the text (if necessary)
	if ((gh->flags & GWIN_FLG_ALLOCTXT)) {
		gh->flags &= ~GWIN_FLG_ALLOCTXT;
		gfxFree((void *)gw->txt);
	}
	// Untangle the listeners (both on us and to us).
	geventDetachSource(&gw->listener, 0);
	geventDetachSourceListeners((GSourceHandle)gh);
}

void _gwidgetRedraw(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_VISIBLE))
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif

	gw->fnDraw(gw, gw->fnParam);
}

void gwinSetEnabled(GHandle gh, bool_t enabled) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	if (enabled) {
		if (!(gh->flags & GWIN_FLG_ENABLED)) {
			gh->flags |= GWIN_FLG_ENABLED;
			_gwidgetRedraw(gh);
		}
	} else {
		if ((gh->flags & GWIN_FLG_ENABLED)) {
			gh->flags &= ~GWIN_FLG_ENABLED;
			_gwidgetRedraw(gh);
		}
	}
}

void gwinSetText(GHandle gh, const char *txt, bool_t useAlloc) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	// Dispose of the old string
	if ((gh->flags & GWIN_FLG_ALLOCTXT)) {
		gh->flags &= ~GWIN_FLG_ALLOCTXT;
		if (gw->txt) {
			gfxFree((void *)gw->txt);
			gw->txt = "";
		}
	}

	// Alloc the new text if required
	if (txt && !*txt) txt = 0;
	if (txt && useAlloc) {
		char *str;

		if ((str = (char *)gfxAlloc(strlen(txt)+1))) {
			gh->flags |= GWIN_FLG_ALLOCTXT;
			strcpy(str, txt);
		}
		txt = (const char *)str;
	}

	gw->txt = txt ? txt : "";
	_gwidgetRedraw(gh);
}

const char *gwinGetText(GHandle gh) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return 0;

	return gw->txt;
}

void gwinSetCustomDraw(GHandle gh, CustomWidgetDrawFunction fn, void *param) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return;

	gw->fnDraw = fn ? fn : wvmt->DefaultDraw;
	gw->fnParam = param;
	_gwidgetRedraw(gh);
}

bool_t gwinAttachListener(GHandle gh, GListener *pl, unsigned flags) {
	if (!(gh->flags & GWIN_FLG_WIDGET))
		return FALSE;

	return geventAttachSource(pl, (GSourceHandle)gh, flags);
}

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	bool_t gwinAttachMouse(GHandle gh, uint16_t instance) {
		GSourceHandle	gsh;
		unsigned		flags;

		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		if (!wvmt->MouseDown && !wvmt->MouseMove && !wvmt->MouseUp)
			return FALSE;

		if (!(gsh = ginputGetMouse(instance)))
			return FALSE;

		flags = wvmt->MouseMove ? (GLISTEN_MOUSEMETA|GLISTEN_MOUSEDOWNMOVES) : GLISTEN_MOUSEMETA;
		return geventAttachSource(&gw->listener, gsh, flags);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	bool_t gwinAttachToggle(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;
		unsigned		flags;

		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		flags = 0;
		if (wvmt->ToggleOff)	flags |= GLISTEN_TOGGLE_OFF;
		if (wvmt->ToggleOn)		flags |= GLISTEN_TOGGLE_ON;
		if (!flags)
			return FALSE;

		if (!(gsh = ginputGetToggle(instance)))
			return FALSE;

		if (wvmt->AssignToggle && !wvmt->AssignToggle(gw, role, instance))
			return FALSE;

		return geventAttachSource(&gw->listener, gsh, flags);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	bool_t gwinAttachDial(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;

		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		if (!wvmt->DialMove)
			return FALSE;

		if (!(gsh = ginputGetDial(instance)))
			return FALSE;

		if (wvmt->AssignDial && !wvmt->AssignDial(gw, role, instance))
			return FALSE;

		return geventAttachSource(&gw->listener, gsh, 0);
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_WIDGET */
/** @} */

