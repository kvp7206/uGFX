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

#define widget(gh)	((GLabelWidget*)gh) 

static void _destroy(GWindowObject *gh) {
	(void)gh;

	return;
}

static void _redraw(GWindowObject *gh) {
	(void)gh;

	return;
}

static void _afterClear(GWindowObject *gh) {
	(void)gh;

	return;
}

static const gwinVMT labelVMT = {
	"Label",				// The class name
	sizeof(GLabelWidget),	// The object size
	_destroy,				// The destroy routine
	0,						// The redraw routine
	_afterClear				// The after-clear routine
};

GHandle gwinLabelCreate(GLabelWidget *widget, GWindowInit *pInit) {
	if (!(widget = (GLabelWidget *)_gwindowCreate(&widget->g, pInit, &labelVMT, 0)))
		return 0;

	widget->g.x = pInit->x;
	widget->g.y = pInit->y;
	widget->g.width = pInit->width;
	widget->g.height = pInit->height;
	gwinSetVisible((GHandle)widget, pInit->show);

	return (GHandle)widget;
}

void gwinLabelSetColor(GHandle gh, color_t color) {
	widget(gh)->g.color = color;
}

void gwinLabelSetBgColor(GHandle gh, color_t bgColor) {
	widget(gh)->g.bgcolor = bgColor;
}

void gwinLabelSetFont(GHandle gh, font_t font) {
	widget(gh)->g.font = font;
}

void gwinLabelSetText(GHandle gh, const char* text) {
	widget(gh)->text = text;

	gwinLabelDraw(gh);
}

void gwinLabelDraw(GHandle gh) {
	gdispFillString(	widget(gh)->g.x,
						widget(gh)->g.y,
						widget(gh)->text,
						widget(gh)->g.font,
						widget(gh)->g.color,
						widget(gh)->g.bgcolor
					);
}

#endif // GFX_USE_GWIN && GFX_NEED_LABEL

