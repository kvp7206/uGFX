/*
 * This file is subject to the terms of the GFX License, v1.0. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
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

#include "gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_BUTTON) || defined(__DOXYGEN__)

#include "gwin/class_gwin.h"

// Parameters for various shapes
#define RND_CNR_SIZE			5		// Rounded corner size for rounded buttons
#define ARROWHEAD_DIVIDER		4		// A quarter of the height for the arrow head
#define ARROWBODY_DIVIDER		4		// A quarter of the width for the arrow body

// Our pressed state
#define GBUTTON_FLG_PRESSED		(GWIN_FIRST_CONTROL_FLAG<<0)

// Prototypes for button VMT functions
static void MouseDown(GWidgetObject *gw, coord_t x, coord_t y);
static void MouseUp(GWidgetObject *gw, coord_t x, coord_t y);
static void ToggleOff(GWidgetObject *gw, uint16_t instance);
static void ToggleOn(GWidgetObject *gw, uint16_t instance);

// The button VMT table
static const gwidgetVMT buttonVMT = {
	{
		"Button",				// The classname
		_gwidgetDestroy,		// The destroy routine
		0,						// The after-clear routine
	},
	gwinButtonDraw_3D,		// The default drawing routine
	MouseDown,				// Process mouse down events
	MouseUp,				// Process mouse up events
	0,						// Process mouse move events (NOT USED)
	ToggleOff,				// Process toggle off events
	ToggleOn,				// Process toggle on events
	0,						// Process dial move events (NOT USED)
	0,						// Process all events (NOT USED)
	0,						// AssignToggle (NOT USED)
	0,						// AssignDial (NOT USED)
};

// Default color scheme
static const GButtonColors GButtonDefaultColorsUp = {
	HTML2COLOR(0x404040),		// color_up_edge;
	HTML2COLOR(0xE0E0E0),		// color_up_fill;
	HTML2COLOR(0x000000),		// color_up_txt;
};
static const GButtonColors GButtonDefaultColorsDown = {
	HTML2COLOR(0x404040),		// color_dn_edge;
	HTML2COLOR(0x808080),		// color_dn_fill;
	HTML2COLOR(0x404040),		// color_dn_txt;
};
static const GButtonColors GButtonDefaultColorsDisabled = {
	HTML2COLOR(0x808080),		// color_dis_edge;
	HTML2COLOR(0xE0E0E0),		// color_dis_fill;
	HTML2COLOR(0xC0C0C0),		// color_dis_txt;
};

// Send the button event
static void SendButtonEvent(GWidgetObject *gw) {
	GSourceListener	*	psl;
	GEvent *			pe;
	#define pbe			((GEventGWinButton *)pe)

	// Trigger a GWIN Button Event
	psl = 0;
	while ((psl = geventGetSourceListener((GSourceHandle)gw, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;
		pbe->type = GEVENT_GWIN_BUTTON;
		pbe->button = (GHandle)gw;
		geventSendEvent(psl);
	}

	#undef pbe
}

// A mouse down has occurred over the button
static void MouseDown(GWidgetObject *gw, coord_t x, coord_t y) {
	(void) x; (void) y;
	gw->g.flags |= GBUTTON_FLG_PRESSED;
	gwinDraw((GHandle)gw);
}

// A mouse up has occurred (it may or may not be over the button)
static void MouseUp(GWidgetObject *gw, coord_t x, coord_t y) {
	(void) x; (void) y;
	gw->g.flags &= ~GBUTTON_FLG_PRESSED;
	gwinDraw((GHandle)gw);

	#if !GWIN_BUTTON_LAZY_RELEASE
		// If the mouse up was not over the button then cancel the event
		if (x < 0 || y < 0 || x >= gw->g.width || y >= gw->g.height)
			return;
	#endif

	SendButtonEvent(gw);
}

// A toggle off has occurred
static void ToggleOff(GWidgetObject *gw, uint16_t instance) {
	(void) instance;
	gw->g.flags &= ~GBUTTON_FLG_PRESSED;
	gwinDraw((GHandle)gw);
}

// A toggle on has occurred
static void ToggleOn(GWidgetObject *gw, uint16_t instance) {
	(void) instance;
	gw->g.flags |= GBUTTON_FLG_PRESSED;
	gwinDraw((GHandle)gw);
	// Trigger the event on button down (different than for mouse/touch)
	SendButtonEvent(gw);
}

GHandle gwinCreateButton(GButtonObject *gw, coord_t x, coord_t y, coord_t width, coord_t height) {
	if (!(gw = (GButtonObject *)_gwidgetInit((GWidgetObject *)gw, x, y, width, height, sizeof(GButtonObject), &buttonVMT)))
		return 0;

	gw->c_up = GButtonDefaultColorsUp;
	gw->c_dn = GButtonDefaultColorsDown;
	gw->c_dis = GButtonDefaultColorsDisabled;
	return (GHandle)gw;
}

void gwinSetButtonColors(GHandle gh, const GButtonColors *pUp, const GButtonColors *pDown, const GButtonColors *pDisabled) {
	if (gh->vmt != (gwinVMT *)&buttonVMT)
		return;

	if (pUp)		((GButtonObject *)gh)->c_up = *pUp;
	if (pDown)		((GButtonObject *)gh)->c_dn = *pDown;
	if (pDisabled)	((GButtonObject *)gh)->c_dis = *pDisabled;
}

bool_t gwinIsButtonPressed(GHandle gh) {
	if (gh->vmt != (gwinVMT *)&buttonVMT)
		return FALSE;

	return (gh->flags & GBUTTON_FLG_PRESSED) ? TRUE : FALSE;
}

/*----------------------------------------------------------
 * Custom Draw Routines
 *----------------------------------------------------------*/

static GButtonColors *getDrawColors(GWidgetObject *gw) {
	if (!(gw->g.flags & GWIN_FLG_ENABLED))		return &((GButtonObject *)gw)->c_dis;
	if ((gw->g.flags & GBUTTON_FLG_PRESSED))	return &((GButtonObject *)gw)->c_dn;
	return &((GButtonObject *)gw)->c_up;
}

void gwinButtonDraw_3D(GWidgetObject *gw, void *param) {
	(void)			param;
	GButtonColors *	pcol;

	if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
	pcol = getDrawColors(gw);
	
	gdispFillStringBox(gw->g.x, gw->g.y, gw->g.width-1, gw->g.height-1, gw->txt, gw->g.font, pcol->color_txt, pcol->color_fill, justifyCenter);
	gdispDrawLine(gw->g.x+gw->g.width-1, gw->g.y, gw->g.x+gw->g.width-1, gw->g.y+gw->g.height-1, pcol->color_edge);
	gdispDrawLine(gw->g.x, gw->g.y+gw->g.height-1, gw->g.x+gw->g.width-2, gw->g.y+gw->g.height-1, pcol->color_edge);
}

void gwinButtonDraw_Box(GWidgetObject *gw, void *param) {
	(void)			param;
	GButtonColors *	pcol;

	if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
	pcol = getDrawColors(gw);
	
	gdispFillStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, pcol->color_fill, justifyCenter);
	gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->color_edge);
}

#if GDISP_NEED_ARC
	void gwinButtonDraw_Rounded(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		if (gw->g.width >= 2*RND_CNR_SIZE+10) {
			gdispFillRoundedBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, RND_CNR_SIZE-1, pcol->color_fill);
			gdispDrawStringBox(gw->g.x+1, gw->g.y+RND_CNR_SIZE, gw->g.width-2, gw->g.height-(2*RND_CNR_SIZE), gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
			gdispDrawRoundedBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, RND_CNR_SIZE, pcol->color_edge);
		} else {
			gdispFillStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, pcol->color_fill, justifyCenter);
			gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, pcol->color_edge);
		}
	}
#endif

#if GDISP_NEED_ELLIPSE
	void gwinButtonDraw_Ellipse(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		gdispFillEllipse(gw->g.x+1, gw->g.y+1, gw->g.width/2-1, gw->g.height/2-1, pcol->color_fill);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
		gdispDrawEllipse(gw->g.x, gw->g.y, gw->g.width/2, gw->g.height/2, pcol->color_edge);
	}
#endif

#if GDISP_NEED_CONVEX_POLYGON
	void gwinButtonDraw_ArrowUp(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;
		point			arw[7];

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		arw[0].x = gw->g.width/2; arw[0].y = 0;
		arw[1].x = gw->g.width-1; arw[1].y = gw->g.height/ARROWHEAD_DIVIDER;
		arw[2].x = (gw->g.width + gw->g.width/ARROWBODY_DIVIDER)/2; arw[2].y = gw->g.height/ARROWHEAD_DIVIDER;
		arw[3].x = (gw->g.width + gw->g.width/ARROWBODY_DIVIDER)/2; arw[3].y = gw->g.height-1;
		arw[4].x = (gw->g.width - gw->g.width/ARROWBODY_DIVIDER)/2; arw[4].y = gw->g.height-1;
		arw[5].x = (gw->g.width - gw->g.width/ARROWBODY_DIVIDER)/2; arw[5].y = gw->g.height/ARROWHEAD_DIVIDER;
		arw[6].x = 0; arw[6].y = gw->g.height/ARROWHEAD_DIVIDER;

		gdispFillConvexPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_fill);
		gdispDrawPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_edge);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowDown(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;
		point			arw[7];

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		arw[0].x = gw->g.width/2; arw[0].y = gw->g.height-1;
		arw[1].x = gw->g.width-1; arw[1].y = gw->g.height-1-gw->g.height/ARROWHEAD_DIVIDER;
		arw[2].x = (gw->g.width + gw->g.width/ARROWBODY_DIVIDER)/2; arw[2].y = gw->g.height-1-gw->g.height/ARROWHEAD_DIVIDER;
		arw[3].x = (gw->g.width + gw->g.width/ARROWBODY_DIVIDER)/2; arw[3].y = 0;
		arw[4].x = (gw->g.width - gw->g.width/ARROWBODY_DIVIDER)/2; arw[4].y = 0;
		arw[5].x = (gw->g.width - gw->g.width/ARROWBODY_DIVIDER)/2; arw[5].y = gw->g.height-1-gw->g.height/ARROWHEAD_DIVIDER;
		arw[6].x = 0; arw[6].y = gw->g.height-1-gw->g.height/ARROWHEAD_DIVIDER;

		gdispFillConvexPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_fill);
		gdispDrawPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_edge);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowLeft(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;
		point			arw[7];

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		arw[0].x = 0; arw[0].y = gw->g.height/2;
		arw[1].x = gw->g.width/ARROWHEAD_DIVIDER; arw[1].y = 0;
		arw[2].x = gw->g.width/ARROWHEAD_DIVIDER; arw[2].y = (gw->g.height - gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[3].x = gw->g.width-1; arw[3].y = (gw->g.height - gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[4].x = gw->g.width-1; arw[4].y = (gw->g.height + gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[5].x = gw->g.width/ARROWHEAD_DIVIDER; arw[5].y = (gw->g.height + gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[6].x = gw->g.width/ARROWHEAD_DIVIDER; arw[6].y = gw->g.height-1;

		gdispFillConvexPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_fill);
		gdispDrawPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_edge);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
	}

	void gwinButtonDraw_ArrowRight(GWidgetObject *gw, void *param) {
		(void)			param;
		GButtonColors *	pcol;
		point			arw[7];

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;
		pcol = getDrawColors(gw);

		arw[0].x = gw->g.width-1; arw[0].y = gw->g.height/2;
		arw[1].x = gw->g.width-1-gw->g.width/ARROWHEAD_DIVIDER; arw[1].y = 0;
		arw[2].x = gw->g.width-1-gw->g.width/ARROWHEAD_DIVIDER; arw[2].y = (gw->g.height - gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[3].x = 0; arw[3].y = (gw->g.height - gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[4].x = 0; arw[4].y = (gw->g.height + gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[5].x = gw->g.width-1-gw->g.width/ARROWHEAD_DIVIDER; arw[5].y = (gw->g.height + gw->g.height/ARROWBODY_DIVIDER)/2;
		arw[6].x = gw->g.width-1-gw->g.width/ARROWHEAD_DIVIDER; arw[6].y = gw->g.height-1;

		gdispFillConvexPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_fill);
		gdispDrawPoly(gw->g.x, gw->g.y, arw, 7, pcol->color_edge);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
	}
#endif

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	void gwinButtonDraw_Image(GWidgetObject *gw, void *param) {
		GButtonColors *	pcol;
		coord_t			sy;

		if (gw->g.vmt != (gwinVMT *)&buttonVMT)	return;

		if (!(gw->g.flags & GWIN_FLG_ENABLED)) {
			pcol = &((GButtonObject *)gw)->c_dis;
			sy = 2 * gw->g.height;
		} else if ((gw->g.flags & GBUTTON_FLG_PRESSED)) {
			pcol = &((GButtonObject *)gw)->c_dn;
			sy = gw->g.height;
		} else {
			pcol = &((GButtonObject *)gw)->c_up;
			sy = 0;
		}

		gdispImageDraw((gdispImage *)param, gw->g.x, gw->g.y, gw->g.width, gw->g.height, 0, sy);
		gdispDrawStringBox(gw->g.x+1, gw->g.y+1, gw->g.width-2, gw->g.height-2, gw->txt, gw->g.font, pcol->color_txt, justifyCenter);
	}
#endif

#endif /* GFX_USE_GWIN && GWIN_NEED_BUTTON */
/** @} */

