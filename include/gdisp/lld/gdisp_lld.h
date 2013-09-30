/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/gdisp/lld/gdisp_lld.h
 * @brief   GDISP Graphic Driver subsystem low level driver header.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_H
#define _GDISP_LLD_H

#if GFX_USE_GDISP || defined(__DOXYGEN__)

/*===========================================================================*/
/* Error checks.                                                             */
/*===========================================================================*/

/**
 * @name    GDISP hardware accelerated support
 * @{
 */
	/**
	 * @brief   Hardware accelerated line drawing.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_LINES
		#define GDISP_HARDWARE_LINES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated screen clears.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CLEARS
		#define GDISP_HARDWARE_CLEARS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated rectangular fills.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_FILLS
		#define GDISP_HARDWARE_FILLS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated fills from an image.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_BITFILLS
		#define GDISP_HARDWARE_BITFILLS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CIRCLES
		#define GDISP_HARDWARE_CIRCLES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled circles.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_CIRCLEFILLS
		#define GDISP_HARDWARE_CIRCLEFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ELLIPSES
		#define GDISP_HARDWARE_ELLIPSES			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled ellipses.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ELLIPSEFILLS
		#define GDISP_HARDWARE_ELLIPSEFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated arc's.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ARCS
		#define GDISP_HARDWARE_ARCS			FALSE
	#endif

	/**
	 * @brief   Hardware accelerated filled arcs.
	 * @details If set to @p FALSE software emulation is used.
	 */
	#ifndef GDISP_HARDWARE_ARCFILLS
		#define GDISP_HARDWARE_ARCFILLS		FALSE
	#endif

	/**
	 * @brief   Hardware accelerated scrolling.
	 * @details If set to @p FALSE there is no support for scrolling.
	 */
	#ifndef GDISP_HARDWARE_SCROLL
		#define GDISP_HARDWARE_SCROLL			FALSE
	#endif

	/**
	 * @brief   Reading back of pixel values.
	 * @details If set to @p FALSE there is no support for pixel read-back.
	 */
	#ifndef GDISP_HARDWARE_PIXELREAD
		#define GDISP_HARDWARE_PIXELREAD		FALSE
	#endif

	/**
	 * @brief   The driver supports one or more control commands.
	 * @details If set to @p FALSE there is no support for control commands.
	 */
	#ifndef GDISP_HARDWARE_CONTROL
		#define GDISP_HARDWARE_CONTROL			FALSE
	#endif

	/**
	 * @brief   The driver supports a non-standard query.
	 * @details If set to @p FALSE there is no support for non-standard queries.
	 */
	#ifndef GDISP_HARDWARE_QUERY
		#define GDISP_HARDWARE_QUERY			FALSE
	#endif

	/**
	 * @brief   The driver supports a clipping in hardware.
	 * @details If set to @p FALSE there is no support for non-standard queries.
	 */
	#ifndef GDISP_HARDWARE_CLIP
		#define GDISP_HARDWARE_CLIP			FALSE
	#endif
/** @} */

/**
 * @name    GDISP software algorithm choices
 * @{
 */
/** @} */

/**
 * @name    GDISP pixel format choices
 * @{
 */
	/**
	 * @brief   The native pixel format for this device
	 * @note	Should be set to one of the following:
	 *				GDISP_PIXELFORMAT_RGB565
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB332
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you set GDISP_PIXELFORMAT_CUSTOM you need to also define
	 *				color_t, RGB2COLOR(r,g,b), HTML2COLOR(h),
	 *              RED_OF(c), GREEN_OF(c), BLUE_OF(c),
	 *              COLOR(c) and MASKCOLOR.
	 */
	#ifndef GDISP_PIXELFORMAT
		#define GDISP_PIXELFORMAT	GDISP_PIXELFORMAT_ERROR
	#endif

	/**
	 * @brief   Do pixels require packing for a blit
	 * @note	Is only valid for a pixel format that doesn't fill it's datatype. ie formats:
	 *				GDISP_PIXELFORMAT_RGB888
	 *				GDISP_PIXELFORMAT_RGB444
	 *				GDISP_PIXELFORMAT_RGB666
	 *				GDISP_PIXELFORMAT_CUSTOM
	 * @note	If you use GDISP_PIXELFORMAT_CUSTOM and packed bit fills
	 *				you need to also define @p gdispPackPixels(buf,cx,x,y,c)
	 * @note	If you are using GDISP_HARDWARE_BITFILLS = FALSE then the pixel
	 *				format must not be a packed format as the software blit does
	 *				not support packed pixels
	 * @note	Very few cases should actually require packed pixels as the low
	 *				level driver can also pack on the fly as it is sending it
	 *				to the graphics device.
	 */
	#ifndef GDISP_PACKED_PIXELS
		#define GDISP_PACKED_PIXELS			FALSE
	#endif

	/**
	 * @brief   Do lines of pixels require packing for a blit
	 * @note	Ignored if GDISP_PACKED_PIXELS is FALSE
	 */
	#ifndef GDISP_PACKED_LINES
		#define GDISP_PACKED_LINES			FALSE
	#endif
/** @} */

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/* Core functions */
	extern bool_t gdisp_lld_init(void);

	/* Some of these functions will be implemented in software by the high level driver
	   depending on the GDISP_HARDWARE_XXX macros defined in gdisp_lld_config.h.
	 */

	/* Drawing functions */
	extern void gdisp_lld_clear(color_t color);
	extern void gdisp_lld_draw_pixel(coord_t x, coord_t y, color_t color);
	extern void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color);
	extern void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer);
	extern void gdisp_lld_draw_line(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

	/* Circular Drawing Functions */
	#if GDISP_NEED_CIRCLE
	extern void gdisp_lld_draw_circle(coord_t x, coord_t y, coord_t radius, color_t color);
	extern void gdisp_lld_fill_circle(coord_t x, coord_t y, coord_t radius, color_t color);
	#endif

	#if GDISP_NEED_ELLIPSE
	extern void gdisp_lld_draw_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	extern void gdisp_lld_fill_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color);
	#endif

	/* Arc Drawing Functions */
	#if GDISP_NEED_ARC
	extern void gdisp_lld_draw_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	extern void gdisp_lld_fill_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color);
	#endif

	/* Text Rendering Functions */
	#if GDISP_NEED_TEXT
	extern void gdisp_lld_draw_char(coord_t x, coord_t y, uint16_t c, font_t font, color_t color);
	extern void gdisp_lld_fill_char(coord_t x, coord_t y, uint16_t c, font_t font, color_t color, color_t bgcolor);
	#endif

	/* Pixel readback */
	#if GDISP_NEED_PIXELREAD
	extern color_t gdisp_lld_get_pixel_color(coord_t x, coord_t y);
	#endif

	/* Scrolling Function - clears the area scrolled out */
	#if GDISP_NEED_SCROLL
	extern void gdisp_lld_vertical_scroll(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor);
	#endif

	/* Set driver specific control */
	#if GDISP_NEED_CONTROL
	extern void gdisp_lld_control(unsigned what, void *value);
	#endif

	/* Query driver specific data */
	#if GDISP_NEED_QUERY
	extern void *gdisp_lld_query(unsigned what);
	#endif

	/* Clipping Functions */
	#if GDISP_NEED_CLIP
	extern void gdisp_lld_set_clip(coord_t x, coord_t y, coord_t cx, coord_t cy);
	#endif

	/* Messaging API */
	#if GDISP_NEED_MSGAPI
	#include "gdisp_lld_msgs.h"
	extern void gdisp_lld_msg_dispatch(gdisp_lld_msg_t *msg);
	#endif

#ifdef __cplusplus
}
#endif

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_H */
/** @} */

