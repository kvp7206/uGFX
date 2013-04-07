/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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

/* Parameters for various shapes */
#define RND_CNR_SIZE			5		// Rounded corner size for rounded buttons
#define ARROWHEAD_DIVIDER		4		// A quarter of the height for the arrow head
#define ARROWBODY_DIVIDER		4		// A quarter of the width for the arrow body

#include <string.h>

#include "gwin/internal.h"

#define GWIN_BUTTON_DEFAULT_SHAPE		GBTN_3D

static const GButtonDrawStyle GButtonDefaultStyleUp = {
	HTML2COLOR(0x404040),		// color_up_edge;
	HTML2COLOR(0xE0E0E0),		// color_up_fill;
	HTML2COLOR(0x000000),		// color_up_txt;
};

static const GButtonDrawStyle GButtonDefaultStyleDown = {
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
	#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
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

			#if GWIN_BUTTON_LAZY_RELEASE
				break;
			#else
				// If the mouse up was over the button then create the event
				if (pme->x >= gbw->gwin.x && pme->x < gbw->gwin.x + gbw->gwin.width
						&& pme->y >= gbw->gwin.y && pme->y < gbw->gwin.y + gbw->gwin.height)
					break;

				return;
			#endif
	#endif

	#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
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
	gb->fn = 0;
	gb->param = 0;
	gwinSetFont(&gb->gwin, font);
	gwinSetButtonStyle(&gb->gwin, GWIN_BUTTON_DEFAULT_SHAPE, &GButtonDefaultStyleUp, &GButtonDefaultStyleDown);
	gb->type = type;
	gb->state = GBTN_UP;
	gb->txt = "";
	geventListenerInit(&gb->listener);
	geventRegisterCallback(&gb->listener, gwinButtonCallback, gb);
	return (GHandle)gb;
}

void gwinSetButtonStyle(GHandle gh, GButtonShape shape, const GButtonDrawStyle *pUp, const GButtonDrawStyle *pDown) {
	#define gbw		((GButtonObject *)gh)
	if (gh->type != GW_BUTTON)
		return;

	switch(shape) {
		case GBTN_SQUARE:		gbw->fn = gwinButtonDraw_Square;		break;
		#if GDISP_NEED_ARC
			case GBTN_ROUNDED:	gbw->fn = gwinButtonDraw_Rounded;		break;
		#endif
		#if GDISP_NEED_ELLIPSE
			case GBTN_ELLIPSE:	gbw->fn = gwinButtonDraw_Ellipse;		break;
		#endif

		#if GDISP_NEED_CONVEX_POLYGON
			case GBTN_ARROW_UP:		gbw->fn = gwinButtonDraw_ArrowUp;		break;
			case GBTN_ARROW_DOWN:	gbw->fn = gwinButtonDraw_ArrowDown;		break;
			case GBTN_ARROW_LEFT:	gbw->fn = gwinButtonDraw_ArrowLeft;		break;
			case GBTN_ARROW_RIGHT:	gbw->fn = gwinButtonDraw_ArrowRight;	break;
		#endif

		case GBTN_CUSTOM:		if (gbw->fn)	break;		/* Fall Through */
		case GBTN_3D:			/* Fall through */
		default:				gbw->fn = gwinButtonDraw_3D;			break;
	}
	if (pUp) {
		gbw->up.color_edge = pUp->color_edge;
		gbw->up.color_fill = pUp->color_fill;
		gbw->up.color_txt = pUp->color_txt;
	}
	if (pDown) {
		gbw->dn.color_edge = pDown->color_edge;
		gbw->dn.color_fill = pDown->color_fill;
		gbw->dn.color_txt = pDown->color_txt;
	}
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
	#define gbw		((GButtonObject *)gh)
	
	if (gh->type != GW_BUTTON)
		return;

	#if GDISP_NEED_CLIP
		gdispSetClip(gh->x, gh->y, gh->width, gh->height);
	#endif

	gbw->fn(gh,
			gbw->state == GBTN_DOWN,
			gh->font && gbw->txt ? gbw->txt : "",
			gbw->state == GBTN_DOWN ? &gbw->dn : &gbw->up,
			gbw->param);

	#undef gbw
}

void gwinSetButtonCustom(GHandle gh, GButtonDrawFunction fn, void *param) {
	#define gbw		((GButtonObject *)gh)

	if (gh->type != GW_BUTTON)
		return;

	gbw->fn = fn ? fn : gwinButtonDraw_3D;
	gbw->param = param;

	#undef gbw
}

void gwinButtonDraw_3D(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
	(void) isdown;
	(void) param;

	gdispFillStringBox(gh->x, gh->y, gh->width-1, gh->height-1, txt, gh->font, pstyle->color_txt, pstyle->color_fill, justifyCenter);
	gdispDrawLine(gh->x+gh->width-1, gh->y, gh->x+gh->width-1, gh->y+gh->height-1, pstyle->color_edge);
	gdispDrawLine(gh->x, gh->y+gh->height-1, gh->x+gh->width-2, gh->y+gh->height-1, pstyle->color_edge);
}

void gwinButtonDraw_Square(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
	(void) isdown;
	(void) param;

	gdispFillStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, pstyle->color_fill, justifyCenter);
	gdispDrawBox(gh->x, gh->y, gh->width, gh->height, pstyle->color_edge);
}

#if GDISP_NEED_ARC
	void gwinButtonDraw_Rounded(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;

		if (gh->width >= 2*RND_CNR_SIZE+10) {
			gdispFillRoundedBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, RND_CNR_SIZE-1, pstyle->color_fill);
			gdispDrawStringBox(gh->x+1, gh->y+RND_CNR_SIZE, gh->width-2, gh->height-(2*RND_CNR_SIZE), txt, gh->font, pstyle->color_txt, justifyCenter);
			gdispDrawRoundedBox(gh->x, gh->y, gh->width, gh->height, RND_CNR_SIZE, pstyle->color_edge);
		} else {
			gdispFillStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, pstyle->color_fill, justifyCenter);
			gdispDrawBox(gh->x, gh->y, gh->width, gh->height, pstyle->color_edge);
		}
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gwinButtonDraw_Ellipse(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;

		gdispFillEllipse(gh->x+1, gh->y+1, gh->width/2-1, gh->height/2-1, pstyle->color_fill);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, justifyCenter);
		gdispDrawEllipse(gh->x, gh->y, gh->width/2, gh->height/2, pstyle->color_edge);
	}
#endif

#if GDISP_NEED_CONVEX_POLYGON
	void gwinButtonDraw_ArrowUp(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;
		point	arw[7];

		arw[0].x = gh->width/2; arw[0].y = 0;
		arw[1].x = gh->width-1; arw[1].y = gh->height/ARROWHEAD_DIVIDER;
		arw[2].x = (gh->width + gh->width/ARROWBODY_DIVIDER)/2; arw[2].y = gh->height/ARROWHEAD_DIVIDER;
		arw[3].x = (gh->width + gh->width/ARROWBODY_DIVIDER)/2; arw[3].y = gh->height-1;
		arw[4].x = (gh->width - gh->width/ARROWBODY_DIVIDER)/2; arw[4].y = gh->height-1;
		arw[5].x = (gh->width - gh->width/ARROWBODY_DIVIDER)/2; arw[5].y = gh->height/ARROWHEAD_DIVIDER;
		arw[6].x = 0; arw[6].y = gh->height/ARROWHEAD_DIVIDER;

		gdispFillConvexPoly(gh->x, gh->y, arw, 7, pstyle->color_fill);
		gdispDrawPoly(gh->x, gh->y, arw, 7, pstyle->color_edge);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowDown(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;
		point	arw[7];

		arw[0].x = gh->width/2; arw[0].y = gh->height-1;
		arw[1].x = gh->width-1; arw[1].y = gh->height-1-gh->height/ARROWHEAD_DIVIDER;
		arw[2].x = (gh->width + gh->width/ARROWBODY_DIVIDER)/2; arw[2].y = gh->height-1-gh->height/ARROWHEAD_DIVIDER;
		arw[3].x = (gh->width + gh->width/ARROWBODY_DIVIDER)/2; arw[3].y = 0;
		arw[4].x = (gh->width - gh->width/ARROWBODY_DIVIDER)/2; arw[4].y = 0;
		arw[5].x = (gh->width - gh->width/ARROWBODY_DIVIDER)/2; arw[5].y = gh->height-1-gh->height/ARROWHEAD_DIVIDER;
		arw[6].x = 0; arw[6].y = gh->height-1-gh->height/ARROWHEAD_DIVIDER;

		gdispFillConvexPoly(gh->x, gh->y, arw, 7, pstyle->color_fill);
		gdispDrawPoly(gh->x, gh->y, arw, 7, pstyle->color_edge);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowLeft(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;
		point	arw[7];

		arw[0].x = 0; arw[0].y = gh->height/2;
		arw[1].x = gh->width/ARROWHEAD_DIVIDER; arw[1].y = 0;
		arw[2].x = gh->width/ARROWHEAD_DIVIDER; arw[2].y = (gh->height - gh->height/ARROWBODY_DIVIDER)/2;
		arw[3].x = gh->width-1; arw[3].y = (gh->height - gh->height/ARROWBODY_DIVIDER)/2;
		arw[4].x = gh->width-1; arw[4].y = (gh->height + gh->height/ARROWBODY_DIVIDER)/2;
		arw[5].x = gh->width/ARROWHEAD_DIVIDER; arw[5].y = (gh->height + gh->height/ARROWBODY_DIVIDER)/2;
		arw[6].x = gh->width/ARROWHEAD_DIVIDER; arw[6].y = gh->height-1;

		gdispFillConvexPoly(gh->x, gh->y, arw, 7, pstyle->color_fill);
		gdispDrawPoly(gh->x, gh->y, arw, 7, pstyle->color_edge);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowRight(GHandle gh, bool_t isdown, const char *txt, const GButtonDrawStyle *pstyle, void *param) {
		(void) isdown;
		(void) param;
		point	arw[7];

		arw[0].x = gh->width-1; arw[0].y = gh->height/2;
		arw[1].x = gh->width-1-gh->width/ARROWHEAD_DIVIDER; arw[1].y = 0;
		arw[2].x = gh->width-1-gh->width/ARROWHEAD_DIVIDER; arw[2].y = (gh->height - gh->height/ARROWBODY_DIVIDER)/2;
		arw[3].x = 0; arw[3].y = (gh->height - gh->height/ARROWBODY_DIVIDER)/2;
		arw[4].x = 0; arw[4].y = (gh->height + gh->height/ARROWBODY_DIVIDER)/2;
		arw[5].x = gh->width-1-gh->width/ARROWHEAD_DIVIDER; arw[5].y = (gh->height + gh->height/ARROWBODY_DIVIDER)/2;
		arw[6].x = gh->width-1-gh->width/ARROWHEAD_DIVIDER; arw[6].y = gh->height-1;

		gdispFillConvexPoly(gh->x, gh->y, arw, 7, pstyle->color_fill);
		gdispDrawPoly(gh->x, gh->y, arw, 7, pstyle->color_edge);
		gdispDrawStringBox(gh->x+1, gh->y+1, gh->width-2, gh->height-2, txt, gh->font, pstyle->color_txt, justifyCenter);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_MOUSE
	bool_t gwinAttachButtonMouse(GHandle gh, uint16_t instance) {
		GSourceHandle gsh;

		if (gh->type != GW_BUTTON || !(gsh = ginputGetMouse(instance)))
			return FALSE;

		return geventAttachSource(&((GButtonObject *)gh)->listener, gsh, GLISTEN_MOUSEMETA);
	}
#endif

#if GFX_USE_GINPUT && GINPUT_NEED_TOGGLE
	bool_t gwinAttachButtonToggle(GHandle gh, uint16_t instance) {
		GSourceHandle gsh;

		if (gh->type != GW_BUTTON || !(gsh = ginputGetToggle(instance)))
			return FALSE;

		return geventAttachSource(&((GButtonObject *)gh)->listener, gsh, GLISTEN_TOGGLE_OFF|GLISTEN_TOGGLE_ON);
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
/** @} */

