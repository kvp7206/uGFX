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

#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "graph.h"

#if GFX_USE_GRAPH

static void _horizontalDotLine(coord_t x0, coord_t y0, coord_t x1, uint16_t space, color_t color) {
	uint16_t offset = x0;
	uint16_t count = ((x1 - x0) / space);

	do {
		gdispDrawPixel(offset, y0, color);
		offset += space;
	} while(count--);
}

static void _verticalDotLine(coord_t x0, coord_t y0, coord_t y1, uint16_t space, color_t color) {
	uint16_t offset = y0;
	uint16_t count = ((y1 - y0) / space);

	do {
		gdispDrawPixel(x0, offset, color);
		offset += space;
	} while(count--);
}

void graphDrawSystem(Graph *g) {
	uint16_t i;

	g->x0 = g->origin_x - abs(g->xmin);
	g->x1 = g->origin_x + abs(g->xmax);
	g->y0 = g->origin_y + abs(g->ymin);
	g->y1 = g->origin_y - abs(g->ymax);

	/* X - Axis */
	if(g->full_grid) {
		for(i = 1; i <= ((g->origin_y - g->y1) / g->grid_size); i++) {
			_horizontalDotLine(g->x0, g->origin_y - g->grid_size * i, g->x1, g->dot_space, g->grid_color);
		}
		for(i = 1; i <= ((g->y0 - g->origin_y) / g->grid_size); i++) {
			_horizontalDotLine(g->x0, g->origin_y + g->grid_size * i, g->x1, g->dot_space, g->grid_color);
		}
	}
	gdispDrawLine(g->x0, g->origin_y, g->x1, g->origin_y, g->axis_color);
	if(g->arrows) {
		gdispDrawLine(g->x1, g->origin_y, g->x1-5, g->origin_y+5, g->axis_color);
		gdispDrawLine(g->x1, g->origin_y, g->x1-5, g->origin_y-5, g->axis_color);
	}

	/* Y - Axis */
	if(g->full_grid) {
		for(i = 1; i <= ((g->origin_x - g->x0) / g->grid_size); i++) {
			_verticalDotLine(g->origin_x - g->grid_size * i, g->y1, g->y0, g->dot_space, g->grid_color);
		}
		for(i = 1; i <= ((g->x1 - g->origin_x) / g->grid_size); i++) {
			_verticalDotLine(g->origin_x + g->grid_size * i, g->y1, g->y0, g->dot_space, g->grid_color);
		}
	}
	gdispDrawLine(g->origin_x, g->y0, g->origin_x, g->y1, g->axis_color);
	if(g->arrows) {
		gdispDrawLine(g->origin_x, g->y1, g->origin_x-5, g->y1+5, g->axis_color);
		gdispDrawLine(g->origin_x, g->y1, g->origin_x+5, g->y1+5, g->axis_color);
	}
}

bool_t _boundaryCheck(Graph *g, coord_t x, coord_t y) {
	if(g->origin_x + x > g->x1)
		return 1;
	if(g->origin_x + x < g->x0)
		return 1;
	if(g->origin_y - y < g->y1)
		return 1;
	if(g->origin_y - y > g->y0)
		return 1;

	return 0;
}

void graphDrawDot(Graph *g, coord_t x, coord_t y, uint16_t radius, color_t color) {
	if(_boundaryCheck(g, x, y))
		return;

	if(radius == 0)
		gdispDrawPixel(g->origin_x + x, g->origin_y - y, color);
	else
		gdispFillCircle(g->origin_x + x, g->origin_y - y, radius, color);
}

void graphDrawDots(Graph *g, int coord[][2], uint16_t entries, uint16_t radius, uint16_t color) {
    uint16_t i;

    for(i = 0; i < entries; i++) {
		if(_boundaryCheck(g, coord[i][0], coord[i][1]))
			continue;

		if(radius == 0)
			gdispDrawPixel(coord[i][0] + g->origin_x, g->origin_y - coord[i][1], color);
		else
        	gdispFillCircle(coord[i][0] + g->origin_x, g->origin_y - coord[i][1], radius, color);
	}
}

void graphDrawNet(Graph *g, int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor) {
    uint16_t i;

	/* draw lines */
    for(i = 0; i < entries-1; i++) {
		if(_boundaryCheck(g, coord[i][0], coord[i][1]))
			continue;

        gdispDrawLine(coord[i][0] + g->origin_x, g->origin_y - coord[i][1], coord[i+1][0] + g->origin_x, g->origin_y - coord[i+1][1], lineColor); 
	}
    
	/* draw dots */
	for(i = 0; i < entries; i++) {
		if(_boundaryCheck(g, coord[i][0], coord[i][1]))
			continue;

        if(radius == 0)
			gdispDrawPixel(coord[i][0] + g->origin_x, g->origin_y - coord[i][1], dotColor);
		else
            gdispFillCircle(coord[i][0] + g->origin_x, g->origin_y - coord[i][1], radius, dotColor);
	}
}

#endif /* GFX_USE_GRAPH */

