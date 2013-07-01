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

GHandle gwinLabelCreate(GLabelWidget *widget, GWindowInit *pInit) {

}

void gwinLabelSetColor(GHandle gh, color_t color) {
	widget(gh)->g.color = color;
}

void gwinLabelSetBgColor(GHandle gh, color_t bgColor) {
	widget(gh)->g.bgcolor = bgColor;
}

void gwinLabelSetText(GHandle gh, char* text) {
	widget(gh)->text = text;

	gwinLabelDraw(gh);
}

void gwinLabelDraw(GHandle gh) {

}

#endif // GFX_USE_GWIN && GFX_NEED_LABEL

