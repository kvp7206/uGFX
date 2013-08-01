/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gdisp/gdisp.h
 * @brief   GDISP Graphic Driver subsystem header file.
 *
 * @addtogroup GDISP
 *
 * @brief		Module to interface graphic / pixel oriented displays
 *
 * @details		The GDISP module provides high level abstraction to interface pixel oriented graphic displays.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in gfxconf.h
 *
 * @{
 */

#ifndef _GDISP_H
#define _GDISP_H

#include "gfx.h"

/* This type definition is defined here as it gets used in other gfx sub-systems even
 * if GFX_USE_GDISP is FALSE.
 */

/**
 * @brief   The type for a coordinate or length on the screen.
 */
typedef int16_t	coord_t;

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief   Type for a 2D point on the screen.
 */
typedef struct point_t {
	coord_t		x, y;
	} point;
/**
 * @brief   Type for the text justification.
 */
typedef enum justify {
	justifyLeft = 0,
	justifyCenter = 1,
	justifyRight = 2
} justify_t;
/**
 * @brief   Type for the font metric.
 */
typedef enum fontmetric {fontHeight, fontDescendersHeight, fontLineSpacing, fontCharPadding, fontMinWidth, fontMaxWidth} fontmetric_t;
/**
 * @brief   The type of a font.
 */
typedef const struct mf_font_s* font_t;
/**
 * @brief   Type for the screen orientation.
 */
typedef enum orientation {GDISP_ROTATE_0, GDISP_ROTATE_90, GDISP_ROTATE_180, GDISP_ROTATE_270} gdisp_orientation_t;
/**
 * @brief   Type for the available power modes for the screen.
 */
typedef enum powermode {powerOff, powerSleep, powerDeepSleep, powerOn} gdisp_powermode_t;

/*
 * This is not documented in Doxygen as it is meant to be a black-box.
 * Applications should always use the routines and macros defined
 * below to access it in case the implementation ever changed.
 */
typedef struct GDISPDriver_t {
		coord_t				Width;
		coord_t				Height;
		gdisp_orientation_t	Orientation;
		gdisp_powermode_t	Powermode;
		uint8_t				Backlight;
		uint8_t				Contrast;
		#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
			coord_t				clipx0, clipy0;
			coord_t				clipx1, clipy1;		/* not inclusive */
		#endif
		} GDISPDriver;

extern GDISPDriver	GDISP;

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/

/**
 * @brief   Driver Control Constants
 * @details	Unsupported control codes are ignored.
 * @note	The value parameter should always be typecast to (void *).
 * @note	There are some predefined and some specific to the low level driver.
 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
 * 											that only supports off/on anything other
 * 											than zero is on.
 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
 * 											this value.
 */
#define GDISP_CONTROL_POWER			0
#define GDISP_CONTROL_ORIENTATION	1
#define GDISP_CONTROL_BACKLIGHT		2
#define GDISP_CONTROL_CONTRAST		3
#define GDISP_CONTROL_LLD			1000

/**
 * @brief   Driver Query Constants
 * @details	Unsupported query codes return (void *)-1.
 * @note	The result should be typecast the required type.
 * @note	GDISP_QUERY_LLD				- Low level driver control constants start at
 * 											this value.
 */
#define GDISP_QUERY_LLD				1000

/**
 * @brief   Driver Pixel Format Constants
 */
#define GDISP_PIXELFORMAT_MONO		1
#define GDISP_PIXELFORMAT_RGB565	565
#define GDISP_PIXELFORMAT_RGB888	888
#define GDISP_PIXELFORMAT_RGB444	444
#define GDISP_PIXELFORMAT_RGB332	332
#define GDISP_PIXELFORMAT_RGB666	666
#define GDISP_PIXELFORMAT_CUSTOM	99999
#define GDISP_PIXELFORMAT_ERROR		88888

/**
 * @name   Some basic colors
 * @{
 */
#define White			HTML2COLOR(0xFFFFFF)
#define Black			HTML2COLOR(0x000000)
#define Gray			HTML2COLOR(0x808080)
#define Grey			Gray
#define Blue			HTML2COLOR(0x0000FF)
#define Red				HTML2COLOR(0xFF0000)
#define Fuchsia			HTML2COLOR(0xFF00FF)
#define Magenta			Fuchsia
#define Green			HTML2COLOR(0x008000)
#define Yellow			HTML2COLOR(0xFFFF00)
#define Aqua			HTML2COLOR(0x00FFFF)
#define Cyan			Aqua
#define Lime			HTML2COLOR(0x00FF00)
#define Maroon			HTML2COLOR(0x800000)
#define Navy			HTML2COLOR(0x000080)
#define Olive			HTML2COLOR(0x808000)
#define Purple			HTML2COLOR(0x800080)
#define Silver			HTML2COLOR(0xC0C0C0)
#define Teal			HTML2COLOR(0x008080)
#define Orange			HTML2COLOR(0xFFA500)
#define Pink			HTML2COLOR(0xFFC0CB)
#define SkyBlue			HTML2COLOR(0x87CEEB)
/** @} */

/*===========================================================================*/
/* Defines related to the pixel format										 */
/*===========================================================================*/

#if defined(__DOXYGEN__)
	/**
	 * @brief   The color of a pixel.
	 */
	typedef uint16_t color_t;
	/**
	 * @brief   Convert a number (of any type) to a color_t.
	 * @details Masks any invalid bits in the color
	 */
	#define COLOR(c)			((color_t)(c))
	/**
	 * @brief   Does the color_t type contain invalid bits that need masking.
	 */
	#define MASKCOLOR			FALSE
	/**
	 * @brief   Convert red, green, blue (each 0 to 255) into a color value.
	 */
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3)))
	/**
	 * @brief   Convert a 6 digit HTML code (hex) into a color value.
	 */
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF80000)>>8) | (((h) & 0x00FC00)>>5) | (((h) & 0x0000F8)>>3)))
	/**
	 * @brief   Extract the red component (0 to 255) of a color value.
	 */
	#define RED_OF(c)			(((c) & 0xF800)>>8)
	/**
	 * @brief   Extract the green component (0 to 255) of a color value.
	 */
	#define GREEN_OF(c)			(((c)&0x007E)>>3)
	/**
	 * @brief   Extract the blue component (0 to 255) of a color value.
	 */
	#define BLUE_OF(c)			(((c)&0x001F)<<3)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_MONO
	typedef uint8_t 			color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((r|g|b) ? 1 : 0)
	#define HTML2COLOR(h)		(h ? 1 : 0)
	#define RED_OF(c)			(c ? 255 : 0)
	#define GREEN_OF(c)			(c ? 255 : 0)
	#define BLUE_OF(c)			(c ? 255 : 0)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB565
	typedef uint16_t			color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF8)<<8) | (((g) & 0xFC)<<3) | (((b) & 0xF8)>>3)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF80000)>>8) | (((h) & 0x00FC00)>>5) | (((h) & 0x0000F8)>>3)))
	#define RED_OF(c)			(((c) & 0xF800)>>8)
	#define GREEN_OF(c)			(((c)&0x07E0)>>3)
	#define BLUE_OF(c)			(((c)&0x001F)<<3)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB888
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0xFFFFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFF)<<16) | (((g) & 0xFF) << 8) | ((b) & 0xFF)))
	#define HTML2COLOR(h)		((color_t)(h))
	#define RED_OF(c)			(((c) & 0xFF0000)>>16)
	#define GREEN_OF(c)			(((c)&0x00FF00)>>8)
	#define BLUE_OF(c)			((c)&0x0000FF)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB444
	typedef uint16_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x0FFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xF0)<<4) | ((g) & 0xF0) | (((b) & 0xF0)>>4)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xF00000)>>12) | (((h) & 0x00F000)>>8) | (((h) & 0x0000F0)>>4)))
	#define RED_OF(c)			(((c) & 0x0F00)>>4)
	#define GREEN_OF(c)			((c)&0x00F0)
	#define BLUE_OF(c)			(((c)&0x000F)<<4)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB332
	typedef uint8_t color_t;
	#define COLOR(c)			((color_t)(c))
	#define MASKCOLOR			FALSE
	#define RGB2COLOR(r,g,b)	((color_t)(((r) & 0xE0) | (((g) & 0xE0)>>3) | (((b) & 0xC0)>>6)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xE00000)>>16) | (((h) & 0x00E000)>>11) | (((h) & 0x0000C0)>>6)))
	#define RED_OF(c)			((c) & 0xE0)
	#define GREEN_OF(c)			(((c)&0x1C)<<3)
	#define BLUE_OF(c)			(((c)&0x03)<<6)

#elif GDISP_PIXELFORMAT == GDISP_PIXELFORMAT_RGB666
	typedef uint32_t color_t;
	#define COLOR(c)			((color_t)(((c) & 0x03FFFF)))
	#define MASKCOLOR			TRUE
	#define RGB2COLOR(r,g,b)	((color_t)((((r) & 0xFC)<<10) | (((g) & 0xFC)<<4) | (((b) & 0xFC)>>2)))
	#define HTML2COLOR(h)		((color_t)((((h) & 0xFC0000)>>6) | (((h) & 0x00FC00)>>4) | (((h) & 0x0000FC)>>2)))
	#define RED_OF(c)			(((c) & 0x03F000)>>12)
	#define GREEN_OF(c)			(((c)&0x00FC00)>>8)
	#define BLUE_OF(c)			(((c)&0x00003F)<<2)

#elif GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_CUSTOM
	#error "GDISP: No supported pixel format has been specified."
#endif

/* Verify information for packed pixels and define a non-packed pixel macro */
#if !GDISP_PACKED_PIXELS
	#define gdispPackPixels(buf,cx,x,y,c)	{ ((color_t *)(buf))[(y)*(cx)+(x)] = (c); }
#elif !GDISP_HARDWARE_BITFILLS
	#error "GDISP: packed pixel formats are only supported for hardware accelerated drivers."
#elif GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB888 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB444 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_RGB666 \
		&& GDISP_PIXELFORMAT != GDISP_PIXELFORMAT_CUSTOM
	#error "GDISP: A packed pixel format has been specified for an unsupported pixel format."
#endif

#if GDISP_NEED_SCROLL && !GDISP_HARDWARE_SCROLL
	#error "GDISP: Hardware scrolling is wanted but not supported."
#endif

#if GDISP_NEED_PIXELREAD && !GDISP_HARDWARE_PIXELREAD
	#error "GDISP: Pixel read-back is wanted but not supported."
#endif

/**
 * @brief   The type of a pixel.
 */
typedef color_t		pixel_t;

#ifdef __cplusplus
extern "C" {
#endif

#if GDISP_NEED_MULTITHREAD || GDISP_NEED_ASYNC || defined(__DOXYGEN__)
	/* These routines can be hardware accelerated
	 *	- Do not add a routine here unless it has also been added to the hardware acceleration layer
	 */

	/* Base Functions */

	/**
	 * @brief   Test if the GDISP engine is currently drawing.
	 * @note    This function will always return FALSE if
	 * 			GDISP_NEED_ASYNC is not defined.
	 *
	 * @return	TRUE if gdisp is busy, FALSE otherwise
	 *
	 * @api
	 */
	bool_t gdispIsBusy(void);

	/* Drawing Functions */

	/**
	 * @brief   Clear the display to the specified color.
	 *
	 * @param[in] color The color to use when clearing the screen
	 *
	 * @api
	 */
	void gdispClear(color_t color);

	/**
	 * @brief   Set a pixel in the specified color.
	 *
	 * @param[in] x,y   The position to set the pixel.
	 * @param[in] color The color to use
	 *
	 * @api
	 */
	void gdispDrawPixel(coord_t x, coord_t y, color_t color);

	/**
	 * @brief   Draw a line.
	 *
	 * @param[in] x0,y0		The start position
	 * @param[in] x1,y1 	The end position
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispDrawLine(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

	/**
	 * @brief   Fill an area with a color.
	 *
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispFillArea(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);

	/**
	 * @brief   Fill an area using the supplied bitmap.
	 * @details The bitmap is in the pixel format specified by the low level driver
	 * @note	If a packed pixel format is used and the width doesn't
	 *			match a whole number of bytes, the next line will start on a
	 *			non-byte boundary (no end-of-line padding).
	 * @note	If GDISP_NEED_ASYNC is defined then the buffer must be static
	 * 			or at least retained until this call has finished the blit. You can
	 * 			tell when all graphics drawing is finished by @p gdispIsBusy() going FALSE.
	 *
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the filled area
	 * @param[in] srcx,srcy The bitmap position to start the fill form
	 * @param[in] srccx		The width of a line in the bitmap
	 * @param[in] buffer	The bitmap in the driver's pixel format
	 *
	 * @api
	 */
	void gdispBlitAreaEx(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);

	/* Clipping Functions */

	#if GDISP_NEED_CLIP || defined(__DOXYGEN__)
		/**
		 * @brief   Clip all drawing to the defined area.
		 *
		 * @param[in] x,y     The start position
		 * @param[in] cx,cy   The size of the clip area
		 *
		 * @api
		 */
		void gdispSetClip(coord_t x, coord_t y, coord_t cx, coord_t cy);
	#endif

	/* Circle Functions */

	#if GDISP_NEED_CIRCLE || defined(__DOXYGEN__)
		/**
		 * @brief   Draw a circle.
		 *
		 * @param[in] x,y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 * @param[in] color		The color to use
		 *
		 * @api
		 */
		void gdispDrawCircle(coord_t x, coord_t y, coord_t radius, color_t color);

		/**
		 * @brief   Draw a filled circle.
		 *
		 * @param[in] x,y		The center of the circle
		 * @param[in] radius	The radius of the circle
		 * @param[in] color		The color to use
		 *
		 * @api
		 */
		void gdispFillCircle(coord_t x, coord_t y, coord_t radius, color_t color);
	#endif
	
	/* Ellipse Functions */

	#if GDISP_NEED_ELLIPSE || defined(__DOXYGEN__)
		/**
		 * @brief   Draw an ellipse.
		 *
		 * @param[in] x,y   The center of the ellipse
		 * @param[in] a,b     The dimensions of the ellipse
		 * @param[in] color   The color to use
		 *
		 * @api
		 */
		void gdispDrawEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);

		/**
		 * @brief   Draw a filled ellipse.
		 *
		 * @param[in] x,y   The center of the ellipse
		 * @param[in] a,b     The dimensions of the ellipse
		 * @param[in] color   The color to use
		 *
		 * @api
		 */
		void gdispFillEllipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#endif

	/* Arc Functions */

	#if GDISP_NEED_ARC || defined(__DOXYGEN__)
		/*
		 * @brief	Draw an arc.
		 *
		 * @param[in] x0,y0		The center point
		 * @param[in] radius	The radius of the arc
		 * @param[in] start		The start angle (0 to 360)
		 * @param[in] end		The end angle (0 to 360)
		 * @param[in] color		The color of the arc
		 *
		 * @api
		 */
		void gdispDrawArc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);

		/*
		 * @brief	Draw a filled arc.
		 * @note				Not very efficient currently - does lots of overdrawing
		 *
		 * @param[in] x0,y0		The center point
		 * @param[in] radius	The radius of the arc
		 * @param[in] start		The start angle (0 to 360)
		 * @param[in] end		The end angle (0 to 360)
		 * @param[in] color		The color of the arc
		 *
		 * @api
		 */
		void gdispFillArc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#endif

	/* Read a pixel Function */

	#if GDISP_NEED_PIXELREAD || defined(__DOXYGEN__)
		/**
		 * @brief   Get the color of a pixel.
		 * @return  The color of the pixel.
		 *
		 * @param[in] x,y     The position of the pixel
		 *
		 * @api
		 */
		color_t gdispGetPixelColor(coord_t x, coord_t y);
	#endif

	/* Scrolling Function - clears the area scrolled out */

	#if GDISP_NEED_SCROLL || defined(__DOXYGEN__)
		/**
		 * @brief   Scroll vertically a section of the screen.
		 * @pre		GDISP_NEED_SCROLL must be set to TRUE in gfxconf.h
		 * @note    Optional.
		 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
		 *
		 * @param[in] x, y     The start of the area to be scrolled
		 * @param[in] cx, cy   The size of the area to be scrolled
		 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
		 * @param[in] bgcolor  The color to fill the newly exposed area.
		 *
		 * @api
		 */
		void gdispVerticalScroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#endif

	/* Set driver specific control */

	#if GDISP_NEED_CONTROL || defined(__DOXYGEN__)
		/**
		 * @brief   Control hardware specific parts of the display. eg powermodes, backlight etc
		 * @note    Depending on the hardware implementation this function may not
		 *          support some codes. They will be ignored.
		 *
		 * @param[in] what		what you want to control
		 * @param[in] value		The value to be assigned
		 *
		 * @api
		 */
		void gdispControl(unsigned what, void *value);
	#endif

	/* Query driver specific data */

	#if GDISP_NEED_QUERY || defined(__DOXYGEN__)
		/**
		 * @brief   Query a property of the display.
		 * @note    The result must be typecast to the correct type.
		 * @note    An unsupported query will return (void *)-1.
		 *
		 * @param[in] what		What to query
		 *
		 * @api
		 */
		void *gdispQuery(unsigned what);
	#endif

#else
	/* Include the low level driver information */
	#include "gdisp/lld/gdisp_lld.h"

	/* The same as above but use the low level driver directly if no multi-thread support is needed */
	#define gdispIsBusy()										FALSE
	#define gdispClear(color)									gdisp_lld_clear(color)
	#define gdispDrawPixel(x, y, color)							gdisp_lld_draw_pixel(x, y, color)
	#define gdispDrawLine(x0, y0, x1, y1, color)				gdisp_lld_draw_line(x0, y0, x1, y1, color)
	#define gdispFillArea(x, y, cx, cy, color)					gdisp_lld_fill_area(x, y, cx, cy, color)
	#define gdispBlitAreaEx(x, y, cx, cy, sx, sy, scx, buf)		gdisp_lld_blit_area_ex(x, y, cx, cy, sx, sy, scx, buf)
	#define gdispSetClip(x, y, cx, cy)							gdisp_lld_set_clip(x, y, cx, cy)
	#define gdispDrawCircle(x, y, radius, color)				gdisp_lld_draw_circle(x, y, radius, color)
	#define gdispFillCircle(x, y, radius, color)				gdisp_lld_fill_circle(x, y, radius, color)
	#define gdispDrawArc(x, y, radius, sangle, eangle, color)	gdisp_lld_draw_arc(x, y, radius, sangle, eangle, color)
	#define gdispFillArc(x, y, radius, sangle, eangle, color)	gdisp_lld_fill_arc(x, y, radius, sangle, eangle, color)
	#define gdispDrawEllipse(x, y, a, b, color)					gdisp_lld_draw_ellipse(x, y, a, b, color)
	#define gdispFillEllipse(x, y, a, b, color)					gdisp_lld_fill_ellipse(x, y, a, b, color)
	#define gdispGetPixelColor(x, y)							gdisp_lld_get_pixel_color(x, y)
	#define gdispVerticalScroll(x, y, cx, cy, lines, bgcolor)	gdisp_lld_vertical_scroll(x, y, cx, cy, lines, bgcolor)
	#define gdispControl(what, value)							gdisp_lld_control(what, value)
	#define gdispQuery(what)									gdisp_lld_query(what)

#endif

/* These routines are not hardware accelerated
 *	- Do not add a hardware accelerated routines here.
 */

/* Extra drawing functions */

/**
 * @brief   Draw a rectangular box.
 *
 * @param[in] x,y		The start position
 * @param[in] cx,cy		The size of the box (outside dimensions)
 * @param[in] color		The color to use
 *
 * @api
 */
void gdispDrawBox(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);

#if GDISP_NEED_CONVEX_POLYGON || defined(__DOXYGEN__)
	/**
	 * @brief   Draw an enclosed polygon (convex, non-convex or complex).
	 *
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispDrawPoly(coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color);

	/**
	 * @brief   Fill a convex polygon
	 * @details Doesn't handle non-convex or complex polygons.
	 *
	 * @param[in] tx, ty	Transform all points in pntarray by tx, ty
	 * @param[in] pntarray	An array of points
	 * @param[in] cnt		The number of points in the array
	 * @param[in] color		The color to use
	 *
	 * @note	Convex polygons are those that have no internal angles. That is;
	 * 			you can draw a line from any point on the polygon to any other point
	 * 			on the polygon without it going outside the polygon. In our case we generalise
	 * 			this a little by saying that an infinite horizontal line (at any y value) will cross
	 * 			no more than two edges on the polygon. Some non-convex polygons do fit this criteria
	 * 			and can therefore be drawn.
	 * @note	This routine is designed to be very efficient with even simple display hardware.
	 *
	 * @api
	 */
	void gdispFillConvexPoly(coord_t tx, coord_t ty, const point *pntarray, unsigned cnt, color_t color);
#endif

/* Text Functions */

#if GDISP_NEED_TEXT || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a text character.
	 *
	 * @param[in] x,y		The position for the text
	 * @param[in] c			The character to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispDrawChar(coord_t x, coord_t y, uint16_t c, font_t font, color_t color);

	/**
	 * @brief   Draw a text character with a filled background.
	 *
	 * @param[in] x,y		The position for the text
	 * @param[in] c			The character to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgcolor	The background color to use
	 *
	 * @api
	 */
	void gdispFillChar(coord_t x, coord_t y, uint16_t c, font_t font, color_t color, color_t bgcolor);

	/**
	 * @brief   Draw a text string.
	 *
	 * @param[in] x,y		The position for the text
	 * @param[in] font		The font to use
	 * @param[in] str		The string to draw
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispDrawString(coord_t x, coord_t y, const char *str, font_t font, color_t color);

	/**
	 * @brief   Draw a text string.
	 *
	 * @param[in] x,y		The position for the text
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgcolor	The background color to use
	 *
	 * @api
	 */
	void gdispFillString(coord_t x, coord_t y, const char *str, font_t font, color_t color, color_t bgcolor);

	/**
	 * @brief   Draw a text string vertically centered within the specified box.
	 *
	 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
	 * @param[in] cx,cy		The width and height of the box
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] justify	Justify the text left, center or right within the box
	 *
	 * @api
	 */
	void gdispDrawStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, justify_t justify);

	/**
	 * @brief   Draw a text string vertically centered within the specified box. The box background is filled with the specified background color.
	 * @note    The entire box is filled
	 *
	 * @param[in] x,y		The position for the text (need to define top-right or base-line - check code)
	 * @param[in] cx,cy		The width and height of the box
	 * @param[in] str		The string to draw
	 * @param[in] font		The font to use
	 * @param[in] color		The color to use
	 * @param[in] bgColor	The background color to use
	 * @param[in] justify	Justify the text left, center or right within the box
	 *
	 * @api
	 */
	void gdispFillStringBox(coord_t x, coord_t y, coord_t cx, coord_t cy, const char* str, font_t font, color_t color, color_t bgColor, justify_t justify);

	/**
	 * @brief   Get a metric of a font.
	 * @return  The metric requested in pixels.
	 *
	 * @param[in] font    The font to test
	 * @param[in] metric  The metric to measure
	 *
	 * @api
	 */
	coord_t gdispGetFontMetric(font_t font, fontmetric_t metric);

	/**
	 * @brief   Get the pixel width of a character.
	 * @return  The width of the character in pixels. Does not include any between character padding.
	 *
	 * @param[in] c       The character to draw
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispGetCharWidth(char c, font_t font);

	/**
	 * @brief   Get the pixel width of a string.
	 * @return  The width of the string in pixels.
	 *
	 * @param[in] str     The string to measure
	 * @param[in] font    The font to use
	 *
	 * @api
	 */
	coord_t gdispGetStringWidth(const char* str, font_t font);

	/**
	 * @brief	Find a font and return it.
	 * @details	The supplied name is matched against the font name. A '*' will replace 0 or more characters.
	 * @return	Returns a font or NULL if no matching font could be found.
	 *
	 * @param[in] name		The font name to find.
	 *
	 * @note				Wildcard matching will match the shortest possible match.
	 *
	 * @api
	 */
	font_t gdispOpenFont(const char *name);

	/**
	 * @brief	Release a font after use.
	 *
	 * @param[in] font		The font to release.
	 *
	 * @api
	 */
	void gdispCloseFont(font_t font);

	/**
	 * @brief	Make a scaled copy of an existing font.
	 * @details	Allocates memory for new font metadata using gfxAlloc, remember to close font after use!
	 * @return	A new font or NULL if out of memory.
	 * 
	 * @param[in] font	The base font to use.
	 * @param[in] scale_x	The scale factor in horizontal direction.
	 * @param[in] scale_y	The scale factor in vertical direction.
	 */
	font_t gdispScaleFont(font_t font, uint8_t scale_x, uint8_t scale_y);
	
	/**
	 * @brief	Get the name of the specified font.
	 * @returns	The name of the font.
	 *
	 * @param[in] font		The font to get the name for.
	 *
	 * @api
	 */
	const char *gdispGetFontName(font_t font);
#endif

/* Extra Arc Functions */

#if GDISP_NEED_ARC || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a rectangular box with rounded corners
	 *
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 * @param[in] radius	The radius of the rounded corners
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispDrawRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);

	/**
	 * @brief   Draw a filled rectangular box with rounded corners
	 *
	 * @param[in] x,y		The start position
	 * @param[in] cx,cy		The size of the box (outside dimensions)
	 * @param[in] radius	The radius of the rounded corners
	 * @param[in] color		The color to use
	 *
	 * @api
	 */
	void gdispFillRoundedBox(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t radius, color_t color);
#endif


/**
 * @brief   Blend 2 colors according to the alpha
 * @return	The combined color
 *
 * @param[in] fg		The foreground color
 * @param[in] bg		The background color
 * @param[in] alpha		The alpha value (0-255). 0 is all background, 255 is all foreground.
 *
 * @api
 */
color_t gdispBlendColor(color_t fg, color_t bg, uint8_t alpha);

/* Support routine for packed pixel formats */
#if !defined(gdispPackPixels) || defined(__DOXYGEN__)
	/**
	 * @brief   Pack a pixel into a pixel buffer.
	 * @note    This function performs no buffer boundary checking
	 *			regardless of whether GDISP_NEED_CLIP has been specified.
	 *
	 * @param[in] buf		The buffer to put the pixel in
	 * @param[in] cx		The width of a pixel line
	 * @param[in] x, y		The location of the pixel to place
	 * @param[in] color		The color to put into the buffer
	 *
	 * @api
	 */
	void gdispPackPixels(const pixel_t *buf, coord_t cx, coord_t x, coord_t y, color_t color);
#endif

/* 
 * Macro definitions
 */

/* Now obsolete functions */
#define gdispBlitArea(x, y, cx, cy, buffer)		gdispBlitAreaEx(x, y, cx, cy, 0, 0, cx, buffer)

/* Macro definitions for common gets and sets */

/**
 * @brief   Set the display power mode.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] powerMode		The new power mode
 *
 * @api
 */
#define gdispSetPowerMode(powerMode)			gdispControl(GDISP_CONTROL_POWER, (void *)(unsigned)(powerMode))

/**
 * @brief   Set the display orientation.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] newOrientation		The new orientation
 *
 * @api
 */
#define gdispSetOrientation(newOrientation)		gdispControl(GDISP_CONTROL_ORIENTATION, (void *)(unsigned)(newOrientation))

/**
 * @brief   Set the display backlight.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] percent		The new brightness (0 - 100%)
 *
 * @note	For displays that only support backlight off and on,
 * 			0 = off, anything else = on
 *
 * @api
 */
#define gdispSetBacklight(percent)				gdispControl(GDISP_CONTROL_BACKLIGHT, (void *)(unsigned)(percent))

/**
 * @brief   Set the display contrast.
 * @note    Ignored if not supported by the display.
 *
 * @param[in] percent		The new contrast (0 - 100%)
 *
 * @api
 */
#define gdispSetContrast(percent)				gdispControl(GDISP_CONTROL_CONTRAST, (void *)(unsigned)(percent))

/**
 * @brief   Get the display width in pixels.
 *
 * @api
 */
#define gdispGetWidth()							(GDISP.Width)

/**
 * @brief   Get the display height in pixels.
 *
 * @api
 */
#define gdispGetHeight()						(GDISP.Height)

/**
 * @brief   Get the current display power mode.
 *
 * @api
 */
#define gdispGetPowerMode()						(GDISP.Powermode)

/**
 * @brief   Get the current display orientation.
 *
 * @api
 */
#define gdispGetOrientation()					(GDISP.Orientation)

/**
 * @brief   Get the current display backlight brightness.
 *
 * @api
 */
#define gdispGetBacklight()						(GDISP.Backlight)

/**
 * @brief   Get the current display contrast.
 *
 * @api
 */
#define gdispGetContrast()						(GDISP.Contrast)

/* More interesting macro's */

/**
 * @brief   Reset the clip area to the full screen
 *
 * @api
 */
#define gdispUnsetClip()						gdispSetClip(0,0,gdispGetWidth(),gdispGetHeight())


#ifdef __cplusplus
}
#endif

#if GDISP_NEED_IMAGE || defined(__DOXYGEN__)
	#include "gdisp/image.h"
#endif

#endif /* GFX_USE_GDISP */

#endif /* _GDISP_H */
/** @} */
