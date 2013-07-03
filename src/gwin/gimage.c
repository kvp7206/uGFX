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
}

static void _redraw(GHandle gh) {
	coord_t		x, y, w, h, dx, dy;

	// The default display area
	x = gh->x;
	y = gh->y;
	w = gh->width;
	h = gh->height;

	// If the image isn't open just clear the area
	if (!gdispImageIsOpen(&widget(gh)->image)) {
		gdispFillArea(x, y, w, h, gh->bgcolor);
		return;
	}

	// Center horizontally if the area is larger than the image
	if (widget(gh)->image.width < w) {
		w = widget(gh)->image.width;
		dx = (gh->width-w)/2;
		x += dx;
		if (dx)
			gdispFillArea(gh->x, y, dx, h, gh->bgcolor);
		gdispFillArea(x+w, y, gh->width-dx-w, h, gh->bgcolor);
		dx = 0;
	}

	// Center image horizontally if the area is smaller than the image
	else if (widget(gh)->image.width > w) {
		dx = (widget(gh)->image.width - w)/2;
	}

	// Center vertically if the area is larger than the image
	if (widget(gh)->image.height < h) {
		h = widget(gh)->image.height;
		dy = (gh->height-h)/2;
		y += dy;
		if (dy)
			gdispFillArea(x, gh->y, w, dy, gh->bgcolor);
		gdispFillArea(x, y+h, w, gh->height-dy-h, gh->bgcolor);
		dy = 0;
	}

	// Center image vertically if the area is smaller than the image
	else if (widget(gh)->image.height > h) {
		dy = (widget(gh)->image.height - h)/2;
	}

	// Reset the background color in case it has changed
	gdispImageSetBgColor(&widget(gh)->image, gh->bgcolor);

	// Display the image
	gdispImageDraw(&widget(gh)->image, x, y, w, h, dx, dy);
}


static const gwinVMT imageVMT = {
	"Image",					// The class name
	sizeof(GImageWidget),		// The object size
	_destroy,					// The destroy routine
	_redraw,					// The redraw routine
	0,							// The after-clear routine
};

GHandle gwinImageCreate(GImageWidget *gobj, GWindowInit *pInit) {
	if (!(gobj = (GImageWidget *)_gwindowCreate(&gobj->g, pInit, &imageVMT, 0)))
		return 0;

	// Ensure the gdispImageIsOpen() gives valid results
	gobj->image.type = 0;

	gwinSetVisible((GHandle)gobj, pInit->show);

	return (GHandle)gobj;
}

bool_t gwinImageOpenMemory(GHandle gh, const void* memory) {
	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);

	if (!gdispImageSetMemoryReader(&widget(gh)->image, memory))
		return FALSE;

	if (gdispImageOpen(&widget(gh)->image) != GDISP_IMAGE_ERR_OK)
		return FALSE;

	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		// Setting the clip here shouldn't be necessary if the redraw doesn't overdraw
		//	but we put it in for safety anyway
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		_redraw(gh);
	}
	return TRUE;
}

#if defined(WIN32) || GFX_USE_OS_WIN32 || GFX_USE_OS_POSIX || defined(__DOXYGEN__)
bool_t gwinImageOpenFile(GHandle gh, const char* filename) {
	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);

	if (!gdispImageSetFileReader(&widget(gh)->image, filename))
		return FALSE;

	if (gdispImageOpen(&widget(gh)->image) != GDISP_IMAGE_ERR_OK)
		return FALSE;

	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		// Setting the clip here shouldn't be necessary if the redraw doesn't overdraw
		//	but we put it in for safety anyway
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		_redraw(gh);
	}
	return TRUE;
}
#endif 

#if GFX_USE_OS_CHIBIOS || defined(__DOXYGEN__)
bool_t gwinImageOpenStream(GHandle gh, void *streamPtr) {
	if (gdispImageIsOpen(&widget(gh)->image))
		gdispImageClose(&widget(gh)->image);

	if (!gdispImageSetBaseFileStreamReader(&widget(gh)->image, streamPtr))
		return FALSE;

	if (gdispImageOpen(&widget(gh)->image) != GDISP_IMAGE_ERR_OK)
		return FALSE;

	if ((gh->flags & GWIN_FLG_VISIBLE)) {
		// Setting the clip here shouldn't be necessary if the redraw doesn't overdraw
		//	but we put it in for safety anyway
		#if GDISP_NEED_CLIP
			gdispSetClip(gh->x, gh->y, gh->width, gh->height);
		#endif
		_redraw(gh);
	}
	return TRUE;
}
#endif

gdispImageError gwinImageCache(GHandle gh) {
	return gdispImageCache(&widget(gh)->image);
}

#endif // GFX_USE_GWIN && GWIN_NEED_IMAGE
/** @} */
