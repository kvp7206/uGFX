/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gdisp/image.h
 * @brief   GDISP image header file.
 *
 * @defgroup Image Image
 * @ingroup GDISP
 * @{
 */

#ifndef _GDISP_IMAGE_H
#define _GDISP_IMAGE_H
#if (GFX_USE_GDISP && GDISP_NEED_IMAGE) || defined(__DOXYGEN__)

/**
 * @brief	The type of image
 */
typedef uint16_t	gdispImageType;
	#define GDISP_IMAGE_TYPE_UNKNOWN	0
	#define GDISP_IMAGE_TYPE_NATIVE		1
	#define GDISP_IMAGE_TYPE_GIF		2
	#define GDISP_IMAGE_TYPE_BMP		3
	#define GDISP_IMAGE_TYPE_JPG		4
	#define GDISP_IMAGE_TYPE_PNG		5

/**
 * @brief	An image error code
 */
typedef uint16_t	gdispImageError;
	#define GDISP_IMAGE_ERR_OK					0
	#define GDISP_IMAGE_ERR_UNRECOVERABLE		0x8000
	#define GDISP_IMAGE_ERR_BADFORMAT			(GDISP_IMAGE_ERR_UNRECOVERABLE+1)
	#define GDISP_IMAGE_ERR_BADDATA				(GDISP_IMAGE_ERR_UNRECOVERABLE+2)
	#define GDISP_IMAGE_ERR_UNSUPPORTED			(GDISP_IMAGE_ERR_UNRECOVERABLE+3)
	#define GDISP_IMAGE_ERR_UNSUPPORTED_OK		3
	#define GDISP_IMAGE_ERR_NOMEMORY			(GDISP_IMAGE_ERR_UNRECOVERABLE+4)

/**
 * @brief	Image flags
 */
typedef uint16_t	gdispImageFlags;
	#define GDISP_IMAGE_FLG_TRANSPARENT			0x0001	/* The image has transparency */
	#define GDISP_IMAGE_FLG_ANIMATED			0x0002	/* The image has animation */
	#define GDISP_IMAGE_FLG_MULTIPAGE			0x0004	/* The image has multiple pages */

struct gdispImageIO;

/**
 * @brief	An image IO close function
 *
 * @param[in] pio	Pointer to the io structure
 * @param[in] desc	The descriptor. A filename or an image structure pointer.
 *
 */
typedef void (*gdispImageIOCloseFn)(struct gdispImageIO *pio);

/**
 * @brief	An image IO read function
 * @returns	The number of bytes actually read or 0 on error
 *
 * @param[in] pio	Pointer to the io structure
 * @param[in] buf	Where the results should be placed
 * @param[in] len	The number of bytes to read
 *
 */
typedef size_t (*gdispImageIOReadFn)(struct gdispImageIO *pio, void *buf, size_t len);

/**
 * @brief	An image IO seek function
 *
 * @param[in] pio	Pointer to the io structure
 * @param[in] pos	Which byte to seek to relative to the start of the "file".
 *
 */
typedef void (*gdispImageIOSeekFn)(struct gdispImageIO *pio, size_t pos);

typedef struct gdispImageIOFunctions {
	gdispImageIOReadFn			read;				/* @< The function to read input */
	gdispImageIOSeekFn			seek;				/* @< The function to seek input */
	gdispImageIOCloseFn			close;				/* @< The function to close input */
	} gdispImageIOFunctions;

/**
 * @brief	The structure defining the IO routines for image handling
 */
typedef struct gdispImageIO {
	const void *				fd;					/* @< The "file" descriptor */
	size_t						pos;				/* @< The current "file" position */
	const gdispImageIOFunctions	*fns;				/* @< The current "file" functions */
} gdispImageIO;

/**
 * @brief	The structure for an image
 */
typedef struct gdispImage {
	gdispImageType						type;				/* @< The image type */
	gdispImageFlags						flags;				/* @< The image flags */
	color_t								bgcolor;			/* @< The default background color */
	coord_t								width, height;		/* @< The image dimensions */
	gdispImageIO						io;					/* @< The image IO functions */
	#if GDISP_NEED_IMAGE_ACCOUNTING
		uint32_t							memused;			/* @< How much RAM is currently allocated */
		uint32_t							maxmemused;			/* @< How much RAM has been allocated (maximum) */
	#endif
	const struct gdispImageHandlers *	fns;				/* @< Don't mess with this! */
	struct gdispImagePrivate *			priv;				/* @< Don't mess with this! */
} gdispImage;
	
#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief	Sets the io fields in the image structure to routines
	 * 			that support reading from an image stored in RAM or Flash.
	 * 			
	 * @return	TRUE if the IO open function succeeds
	 *
	 * @param[in] img   	The image structure
	 * @param[in] memimage	A pointer to the image in RAM or Flash 
	 *
	 * @note	Always returns TRUE for a Memory Reader
	 */
	bool_t gdispImageSetMemoryReader(gdispImage *img, const void *memimage);

	#if GFX_USE_OS_CHIBIOS || defined(__DOXYGEN__)
		/**
		 * @brief	Sets the io fields in the image structure to routines
		 * 			that support reading from an image stored on a BaseFileStream (eg SDCard).
		 *
		 * @return	TRUE if the IO open function succeeds
		 *
		 * @param[in] img   			The image structure
		 * @param[in] BaseFileStreamPtr	A pointer to the (open) BaseFileStream object.
		 *
		 */
		bool_t gdispImageSetBaseFileStreamReader(gdispImage *img, void *BaseFileStreamPtr);
	#endif

	#if defined(WIN32) || GFX_USE_OS_WIN32 || GFX_USE_OS_LINUX || GFX_USE_OS_OSX || defined(__DOXYGEN__)
		/**
		 * @brief	Sets the io fields in the image structure to routines
		 * 			that support reading from an image stored in Win32 simulators native
		 * 			file system.
		 * @pre		Only available on the Win32 simulator
		 *
		 * @return	TRUE if the IO open function succeeds
		 *
		 * @param[in] img   	The image structure
		 * @param[in] filename	The filename to open
		 *
		 */
		bool_t gdispImageSetFileReader(gdispImage *img, const char *filename);
		/* Old definition */
		#define gdispImageSetSimulFileReader(img, fname)	gdispImageSetFileReader(img, fname)
	#endif
	
	/**
	 * @brief	Open an image ready for drawing
	 * @details	Determine the image format and get ready to decode the first image frame
	 * @return	GDISP_IMAGE_ERR_OK (0) on success or an error code.
	 * 
	 * @param[in] img   The image structure
	 * 
	 * @pre		The io fields should be filled in before calling gdispImageOpen()
	 * 
	 * @note	This determines which decoder to use and then initialises all other fields
	 * 			in the gdispImage structure.
	 * @note	The image background color is set to White.
	 * @note	There are three types of return - everything OK, partial success and unrecoverable
	 * 			failures. For everything OK it returns GDISP_IMAGE_ERR_OK. A partial success can
	 * 			be distinguished from a unrecoverable failure by testing the GDISP_IMAGE_ERR_UNRECOVERABLE
	 * 			bit in the error code.
	 * 			A partial success return code means an image can still be drawn but perhaps with
	 * 			reduced functionality eg only the first page of a multi-page image.
	 * @note	@p gdispImageClose() can be called even after a failure to open the image to ensure
	 * 			that the IO close routine gets called.
	 */
	gdispImageError gdispImageOpen(gdispImage *img);
	
	/**
	 * @brief	Close an image and release any dynamically allocated working storage.
	 * 
	 * @param[in] img   The image structure
	 * 
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	Also calls the IO close function (if it hasn't already been called).
	 */
	void gdispImageClose(gdispImage *img);

	/**
	 * @brief	Is an image open.
	 * @return	TRUE if the image is currently open.
	 *
	 * @param[in] img   The image structure
	 *
	 * @note	Be careful with calling this on an uninitialized image structure as the image
	 * 			will contain random data which may be interpreted as meaning the image
	 * 			is open. Clearing the Image structure to 0's will guarantee the image
	 * 			is seen as being closed.
	 */
	bool_t gdispImageIsOpen(gdispImage *img);

	/**
	 * @brief	Set the background color of the image.
	 *
	 * @param[in] img   	The image structure
	 * @param[in] bgcolor	The background color to use
	 *
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	This color is only used when an image has to restore part of the background before
	 * 			continuing with drawing that includes transparency eg some GIF animations.
	 */
	void gdispImageSetBgColor(gdispImage *img, color_t bgcolor);
	
	/**
	 * @brief	Cache the image
	 * @details	Decodes and caches the current frame into RAM.
	 * @return	GDISP_IMAGE_ERR_OK (0) on success or an error code.
	 *
	 * @param[in] img   The image structure
	 *
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	This can use a LOT of RAM!
	 * @note	The decoder may choose to ignore the request for caching. If it does so it will
	 * 			return GDISP_IMAGE_ERR_UNSUPPORTED_OK.
	 * @note	A fatal error here does not necessarily mean that drawing the image will fail. For
	 * 			example, a GDISP_IMAGE_ERR_NOMEMORY error simply means there isn't enough RAM to
	 * 			cache the image.
	 */
	gdispImageError gdispImageCache(gdispImage *img);

	/**
	 * @brief	Draw the image
	 * @return	GDISP_IMAGE_ERR_OK (0) on success or an error code.
	 * 
	 * @param[in] img   The image structure
	 * @param[in] x,y	The screen location to draw the image
	 * @param[in] cx,cy	The area on the screen to draw
	 * @param[in] sx,sy	The image position to start drawing at
	 * 
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	If sx,sy + cx,cy is outside the image boundaries the area outside the image
	 * 			is simply not drawn.
	 * @note	If @p gdispImageCache() has been called first for this frame, this routine will draw using a
	 * 			fast blit from the cached frame. If not, it reads the input and decodes it as it
	 * 			is drawing. This may be significantly slower than if the image has been cached (but
	 * 			uses a lot less RAM)
	 */
	gdispImageError gdispImageDraw(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);

	/**
	 * @brief	Prepare for the next frame/page in the image file.
	 * @return	A time in milliseconds to keep displaying the current frame before trying to draw
	 * 			the next frame. Watch out for the special values TIME_IMMEDIATE and TIME_INFINITE.
	 * 
	 * @param[in] img   The image structure
	 * 
	 * @pre		gdispImageOpen() must have returned successfully.
	 *
	 * @note	It will return TIME_IMMEDIATE if the first frame/page hasn't been drawn or if the next frame
	 * 			should be drawn immediately.
	 * @note	It will return TIME_INFINITE if another image frame doesn't exist or an error has occurred.
	 * @note	Images that support multiple pages (eg TIFF files) will return TIME_IMMEDIATE between pages
	 * 			and then TIME_INFINITE when there are no more pages.
	 * @note	An image that displays a looped animation will never return TIME_INFINITE unless it
	 * 			gets an error.
	 * @note	Calling gdispImageDraw() after getting a TIME_INFINITE will go back to drawing the first
	 * 			frame/page.
	 */
	delaytime_t gdispImageNext(gdispImage *img);
	
	#if GDISP_NEED_IMAGE_NATIVE
		/**
		 * @brief	The image drawing routines for a NATIVE format image.
		 * 
		 * @note	Only use these functions if you absolutely know the format
		 * 			of the image you are decoding. Generally you should use the
		 * 			generic functions and it will auto-detect the format.
		 * @note	A NATIVE format image is defined as an 8 byte header described below, immediately
		 * 			followed by the bitmap data. The bitmap data is stored in the native format for
		 * 			the display controller. If the pixel format specified in the header does not
		 * 			match the controller native format then the image is rejected.
		 * @note	The 8 byte header:
		 *  			{ 'N', 'I', width.hi, width.lo, height.hi, height.lo, format.hi, format.lo }
		 *  			The format word = GDISP_PIXELFORMAT
		 * @{
		 */
		gdispImageError gdispImageOpen_NATIVE(gdispImage *img);
		void gdispImageClose_NATIVE(gdispImage *img);
		gdispImageError gdispImageCache_NATIVE(gdispImage *img);
		gdispImageError gdispImageDraw_NATIVE(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
		delaytime_t gdispImageNext_NATIVE(gdispImage *img);
		/* @} */
	#endif

	#if GDISP_NEED_IMAGE_GIF
		/**
		 * @brief	The image drawing routines for a GIF image.
		 * @note	Only use these functions if you absolutely know the format
		 * 			of the image you are decoding. Generally you should use the
		 * 			generic functions and it will auto-detect the format.
		 * @{
		 */
		gdispImageError gdispImageOpen_GIF(gdispImage *img);
		void gdispImageClose_GIF(gdispImage *img);
		gdispImageError gdispImageCache_GIF(gdispImage *img);
		gdispImageError gdispImageDraw_GIF(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
		delaytime_t gdispImageNext_GIF(gdispImage *img);
		/* @} */
	#endif

	#if GDISP_NEED_IMAGE_BMP
		/**
		 * @brief	The image drawing routines for a BMP image.
		 * @note	Only use these functions if you absolutely know the format
		 * 			of the image you are decoding. Generally you should use the
		 * 			generic functions and it will auto-detect the format.
		 * @{
		 */
		gdispImageError gdispImageOpen_BMP(gdispImage *img);
		void gdispImageClose_BMP(gdispImage *img);
		gdispImageError gdispImageCache_BMP(gdispImage *img);
		gdispImageError gdispImageDraw_BMP(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
		delaytime_t gdispImageNext_BMP(gdispImage *img);
		/* @} */
	#endif
	
	#if GDISP_NEED_IMAGE_JPG
		/**
		 * @brief	The image drawing routines for a JPG image.
		 * @note	Only use these functions if you absolutely know the format
		 * 			of the image you are decoding. Generally you should use the
		 * 			generic functions and it will auto-detect the format.
		 * @{
		 */
		gdispImageError gdispImageOpen_JPG(gdispImage *img);
		void gdispImageClose_JPG(gdispImage *img);
		gdispImageError gdispImageCache_JPG(gdispImage *img);
		gdispImageError gdispImageDraw_JPG(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
		delaytime_t gdispImageNext_JPG(gdispImage *img);
		/* @} */
	#endif

	#if GDISP_NEED_IMAGE_PNG
		/**
		 * @brief	The image drawing routines for a PNG image.
		 * @note	Only use these functions if you absolutely know the format
		 * 			of the image you are decoding. Generally you should use the
		 * 			generic functions and it will auto-detect the format.
		 * @{
		 */
		gdispImageError gdispImageOpen_PNG(gdispImage *img);
		void gdispImageClose_PNG(gdispImage *img);
		gdispImageError gdispImageCache_PNG(gdispImage *img);
		gdispImageError gdispImageDraw_PNG(gdispImage *img, coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t sx, coord_t sy);
		delaytime_t gdispImageNext_PNG(gdispImage *img);
		/* @} */
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GDISP && GDISP_NEED_IMAGE */
#endif /* _GDISP_IMAGE_H */
/** @} */

