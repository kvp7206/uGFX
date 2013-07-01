/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://chibios-gfx.com/license.html
 */

/**
 * @file	src/gwin/image.c
 * @brief	GWIN sub-system image code.
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_IMAGE

#include "gwin/class_gwin.h"

#define widget(gh)	((GImageWidget*)gh) 

static void _destroy(GWindowObject *gh) {
	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);

	return;
}

static void _afterClear(GWindowObject *gh) {
	(void)gh;	

	return;
}

static const gwinVMT imageVMT = {
	"Image",					// The class name
	sizeof(GImageWidget),		// The object size
	_destroy,					// The destroy routine
	0,
	_afterClear,				// The after-clear routine
};

GHandle gwinImageCreate(GImageWidget *widget, GWindowInit *pInit) {
	if (!(widget = (GImageWidget *)_gwindowCreate(&widget->g, pInit, &imageVMT, 0)))
		return 0;

	widget->image = gfxAlloc(sizeof(gdispImage));
	if (widget->image == NULL)
		return 0;

	widget->g.x = pInit->x;
	widget->g.y = pInit->y;
	widget->g.width = pInit->width;
	widget->g.height = pInit->height;
	widget->bgColor = Black;
	gwinSetVisible((GHandle)widget, pInit->show);

	return (GHandle)widget;
}

bool_t gwinImageOpenMemory(GHandle gh, const void* memory) {
	bool_t err;

	err = gdispImageSetMemoryReader(widget(gh)->image, memory);
	gdispImageOpen(widget(gh)->image);

	return err;
}

#if defined(WIN32) || GFX_USE_OS_WIN32 || GFX_USE_OS_POSIX || defined(__DOXYGEN__)
bool_t gwinImageOpenFile(GHandle gh, const char* filename) {
	return gdispImageSetFileReader(widget(gh)->image, filename);
}
#endif 

#if GFX_USE_OS_CHIBIOS || defined(__DOXYGEN__)
bool_t gwinImageOpenStream(GHandle gh, void *streamPtr) {
	return gdispImageSetBaseFileStreamReader(widget(gh)->image, streamPtr);
}
#endif

gdispImageError gwinImageCache(GHandle gh) {
	return gdispImageCache(widget(gh)->image);
}

void gwinImageSetBgColor(GHandle gh, color_t bgColor) {
	widget(gh)->bgColor = bgColor;
}

void gwinImageDraw(GHandle gh) {
	gdispImageDraw(widget(gh)->image, widget(gh)->g.x, widget(gh)->g.y, widget(gh)->g.width, widget(gh)->g.height, 0, 0);
} 

#endif // GFX_USE_GWIN && GWIN_NEED_IMAGE
/** @} */

