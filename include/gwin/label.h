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
 * @details		GWIN allos it to create an label widget. The widget
 *				takes no user input.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_LABEL must be set to TRUE in your gfxconf.h
 * @pre			The font you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_LABEL_H
#define _GWIN_LABEL_H

// This file is included within "gwin/gwin.h"

// An label window
typedef struct GLabelWidget_t {
	GWindowObject	g;

	char*			text;
} GLabelWidget;

#ifdef __cplusplus
extern "C" {
#endif

GHandle gwinLabelCreate(GLabelWidget *widget, GWindowInit *pInit);
void gwinLabelSetColor(GHandle gh, color_t color);
void gwinLabelSetBgColor(GHandle gh, color_t bgColor);
void gwinLabelSetText(GHandle gh, char* text);
void gwinLabelDraw(GHandle gh);

#ifdef __cplusplus
}
#endif

#endif // _GWIN_LABEL_H
/** @} */

