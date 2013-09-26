/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gdisp/image.c
 * @brief   GDISP generic image code.
 *
 * @defgroup Image Image
 * @ingroup GDISP
 */
#include "gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE

#include <string.h>

/* The structure defining the routines for image drawing */
typedef struct gdispImageHandlers {
	gdispImageError	(*open)(gdispImage *img);			/* The open function */
	void			(*close)(gdispImage *img);			/* The close function */
	gdispImageError	(*cache)(gdispImage *img);			/* The cache function */
	gdispImageError	(*draw)(gdispImage *img,
							coord_t x, coord_t y,
							coord_t cx, coord_t cy,
							coord_t sx, coord_t sy);	/* The draw function */
	delaytime_t		(*next)(gdispImage *img);			/* The next frame function */
} gdispImageHandlers;

static gdispImageHandlers ImageHandlers[] = {
	#if GDISP_NEED_IMAGE_NATIVE
		{	gdispImageOpen_NATIVE,	gdispImageClose_NATIVE,
			gdispImageCache_NATIVE,	gdispImageDraw_NATIVE,	gdispImageNext_NATIVE,
		},
	#endif
	#if GDISP_NEED_IMAGE_GIF
		{	gdispImageOpen_GIF,		gdispImageClose_GIF,
			gdispImageCache_GIF,	gdispImageDraw_GIF,		gdispImageNext_GIF,
		},
	#endif
	#if GDISP_NEED_IMAGE_BMP
		{	gdispImageOpen_BMP,		gdispImageClose_BMP,
			gdispImageCache_BMP,	gdispImageDraw_BMP,		gdispImageNext_BMP,
		},
	#endif
	#if GDISP_NEED_IMAGE_JPG
		{	gdispImageOpen_JPG,		gdispImageClose_JPG,
			gdispImageCache_JPG,	gdispImageDraw_JPG,		gdispImageNext_JPG,
		},
	#endif
	#if GDISP_NEED_IMAGE_PNG
		{	gdispImageOpen_PNG,		gdispImageClose_PNG,
			gdispImageCache_PNG,	gdispImageDraw_PNG,		gdispImageNext_PNG,
		},
	#endif
};

static size_t ImageMemoryRead(struct gdispImageIO *pio, void *buf, size_t len) {
	if (pio->fd == (void *)-1) return 0;
	memcpy(buf, ((const char *)pio->fd)+pio->pos, len);
	pio->pos += len;
	return len;
}

static void ImageMemorySeek(struct gdispImageIO *pio, size_t pos) {
	if (pio->fd == (void *)-1) return;
	pio->pos = pos;
}

static void ImageMemoryClose(struct gdispImageIO *pio) {
	pio->fd = (void *)-1;
	pio->pos = 0;
}

static const gdispImageIOFunctions ImageMemoryFunctions =
	{ ImageMemoryRead, ImageMemorySeek, ImageMemoryClose };

bool_t gdispImageSetMemoryReader(gdispImage *img, const void *memimage) {
	img->io.fns = &ImageMemoryFunctions;
	img->io.pos = 0;
	img->io.fd = memimage;
	return TRUE;
}

#if GFX_USE_OS_CHIBIOS
	static size_t ImageBaseFileStreamRead(struct gdispImageIO *pio, void *buf, size_t len) {
		if (pio->fd == (void *)-1) return 0;
		len = chSequentialStreamRead(((BaseFileStream *)pio->fd), (uint8_t *)buf, len);
		pio->pos += len;
		return len;
	}

	static void ImageBaseFileStreamSeek(struct gdispImageIO *pio, size_t pos) {
		if (pio->fd == (void *)-1) return;
		if (pio->pos != pos) {
			chFileStreamSeek(((BaseFileStream *)pio->fd), pos);
			pio->pos = pos;
		}
	}

	static void ImageBaseFileStreamClose(struct gdispImageIO *pio) {
		if (pio->fd == (void *)-1) return;
		chFileStreamClose(((BaseFileStream *)pio->fd));
		pio->fd = (void *)-1;
		pio->pos = 0;
	}

	static const gdispImageIOFunctions ImageBaseFileStreamFunctions =
		{ ImageBaseFileStreamRead, ImageBaseFileStreamSeek, ImageBaseFileStreamClose };

	bool_t gdispImageSetBaseFileStreamReader(gdispImage *img, void *BaseFileStreamPtr) {
		img->io.fns = &ImageBaseFileStreamFunctions;
		img->io.pos = 0;
		img->io.fd = BaseFileStreamPtr;
		return TRUE;
	}
#endif

#if defined(WIN32) || GFX_USE_OS_WIN32 || GFX_USE_OS_LINUX || GFX_USE_OS_OSX
	#include <stdio.h>

	static size_t ImageFileRead(struct gdispImageIO *pio, void *buf, size_t len) {
		if (!pio->fd) return 0;
		len = fread(buf, 1, len, (FILE *)pio->fd);
		if ((int)len < 0) len = 0;
		pio->pos += len;
		return len;
	}

	static void ImageFileSeek(struct gdispImageIO *pio, size_t pos) {
		if (!pio->fd) return;
		if (pio->pos != pos) {
			fseek((FILE *)pio->fd, pos, SEEK_SET);
			pio->pos = pos;
		}
	}

	static void ImageFileClose(struct gdispImageIO *pio) {
		if (!pio->fd) return;
		fclose((FILE *)pio->fd);
		pio->fd = 0;
		pio->pos = 0;
	}

	static const gdispImageIOFunctions ImageFileFunctions =
		{ ImageFileRead, ImageFileSeek, ImageFileClose };

	bool_t gdispImageSetFileReader(gdispImage *img, const char *filename) {
		img->io.fns = &ImageFileFunctions;
		img->io.pos = 0;
		#if defined(WIN32) || GFX_USE_OS_WIN32
			img->io.fd = (void *)fopen(filename, "rb");
		#else
			img->io.fd = (void *)fopen(filename, "r");
		#endif

		return img->io.fd != 0;
	}
#endif

gdispImageError gdispImageOpen(gdispImage *img) {
	gdispImageError err;

	img->bgcolor = White;
	for(img->fns = ImageHandlers; img->fns < ImageHandlers+sizeof(ImageHandlers)/sizeof(ImageHandlers[0]); img->fns++) {
		err = img->fns->open(img);
		if (err != GDISP_IMAGE_ERR_BADFORMAT) {
			if ((err & GDISP_IMAGE_ERR_UNRECOVERABLE))
				img->fns = 0;
			return err;
		}
		img->io.fns->seek(&img->io, 0);
	}
	img->type = GDISP_IMAGE_TYPE_UNKNOWN;
	img->flags = 0;
	img->fns = 0;
	img->priv = 0;
	return GDISP_IMAGE_ERR_BADFORMAT;
}

void gdispImageClose(gdispImage *img) {
	if (img->fns)
		img->fns->close(img);
	else
		img->io.fns->close(&img->io);
	img->type = GDISP_IMAGE_TYPE_UNKNOWN;
	img->flags = 0;
	img->fns = 0;
	img->priv = 0;
}

bool_t gdispImageIsOpen(gdispImage *img) {
	return img->type != GDISP_IMAGE_TYPE_UNKNOWN && img->fns != 0;
}

void gdispImageSetBgColor(gdispImage *img, color_t bgcolor) {
	img->bgcolor = bgcolor;
}

gdispImageError gdispImageCache(gdispImage *img) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->cache(img);
}

gdispImageError gdispImageDraw(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->draw(img, x, y, cx, cy, sx, sy);
}

delaytime_t gdispImageNext(gdispImage *img) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->next(img);
}

// Helper Routines
void *gdispImageAlloc(gdispImage *img, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		void *ptr;

		ptr = gfxAlloc(sz);
		if (ptr) {
			img->memused += sz;
			if (img->memused > img->maxmemused)
				img->maxmemused = img->memused;
		}
		return ptr;
	#else
		(void) img;
		return gfxAlloc(sz);
	#endif
}

void gdispImageFree(gdispImage *img, void *ptr, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		gfxFree(ptr);
		img->memused -= sz;
	#else
		(void) img;
		(void) sz;
		gfxFree(ptr);
	#endif
}

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE */
/** @} */
