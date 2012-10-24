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

#ifndef GRAPH_H
#define GRAPH_H

#if GFX_USE_GRAPH

typedef struct _Graph {
	coord_t x0;
	coord_t y0;
	coord_t x1;
	coord_t y1;
	uint16_t grid_size;
	uint16_t dot_space;
	bool_t full_grid;
	color_t color;
} Graph;

#ifdef __cplusplus
extern "C" {
#endif

void graphDrawOneQuadrat(Graph *g);
void graphDrawFourQuadrants(Graph *g);
void graphDrawDots(int coord[][2], uint16_t entries, uint16_t radius, uint16_t color);
void graphDrawNet(int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GRAPH */

#endif

