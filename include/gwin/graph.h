/*
    ChibiOS/GFX - Copyright (C) 2012
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
 * @file	include/gwin/graph.h
 * @brief	GWIN GRAPH module header file.
 *
 * @addtogroup GWIN_GRAPH
 * @{
 */

#ifndef _GWIN_GRAPH_H
#define _GWIN_GRAPH_H

#if GWIN_NEED_GRAPH || defined(__DOXYGEN__)

/*===========================================================================*/
/* Driver constants.														 */
/*===========================================================================*/

#define GW_GRAPH				0x0003

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct GGraphPoint_t {
	coord_t		x, y;
	} GGraphPoint;

typedef enum GGraphPointType_e {
	GGRAPH_POINT_NONE, GGRAPH_POINT_DOT, GGRAPH_POINT_SQUARE, GGRAPH_POINT_CIRCLE
	} GGraphPointType;

typedef struct GGraphPointStyle_t {
	GGraphPointType		type;
	coord_t				size;
	color_t				color;
	} GGraphPointStyle;

typedef enum GGraphLineType_e {
	GGRAPH_LINE_NONE, GGRAPH_LINE_SOLID, GGRAPH_LINE_DOT, GGRAPH_LINE_DASH
	} GGraphLineType;

typedef struct GGraphLineStyle_t {
	GGraphLineType		type;
	coord_t				size;
	color_t				color;
	} GGraphLineStyle;

typedef struct GGraphGridStyle_t {
	GGraphLineType		type;
	coord_t				size;
	color_t				color;
	coord_t				spacing;
	} GGraphGridStyle;

typedef struct GGraphStyle_t {
	GGraphPointStyle	point;
	GGraphLineStyle		line;
	GGraphLineStyle		xaxis;
	GGraphLineStyle		yaxis;
	GGraphGridStyle		xgrid;
	GGraphGridStyle		ygrid;
	uint16_t			flags;
		#define GWIN_GRAPH_STYLE_XAXIS_ARROWS	0x0001
		#define GWIN_GRAPH_STYLE_YAXIS_ARROWS	0x0002
} GGraphStyle;

// A graph window
typedef struct GGraphObject_t {
	GWindowObject		gwin;
	GGraphStyle			style;
	coord_t				xorigin, yorigin;
	coord_t				lastx, lasty;
	} GGraphObject;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	GHandle gwinCreateGraph(GGraphObject *gg, coord_t x, coord_t y, coord_t width, coord_t height);
	void gwinGraphSetStyle(GHandle gh, const GGraphStyle *pstyle);
	void gwinGraphSetOrigin(GHandle gh, coord_t x, coord_t y);
	void gwinGraphDrawAxis(GHandle gh);
	void gwinGraphStartSet(GHandle gh);
	void gwinGraphDrawPoint(GHandle gh, coord_t x, coord_t y);
	void gwinGraphDrawPoints(GHandle gh, const GGraphPoint *points, unsigned count);

#ifdef __cplusplus
}
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

typedef struct _Graph {
	coord_t origin_x;
	coord_t origin_y;
	coord_t xmin;
	coord_t xmax;
	coord_t ymin;
	coord_t ymax;
	uint16_t grid_size;
	uint16_t dot_space;
	bool_t full_grid;
	bool_t arrows;
	color_t axis_color;
	color_t grid_color;

	/* do never modify values below this line manually */
	coord_t x0;
	coord_t x1;
	coord_t y0;
	coord_t y1;
} Graph;

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

#ifdef __cplusplus
}
#endif

#endif	/* GWIN_NEED_GRAPH */

#endif	/* _GWIN_GRAPH_H */
/** @} */

