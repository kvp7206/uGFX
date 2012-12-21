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
 * @defgroup Graph Graph
 * @ingroup GWIN
 *
 * @details	GWIN allows it to easily draw graphs.
 * @pre		GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre		GWIN_NEED_GRAPH must be set to TRUE in your gfxconf.h
 *
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
		#define GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS	0x0001
		#define GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS	0x0002
		#define GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS	0x0004
		#define GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS	0x0008
		#define GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS	(GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_NEGATIVE_AXIS_ARROWS	(GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_XAXIS_ARROWS			(GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_YAXIS_ARROWS			(GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS|GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)
		#define GWIN_GRAPH_STYLE_ALL_AXIS_ARROWS		(GWIN_GRAPH_STYLE_XAXIS_ARROWS|GWIN_GRAPH_STYLE_YAXIS_ARROWS)
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

/**
 * @brief   Create a graph window.
 * @return  NULL if there is no resultant drawing area, otherwise a window handle.
 *
 * @param[in] gg		The GGraphObject structure to initialise. If this is NULL the structure is dynamically allocated.
 * @param[in] x,y		The screen co-ordinates for the bottom left corner of the window
 * @param[in] width		The width of the window
 * @param[in] height	The height of the window
 * @note				The console is not automatically cleared on creation. You must do that by calling gwinClear() (possibly after changing your background color)
 * @note				The coordinate system within the window for graphing operations (but not for any other drawing
 * 						operation) is relative to the bottom left corner and then shifted right and up by the specified
 * 						graphing x and y origin. Note that this system is inverted in the y direction relative to the display.
 * 						This gives the best graphing arrangement ie. increasing y values are closer to the top of the display.
 *
 * @api
 */
GHandle gwinCreateGraph(GGraphObject *gg, coord_t x, coord_t y, coord_t width, coord_t height);

/**
 * @brief   Set the style of the graphing operations.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] pstyle	The graph style to set.
 * @note				The graph is not automatically redrawn. The new style will apply to any new drawing operations.
 *
 * @api
 */
void gwinGraphSetStyle(GHandle gh, const GGraphStyle *pstyle);

/**
 * @brief   Set the origin for graphing operations.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] x, y		The new origin for the graph (in graph coordinates relative to the bottom left corner).
 * @note				The graph is not automatically redrawn. The new origin will apply to any new drawing operations.
 *
 * @api
 */
void gwinGraphSetOrigin(GHandle gh, coord_t x, coord_t y);

/**
 * @brief   Draw the axis and the background grid.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @note				The graph is not automatically cleared. You must do that first by calling gwinClear().
 *
 * @api
 */
void gwinGraphDrawAxis(GHandle gh);

/**
 * @brief   Start a new set of graphing data.
 * @details	This prevents a line being drawn from the last data point to the next point to be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 *
 * @api
 */
void gwinGraphStartSet(GHandle gh);

/**
 * @brief   Draw a graph point.
 * @details	A graph point and a line connecting to the previous point will be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] x, y		The new point for the graph.
 *
 * @api
 */
void gwinGraphDrawPoint(GHandle gh, coord_t x, coord_t y);

/**
 * @brief   Draw multiple graph points.
 * @details	A graph point and a line connecting to each previous point will be drawn.
 *
 * @param[in] gh		The window handle (must be a graph window)
 * @param[in] points	The array of points for the graph.
 * @param[in] count		The number of points in the array.
 * @note				This is slightly more efficient than calling gwinGraphDrawPoint() repeatedly.
 *
 * @api
 */
void gwinGraphDrawPoints(GHandle gh, const GGraphPoint *points, unsigned count);

#ifdef __cplusplus
}
#endif

#endif	/* GWIN_NEED_GRAPH */

#endif	/* _GWIN_GRAPH_H */
/** @} */

