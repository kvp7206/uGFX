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
 * @file	include/graph.h
 * @brief	GRAPH module header file.
 *
 * @addtogroup GRAPH
 * @{
 */

#ifndef GRAPH_H
#define GRAPH_H

#ifndef GFX_USE_GRAPH
	#define GFX_USE_GRAPH FALSE
#endif

#if GFX_USE_GRAPH || defined(__DOXYGEN__)

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

void graphDrawSystem(Graph *g);
void graphDrawDot(Graph *g, coord_t x, coord_t y, uint16_t radius, color_t color);
void graphDrawDots(Graph *g, int coord[][2], uint16_t entries, uint16_t radius, uint16_t color);
void graphDrawNet(Graph *g, int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GRAPH */

#endif
/** @} */

