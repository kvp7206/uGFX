/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_WIDGET

#include <string.h>

#include "gwin/class_gwin.h"

/* Our listener for events for widgets */
static GListener	gl;

/* We use these everywhere in this file */
#define gw		((GWidgetObject *)gh)
#define wvmt	((gwidgetVMT *)gh->vmt)

/* Process an event */
static void gwidgetEvent(void *param, GEvent *pe) {
	#define gh		QItem2GWindow(qi)
	#define pme		((GEventMouse *)pe)
	#define pte		((GEventToggle *)pe)
	#define pde		((GEventDial *)pe)

	const gfxQueueASyncItem *	qi;
	#if GFX_USE_GINPUT && (GINPUT_NEED_TOGGLE || GINPUT_NEED_DIAL)
		uint16_t				role;
	#endif
	(void)						param;

	// Process various events
	switch (pe->type) {

	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	case GEVENT_MOUSE:
	case GEVENT_TOUCH:
		// Cycle through all windows
		for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {

			// check if it a widget that is enabled and visible
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE)) != (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE))
				continue;

			// Are we captured?
			if ((gw->g.flags & GWIN_FLG_MOUSECAPTURE)) {
				if ((pme->last_buttons & ~pme->current_buttons & GINPUT_MOUSE_BTN_LEFT)) {
					gw->g.flags &= ~GWIN_FLG_MOUSECAPTURE;
					if (wvmt->MouseUp)
						wvmt->MouseUp(gw, pme->x - gw->g.x, pme->y - gw->g.y);
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
		}
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	case GEVENT_TOGGLE:
		// Cycle through all windows
		for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {

			// check if it a widget that is enabled and visible
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE)) != (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE))
				continue;

			for(role = 0; role < wvmt->toggleroles; role++) {
				if (wvmt->ToggleGet(gw, role) == pte->instance) {
					if (pte->on) {
						if (wvmt->ToggleOn)
							wvmt->ToggleOn(gw, role);
					} else {
						if (wvmt->ToggleOff)
							wvmt->ToggleOff(gw, role);
					}
				}
			}
		}
		break;
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	case GEVENT_DIAL:
		// Cycle through all windows
		for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {

			// check if it a widget that is enabled and visible
			if ((gh->flags & (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE)) != (GWIN_FLG_WIDGET|GWIN_FLG_ENABLED|GWIN_FLG_VISIBLE))
				continue;

			for(role = 0; role < wvmt->dialroles; role++) {
				if (wvmt->DialGet(gw, role) == pte->instance) {
					if (wvmt->DialMove)
						wvmt->DialMove(gw, role, pde->value, pde->maxvalue);
				}
			}
		}
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

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	static GHandle FindToggleUser(uint16_t instance) {
		#define gh					QItem2GWindow(qi)
		const gfxQueueASyncItem *	qi;
		uint16_t					role;

		for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {
			if (!(gh->flags & GWIN_FLG_WIDGET))		// check if it a widget
				continue;

			for(role = 0; role < wvmt->toggleroles; role++) {
				if (wvmt->ToggleGet(gw, role) == instance)
					return gh;
			}
		}
		return 0;
		#undef gh
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	static GHandle FindDialUser(uint16_t instance) {
		#define gh					QItem2GWindow(qi)
		const gfxQueueASyncItem *	qi;
		uint16_t					role;

		for(qi = gfxQueueASyncPeek(&_GWINList); qi; qi = gfxQueueASyncNext(qi)) {
			if (!(gh->flags & GWIN_FLG_WIDGET))		// check if it a widget
				continue;

			for(role = 0; role < wvmt->dialroles; role++) {
				if (wvmt->DialGet(gw, role) == instance)
					return gh;
			}
		}
		return 0;
		#undef gh
	}
#endif

void _gwidgetInit(void) {
	geventListenerInit(&gl);
	geventRegisterCallback(&gl, gwidgetEvent, 0);
}

GHandle _gwidgetCreate(GWidgetObject *pgw, GWidgetInit *pInit, const gwidgetVMT *vmt) {
	if (!(pgw = (GWidgetObject *)_gwindowCreate(&pgw->g, &pInit->g, &vmt->g, GWIN_FLG_WIDGET|GWIN_FLG_ENABLED)))
		return 0;

	pgw->txt = pInit->text ? pInit->text : "";
	pgw->fnDraw = vmt->DefaultDraw;
	pgw->fnParam = 0;

	return 	&pgw->g;
}

void _gwidgetDestroy(GHandle gh) {
	#if GFX_USE_GINPUT && (GINPUT_NEED_TOGGLE || GINPUT_NEED_DIAL)
		uint16_t	role, instance;
	#endif

	// Deallocate the text (if necessary)
	if ((gh->flags & GWIN_FLG_ALLOCTXT)) {
		gh->flags &= ~GWIN_FLG_ALLOCTXT;
		gfxFree((void *)gw->txt);
	}

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
		// Detach any toggles from this object
		for(role = 0; role < wvmt->toggleroles; role++) {
			instance = wvmt->ToggleGet(gw, role);
			if (instance != GWIDGET_NO_INSTANCE) {
				wvmt->ToggleAssign(gw, role, GWIDGET_NO_INSTANCE);
				if (!FindToggleUser(instance))
					geventDetachSource(&gl, ginputGetToggle(instance));
			}
		}
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
		// Detach any dials from this object
		for(role = 0; role < wvmt->dialroles; role++) {
			instance = wvmt->DialGet(gw, role);
			if (instance != GWIDGET_NO_INSTANCE) {
				wvmt->DialAssign(gw, role, GWIDGET_NO_INSTANCE);
				if (!FindDialUser(instance))
					geventDetachSource(&gl, ginputGetDial(instance));
			}
		}
	#endif

	// Remove any listeners on this object.
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

bool_t gwinAttachListener(GListener *pl) {
	return geventAttachSource(pl, GWIDGET_SOURCE, 0);
}

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	bool_t gwinAttachMouse(uint16_t instance) {
		GSourceHandle	gsh;

		if (!(gsh = ginputGetMouse(instance)))
			return FALSE;

		return geventAttachSource(&gl, gsh, GLISTEN_MOUSEMETA|GLISTEN_MOUSEDOWNMOVES);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	bool_t gwinAttachToggle(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;
		uint16_t		oi;

		// Is this a widget
		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		// Is the role valid
		if (role >= wvmt->toggleroles)
			return FALSE;

		// Is this a valid device
		if (!(gsh = ginputGetToggle(instance)))
			return FALSE;

		// Is this already done?
		oi = wvmt->ToggleGet(gw, role);
		if (instance == oi)
			return TRUE;

		// Remove the old instance
		if (oi != GWIDGET_NO_INSTANCE) {
			wvmt->ToggleAssign(gw, role, GWIDGET_NO_INSTANCE);
			if (!FindToggleUser(oi))
				geventDetachSource(&gl, ginputGetToggle(oi));
		}

		// Assign the new
		wvmt->ToggleAssign(gw, role, instance);
		return geventAttachSource(&gl, gsh, GLISTEN_TOGGLE_ON|GLISTEN_TOGGLE_OFF);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_DIAL
	bool_t gwinAttachDial(GHandle gh, uint16_t role, uint16_t instance) {
		GSourceHandle	gsh;
		uint16_t		oi;

		if (!(gh->flags & GWIN_FLG_WIDGET))
			return FALSE;

		// Is the role valid
		if (role >= wvmt->dialroles)
			return FALSE;

		// Is this a valid device
		if (!(gsh = ginputGetDial(instance)))
			return FALSE;

		// Is this already done?
		oi = wvmt->DialGet(gw, role);
		if (instance == oi)
			return TRUE;

		// Remove the old instance
		if (oi != GWIDGET_NO_INSTANCE) {
			wvmt->DialAssign(gw, role, GWIDGET_NO_INSTANCE);
			if (!FindDialUser(oi))
				geventDetachSource(&gl, ginputGetDial(oi));
		}

		// Assign the new
		wvmt->DialAssign(gw, role, instance);
		return geventAttachSource(&gl, gsh, 0);
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_WIDGET */
/** @} */
