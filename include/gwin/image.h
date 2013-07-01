/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file	include/gwin/image.h
 * @brief	GWIN image widget header file.
 *
 * @defgroup Image Image
 * @ingroup GWIN
 *
 * @details		GWIN allos it to create an image widget. The widget
 *				takes no user input.
 *
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_IMAGE must be set to TRUE in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_IMAGE_H
#define _GWIN_IMAGE_H

// This file is included within "gwin/gwin.h"

// An image window
typedef struct GImageWidget_t {
	GWindowObject	g;
} GImageWidget;

#ifdef __cplusplus
extern "C" {
#endif

GHandle gwinImageCreate(GImageWidget *widget, GWindowInit *pInit);
void gwinImageDisplay(GImageWidget *widget, gdispImage *image);

#ifdef __cplusplus
}
#endif

#endif // _GWIN_IMAGE_H
/** @} */

