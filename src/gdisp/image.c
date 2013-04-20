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
 * @file    src/gdisp/image.c
 * @brief   GDISP generic image code.
 */
#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if GFX_USE_GDISP && GDISP_NEED_IMAGE

/* The structure defining the routines for image drawing */
typedef struct gdispImageHandlers {
	gdispImageError	(*open)(gdispImage *img);			/* The open function */
	void			(*close)(gdispImage *img);			/* The close function */
	gdispImageError	(*cache)(gdispImage *img);			/* The cache function */
	gdispImageError	(*draw)(gdispImage *img,
							coord_t x, coord_t y,
							coord_t cx, coord_t cy,
							coord_t sx, coord_t sy);	/* The draw function */
	systime_t		(*next)(gdispImage *img);			/* The next frame function */
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

#if defined(WIN32)
	#include <fcntl.h>

	static size_t ImageSimulFileRead(struct gdispImageIO *pio, void *buf, size_t len) {
		if (pio->fd == (void *)-1) return 0;
		len = read((int)pio->fd, buf, len);
		if ((int)len < 0) len = 0;
		pio->pos += len;
		return len;
	}

	static void ImageSimulFileSeek(struct gdispImageIO *pio, size_t pos) {
		if (pio->fd == (void *)-1) return;
		if (pio->pos != pos) {
			lseek((int)pio->fd, pos, SEEK_SET);
			pio->pos = pos;
		}
	}

	static void ImageSimulFileClose(struct gdispImageIO *pio) {
		if (pio->fd == (void *)-1) return;
		close((int)pio->fd);
		pio->fd = (void *)-1;
		pio->pos = 0;
	}

	static const gdispImageIOFunctions ImageSimulFileFunctions =
		{ ImageSimulFileRead, ImageSimulFileSeek, ImageSimulFileClose };

	bool_t gdispImageSetSimulFileReader(gdispImage *img, const char *filename) {
		img->io.fns = &ImageSimulFileFunctions;
		img->io.pos = 0;
		img->io.fd = (void *)open(filename, O_RDONLY|O_BINARY);
		return img->io.fd != (void *)-1;
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

systime_t gdispImageNext(gdispImage *img) {
	if (!img->fns) return GDISP_IMAGE_ERR_BADFORMAT;
	return img->fns->next(img);
}

// Helper Routines
void *gdispImageAlloc(gdispImage *img, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		void *ptr;

		ptr = chHeapAlloc(NULL, sz);
		if (ptr) {
			img->memused += sz;
			if (img->memused > img->maxmemused)
				img->maxmemused = img->memused;
		}
		return ptr;
	#else
		(void) img;
		return chHeapAlloc(NULL, sz);
	#endif
}

void gdispImageFree(gdispImage *img, void *ptr, size_t sz) {
	#if GDISP_NEED_IMAGE_ACCOUNTING
		chHeapFree(ptr);
		img->memused -= sz;
	#else
		(void) img;
		(void) sz;
		chHeapFree(ptr);
	#endif
}

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE */
/** @} */
