/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
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

static const GCheckboxDrawStyle GCheckboxDefaultStyleSelected = {
	Green
};

static const GCheckboxDrawStyle GCheckboxDefaultStyleUnselected = {
	Red
};

/* process an event callback */
static void gwinCheckboxCallback(void *param, GEvent *pe) {
	GSourceListener *psl;
	#define gh		((GHandle)param)
	#define gbw		((GCheckboxObject *)param)
	#define gsh		((GSourceHandle)param)
	#define pme		((GEventMouse *)pe)
	#define pte		((GEventTouch *)pe)
	#define pxe		((GEventToggle *)pe)
	#define pbe		((GEventGWinCheckbox *)pe)

	/* check if checkbox is disabled */
	if (!gh->enabled)
		return;

	switch (pe->type) {
	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
		case GEVENT_MOUSE:
		case GEVENT_TOUCH:

			// Ignore anything other than the primary mouse button going up or down
			if (!((pme->current_buttons ^ pme->last_buttons) & GINPUT_MOUSE_BTN_LEFT))
				return;

			if ((pme->current_buttons & GINPUT_MOUSE_BTN_LEFT)
					&& pme->x >= gbw->gwin.x && pme->x < gbw->gwin.x + gbw->gwin.width
					&& pme->y >= gbw->gwin.y && pme->y < gbw->gwin.y + gbw->gwin.height) {

				gbw->state = !gbw->state;

				gwinCheckboxDraw((GHandle)param);
			}
	#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */

		default:
			return;
	}

	#undef gh
	#undef pbe
	#undef pme
	#undef pte
	#undef pxe
	#undef gsh
	#undef gbw
}

GHandle gwinCheckboxCreate(GCheckboxObject *gb, coord_t x, coord_t y, coord_t width, coord_t height) {
	if (!(gb = (GCheckboxObject *)_gwinInit((GWindowObject *)gb, x, y, width, height, sizeof(GCheckboxObject))))
		return 0;

	gb->gwin.type = GW_CHECKBOX;
	gb->fn = 0;
	gb->param = 0;
	gb->state = GCHBX_UNSET;

	gwinCheckboxSetStyle(&gb->gwin, GCHBX_NORMAL, &GCheckboxDefaultStyleSelected, &GCheckboxDefaultStyleUnselected);
	geventListenerInit(&gb->listener);
	geventRegisterCallback(&gb->listener, gwinCheckboxCallback, gb);

	// checkboxes are enabled by default
	gb->gwin.enabled = TRUE;

	return (GHandle)gb;
}

void gwinCheckboxSetStyle(GHandle gh, GCheckboxShape shape, const GCheckboxDrawStyle *pSelected, const GCheckboxDrawStyle *pUnselected) {
	#define gcw		((GCheckboxObject *)gh)

	if (gh->type != GW_CHECKBOX)
		return;

	switch (shape) {
		case GCHBX_NORMAL:
			gcw->fn = gwinCheckboxDraw_Normal;
			break;

		default:
			gcw->fn = gwinCheckboxDraw_Normal;
			break;
	}

	if (pSelected) {
		gcw->set.color = pSelected->color;
	}

	if (pUnselected) {
		gcw->unset.color = pSelected->color;
	}

	#undef gcw
}

void gwinCheckboxSetEnabled(GHandle gh, bool_t enabled) {
	if (gh->type != GW_CHECKBOX)
		return;

	gh->enabled = enabled;
}

void gwinCheckboxDraw(GHandle gh) {
	#define gcw		((GCheckboxObject *)gh)

	if (gh->type != GW_CHECKBOX)
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif

	gcw->fn(gh,
            gcw->gwin.enabled,
            gcw->state,
            gcw->param);

	#undef gcw
}

void gwinCheckboxDraw_Normal(GHandle gh, bool_t enabled, bool_t state, void* param) {
	(void) enabled;
	(void) param;

	if (state)
		gdispClear(Green);
	else
		gdispClear(Red);
}

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	bool_t gwinCheckboxAttachMouse(GHandle gh, uint16_t instance) {
		GSourceHandle gsh;

		if (gh->type != GW_CHECKBOX || !(gsh = ginputGetMouse(instance)))
			return FALSE;

		return geventAttachSource(&((GCheckboxObject *)gh)->listener, gsh, GLISTEN_MOUSEMETA);
	}
#endif

#endif /* (GFX_USE_GWIN && GWIN_NEED_CHECKBOX) */
/** @} */

