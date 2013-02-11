/*
    ChibiOS/GFX - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file    src/gwin/button.c
 * @brief   GWIN sub-system button code.
 *
 * @defgroup Button Button
 * @ingroup GWIN
 *
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_BUTTON) || defined(__DOXYGEN__)

#include <string.h>

#include "gwin/internal.h"

static const GButtonStyle GButtonDefaultStyle = {
	GBTN_3D,
	HTML2COLOR(0x404040),		// color_up_edge;
	HTML2COLOR(0xE0E0E0),		// color_up_fill;
	HTML2COLOR(0x000000),		// color_up_txt;
	HTML2COLOR(0x404040),		// color_dn_edge;
	HTML2COLOR(0x808080),		// color_dn_fill;
	HTML2COLOR(0x404040),		// color_dn_txt;
	};

// Process an event callback
static void gwinButtonCallback(void *param, GEvent *pe) {
	GSourceListener	*psl;
	#define gh		((GHandle)param)
	#define gbw		((GButtonObject *)param)
	#define gsh		((GSourceHandle)param)
	#define pme		((GEventMouse *)pe)
	#define pte		((GEventTouch *)pe)
	#define pxe		((GEventToggle *)pe)
	#define pbe		((GEventGWinButton *)pe)

	switch (pe->type) {
	#if defined(GINPUT_NEED_MOUSE) && GINPUT_NEED_MOUSE
		case GEVENT_MOUSE:
		case GEVENT_TOUCH:
			// Ignore anything other than the primary mouse button going up or down
			if (!((pme->current_buttons ^ pme->last_buttons) & GINPUT_MOUSE_BTN_LEFT))
				return;

			if (gbw->state == GBTN_UP) {
				// Our button is UP: Test for button down over the button
				if ((pme->current_buttons & GINPUT_MOUSE_BTN_LEFT)
						&& pme->x >= gbw->gwin.x && pme->x < gbw->gwin.x + gbw->gwin.width
						&& pme->y >= gbw->gwin.y && pme->y < gbw->gwin.y + gbw->gwin.height) {
					gbw->state = GBTN_DOWN;
					gwinButtonDraw((GHandle)param);
				}
				return;
			}

			// Our button is DOWN

			// Skip more mouse downs
			if ((pme->current_buttons & GINPUT_MOUSE_BTN_LEFT))
				return;

			// This must be a mouse up - set the button as UP
			gbw->state = GBTN_UP;
			gwinButtonDraw((GHandle)param);

			// If the mouse up was over the button then create the event
			if (pme->x >= gbw->gwin.x && pme->x < gbw->gwin.x + gbw->gwin.width
					&& pme->y >= gbw->gwin.y && pme->y < gbw->gwin.y + gbw->gwin.height)
				break;

			return;
	#endif

	#if defined(GINPUT_NEED_TOGGLE) && GINPUT_NEED_TOGGLE
		case GEVENT_TOGGLE:
			// State has changed - update the button
			gbw->state = pxe->on ? GBTN_DOWN : GBTN_UP;
			gwinButtonDraw((GHandle)param);

			// Trigger the event on button down (different than for mouse/touch)
			if (gbw->state == GBTN_DOWN)
				break;

			return;
	#endif

	default:
		return;
	}

	// Trigger a GWIN Button Event
	psl = 0;
	while ((psl = geventGetSourceListener(gsh, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pbe->type = GEVENT_GWIN_BUTTON;
		pbe->button = gh;
		geventSendEvent(psl);
	}

	#undef pbe
	#undef pme
	#undef pte
	#undef pxe
	#undef gsh
	#undef gbw
	#undef gh
}

GHandle gwinCreateButton(GButtonObject *gb, coord_t x, coord_t y, coord_t width, coord_t height, font_t font, GButtonType type) {
	if (!(gb = (GButtonObject *)_gwinInit((GWindowObject *)gb, x, y, width, height, sizeof(GButtonObject))))
		return 0;
	gb->gwin.type = GW_BUTTON;
	gwinSetFont(&gb->gwin, font);
	gwinSetButtonStyle(&gb->gwin, &GButtonDefaultStyle);
	gb->type = type;
	gb->state = GBTN_UP;
	gb->txt = "";
	geventListenerInit(&gb->listener);
	geventRegisterCallback(&gb->listener, gwinButtonCallback, gb);
	return (GHandle)gb;
}

void gwinSetButtonStyle(GHandle gh, const GButtonStyle *style) {
	#define gbw		((GButtonObject *)gh)
	if (gh->type != GW_BUTTON)
		return;
	
	gbw->style.shape = style->shape;
	gbw->style.color_up_edge = style->color_up_edge;
	gbw->style.color_up_fill = style->color_up_fill;
	gbw->style.color_dn_edge = style->color_dn_edge;
	gbw->style.color_dn_fill = style->color_dn_fill;
	gbw->style.color_up_txt = style->color_up_txt;
	gbw->style.color_dn_txt = style->color_dn_txt;
	#undef gbw
}

void gwinSetButtonText(GHandle gh, const char *txt, bool_t useAlloc) {
	#define gbw		((GButtonObject *)gh)
	if (gh->type != GW_BUTTON)
		return;

	// Dispose of the old string
	if ((gh->flags & GBTN_FLG_ALLOCTXT)) {
		gh->flags &= ~GBTN_FLG_ALLOCTXT;
		if (gbw->txt) {
			chHeapFree((void *)gbw->txt);
			gbw->txt = "";
		}
	}
	// Alloc the new text if required
	if (txt && useAlloc) {
		char *str;
		
		if ((str = (char *)chHeapAlloc(NULL, strlen(txt)+1))) {
			gh->flags |= GBTN_FLG_ALLOCTXT;
			strcpy(str, txt);
		}
		txt = (const char *)str;
	}
	
	gbw->txt = txt ? txt : "";
	#undef gbw
}

void gwinButtonDraw(GHandle gh) {
	color_t			cedge;
	color_t			cfill;
	color_t			ctxt;
	const char *	txt;
	#define gbw		((GButtonObject *)gh)
	#define RND_CNR_SIZE	5
	
	if (gh->type != GW_BUTTON)
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif
	
	// Get the text (safely)
	txt = gh->font && gbw->txt ? gbw->txt : "";
	
	// Determine the colors to use
	switch(gbw->state) {
	case GBTN_DOWN:
		cedge = gbw->style.color_dn_edge;
		cfill = gbw->style.color_dn_fill;
		ctxt = gbw->style.color_dn_txt;
		break;
	case GBTN_UP:	default:
		cedge = gbw->style.color_up_edge;
		cfill = gbw->style.color_up_fill;
		ctxt = gbw->style.color_up_txt;
		break;
	}
	
	// Draw according to the shape specified.
	switch(gbw->style.shape) {
#if GDISP_NEED_ARC
	case GBTN_ROUNDED:
		if (gh->width >= 2*RND_CNR_SIZE+10) {
			gdispFillRoundedBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, RND_CNR_SIZE-1, cfill);
			gdispDrawStringBox(gh->x+1, gh->y+RND_CNR_SIZE, gh->width-2, gh->height-(2*RND_CNR_SIZE), txt, gh->font, ctxt, justifyCenter);
			gdispDrawRoundedBox(gh->x, gh->y, gh->width, gh->height, RND_CNR_SIZE, cedge);
			break;
		}
		/* Fall Through */
#endif
	case GBTN_SQUARE:
		gdispFillStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, ctxt, cfill, justifyCenter);
		gdispDrawBox(gh->x, gh->y, gh->width, gh->height, cedge);
		break;
#if GDISP_NEED_ELLIPSE
	case GBTN_ELLIPSE:
		gdispFillEllipse(gh->x+1, gh->y+1, gh->width/2-1, gh->height/2-1, cfill);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, ctxt, justifyCenter);
		gdispDrawEllipse(gh->x, gh->y, gh->width/2, gh->height/2, cedge);
		break;
#endif
	case GBTN_3D:	default:
		gdispFillStringBox(gh->x, gh->y, gh->width-1, gh->height-1, txt, gh->font, ctxt, cfill, justifyCenter);
		gdispDrawLine(gh->x+gh->width-1, gh->y, gh->x+gh->width-1, gh->y+gh->height-1, cedge);
		gdispDrawLine(gh->x, gh->y+gh->height-1, gh->x+gh->width-2, gh->y+gh->height-1, cedge);
		break;
	}
	#undef gbw
}

#if defined(GINPUT_NEED_MOUSE) && GINPUT_NEED_MOUSE
	bool_t gwinAttachButtonMouseSource(GHandle gh, GSourceHandle gsh) {
		if (gh->type != GW_BUTTON)
			return FALSE;

		return geventAttachSource(&((GButtonObject *)gh)->listener, gsh, GLISTEN_MOUSEMETA);
	}
#endif

#if defined(GINPUT_NEED_TOGGLE) && GINPUT_NEED_TOGGLE
	bool_t gwinAttachButtonToggleSource(GHandle gh, GSourceHandle gsh) {
		if (gh->type != GW_BUTTON)
			return FALSE;

		return geventAttachSource(&((GButtonObject *)gh)->listener, gsh, GLISTEN_TOGGLE_OFF|GLISTEN_TOGGLE_ON);
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
/** @} */

