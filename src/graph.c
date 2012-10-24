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

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "graph.h"

#if GFX_USE_GRAPH

point_t origin;		// origin of graph

static void swapCoordinates(coord_t *a, coord_t *b) {
	coord_t temp;
 
	temp = *b;
	*b = *a;
	*a = temp;   
}

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

void graphDrawOneQuadrant(Graph *g) {
	if(g->x0 > g->x1)
		swapCoordinates(&g->x0, &g->x1);
	if(g->y0 > g->y1)
		swapCoordinates(&g->y0, &g->y1);

	uint16_t i;
    uint16_t length_x = (g->x1 - g->x0);
    uint16_t length_y = (g->y1 - g->y0);
    uint16_t middle_x = (g->x1 - g->x0) / 2;
    uint16_t middle_y = (g->y1 - g->y0) / 2;

	origin.x = g->x0;
	origin.y = g->y1;

    /* X Axis */
    gdispDrawLine(g->x0, g->y1, g->x1, g->y1, g->color);
	if(g->grid_size > 0)
		for(i = 0; i <= (length_y / g->grid_size); i++)
    	    _horizontalDotLine(g->x0, g->y0 + g->grid_size * i, g->x1, g->dot_space, g->color);

    /* Y Axis */
    gdispDrawLine(g->x0, g->y0, g->x0, g->y1, g->color);
	if(g->grid_size > 0);
		for(i = 0; i <= (length_x / g->grid_size); i++)
    	    _verticalDotLine(g->x0 + g->grid_size * i, g->y0, g->y1, g->dot_space, g->color);		
}

void graphDrawFourQuadrants(Graph *g) {
	if(g->x0 > g->x1)
		swapCoordinates(&g->x0, &g->x1);
	if(g->y0 > g->y1)
		swapCoordinates(&g->y0, &g->y1);

	uint16_t i;
	uint16_t length_x = (g->x1 - g->x0);
	uint16_t length_y = (g->y1 - g->y0);
	uint16_t middle_x = (g->x1 - g->x0) / 2;
	uint16_t middle_y = (g->y1 - g->y0) / 2;

	origin.x = middle_x;
	origin.y = middle_y;

	/* X Axis */
	gdispDrawLine(g->x0, middle_y, g->x1, middle_y, g->color);
	if(g->grid_size > 0)
		for(i = 0; i <= (length_y / g->grid_size); i++) 
			_horizontalDotLine(g->x0, g->y0 + g->grid_size * i, g->x1, g->dot_space, g->color);

	/* Y Axis */
	gdispDrawLine(middle_x, g->y0, middle_x, g->y1, g->color);
	if(g->grid_size > 0)
		for(i = 0; i <= (length_x / g->grid_size); i++)
			_verticalDotLine(g->x0 + g->grid_size * i, g->y0, g->y1, g->dot_space, g->color);
}

void graphDrawDot(coord_t x, coord_t y, uint16_t radius, color_t color) {
	if(radius == 1)
		gdispDrawPixel(origin.x + x, origin.y + y, color);
	else
		gdispFillCircle(origin.x + x, origin.y + y, radius, color);
}

void graphDrawDots(int coord[][2], uint16_t entries, uint16_t radius, uint16_t color) {
    uint16_t i;

    for(i = 0; i < entries; i++)
        gdispFillCircle(coord[i][0] + origin.x, origin.y - coord[i][1], radius, color);
}

void graphDrawNet(int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor) {
    uint16_t i;

    for(i = 0; i < entries; ++i)
        gdispDrawLine(coord[i-1][0] + origin.x, origin.y - coord[i-1][1], coord[i][0] + origin.x, origin.y - coord[i][1], lineColor); 
    for(i = 0; i < entries; ++i)
        if(radius != 0)
            lcdFillCircle(coord[i][0] + origin.x, origin.y - coord[i][1], radius, dotColor);
}

#endif /* GFX_USE_GRAPH */

