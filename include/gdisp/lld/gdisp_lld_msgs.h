/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gdisp/lld/gdisp_lld_msgs.h
 * @brief   GDISP Graphic Driver subsystem low level driver message structures.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_LLD_MSGS_H
#define _GDISP_LLD_MSGS_H

/* This file describes the message API for gdisp_lld */
#if GFX_USE_GDISP && GDISP_NEED_MSGAPI

typedef enum gdisp_msgaction {
	GDISP_LLD_MSG_NOP,
	GDISP_LLD_MSG_INIT,
	GDISP_LLD_MSG_CLEAR,
	GDISP_LLD_MSG_DRAWPIXEL,
	GDISP_LLD_MSG_FILLAREA,
	GDISP_LLD_MSG_BLITAREA,
	GDISP_LLD_MSG_DRAWLINE,
	#if GDISP_NEED_CLIP
		GDISP_LLD_MSG_SETCLIP,
	#endif
	#if GDISP_NEED_CIRCLE
		GDISP_LLD_MSG_DRAWCIRCLE,
		GDISP_LLD_MSG_FILLCIRCLE,
	#endif
	#if GDISP_NEED_ELLIPSE
		GDISP_LLD_MSG_DRAWELLIPSE,
		GDISP_LLD_MSG_FILLELLIPSE,
	#endif
	#if GDISP_NEED_ARC
		GDISP_LLD_MSG_DRAWARC,
		GDISP_LLD_MSG_FILLARC,
	#endif
	#if GDISP_NEED_PIXELREAD
		GDISP_LLD_MSG_GETPIXELCOLOR,
	#endif
	#if GDISP_NEED_SCROLL
		GDISP_LLD_MSG_VERTICALSCROLL,
	#endif
	#if GDISP_NEED_CONTROL
		GDISP_LLD_MSG_CONTROL,
	#endif
	GDISP_LLD_MSG_QUERY,
} gdisp_msgaction_t;

typedef union gdisp_lld_msg {
	struct {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;
	};
	struct gdisp_lld_msg_init {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_INIT
	} init;
	struct gdisp_lld_msg_clear {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_CLEAR
		color_t				color;
	} clear;
	struct gdisp_lld_msg_drawpixel {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_DRAWPIXEL
		coord_t				x, y;
		color_t				color;
	} drawpixel;
	struct gdisp_lld_msg_fillarea {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_FILLAREA
		coord_t				x, y;
		coord_t				cx, cy;
		color_t				color;
	} fillarea;
	struct gdisp_lld_msg_blitarea {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_BLITAREA
		coord_t				x, y;
		coord_t				cx, cy;
		coord_t				srcx, srcy;
		coord_t				srccx;
		const pixel_t		*buffer;
	} blitarea;
	struct gdisp_lld_msg_setclip {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_SETCLIP
		coord_t				x, y;
		coord_t				cx, cy;
	} setclip;
	struct gdisp_lld_msg_drawline {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_DRAWLINE
		coord_t				x0, y0;
		coord_t				x1, y1;
		color_t				color;
	} drawline;
	struct gdisp_lld_msg_drawcircle {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_DRAWCIRCLE
		coord_t				x, y;
		coord_t				radius;
		color_t				color;
	} drawcircle;
	struct gdisp_lld_msg_fillcircle {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_FILLCIRCLE
		coord_t				x, y;
		coord_t				radius;
		color_t				color;
	} fillcircle;
	struct gdisp_lld_msg_drawellipse {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_DRAWELLIPSE
		coord_t				x, y;
		coord_t				a, b;
		color_t				color;
	} drawellipse;
	struct gdisp_lld_msg_fillellipse {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_FILLELLIPSE
		coord_t				x, y;
		coord_t				a, b;
		color_t				color;
	} fillellipse;
	struct gdisp_lld_msg_drawarc {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_DRAWARC
		coord_t				x, y;
		coord_t				radius;
		coord_t				startangle, endangle;
		color_t				color;
	} drawarc;
	struct gdisp_lld_msg_fillarc {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_FILLARC
		coord_t				x, y;
		coord_t				radius;
		coord_t				startangle, endangle;
		color_t				color;
	} fillarc;
	struct gdisp_lld_msg_getpixelcolor {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_GETPIXELCOLOR
		coord_t				x, y;
		color_t				result;
	} getpixelcolor;
	struct gdisp_lld_msg_verticalscroll {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_VERTICALSCROLL
		coord_t				x, y;
		coord_t				cx, cy;
		int					lines;
		color_t				bgcolor;
	} verticalscroll;
	struct gdisp_lld_msg_control {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_CONTROL
		int					what;
		void *				value;
	} control;
	struct gdisp_lld_msg_query {
		gfxQueueItem		qi;
		gdisp_msgaction_t	action;			// GDISP_LLD_MSG_QUERY
		int					what;
		void *				result;
	} query;
} gdisp_lld_msg_t;

#endif	/* GFX_USE_GDISP && GDISP_NEED_MSGAPI */
#endif	/* _GDISP_LLD_MSGS_H */
/** @} */

