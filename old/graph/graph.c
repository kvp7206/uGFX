/*
    ChibiOS/RT - Copyright (C) 2012
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

#include "glcd.h"

#define MARKSIZE	5 	// half

static uint16_t x, y;			// origins in graph
static uint16_t grid_X, grid_Y;	//grids

void graphDrawSystem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gridX, uint16_t gridY, uint16_t color) {
	uint16_t i, length;
	volatile uint16_t off;

	x = x0;
	y = y0;
	grid_X = gridX;
	grid_Y = gridY;

	// X-Axis
	length = x1 - x0;
	lcdDrawLine(x0, y0, x1, y0, color);
	lcdDrawLine(x1, y0, x1-5, y0+5, color);
	lcdDrawLine(x1, y0, x1-5, y0-5, color);	
	for(i=1; i<(length / grid_X); i++) {
		off = x0 + i * grid_X;
		lcdDrawLine(off, y0-MARKSIZE, off, y0+MARKSIZE, color);
	}

	// Y-Axis
	length = y0 - y1;
	lcdDrawLine(x0, y0, x0, y1, color);
	lcdDrawLine(x0, y1, x0-5, y1+5, color);
	lcdDrawLine(x0, y1, x0+5, y1+5, color);
	for(i=1; i<(length / grid_Y); i++) {
		off = y0 + i * grid_Y;
		lcdDrawLine(x0-MARKSIZE, off, x0+MARKSIZE, off, color);	
	}
}

void graphDrawDots(int coord[][2], uint16_t entries, uint16_t radius, uint16_t color) {
	uint16_t i;

	for(i = 0; i < entries; i++)
		lcdDrawCircle(coord[i][0]+x, y-coord[i][1], radius, 1, color);
}

void graphDrawNet(int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor) {
	uint16_t i;

	for(i = 0; i < entries; ++i)
		lcdDrawLine(coord[i-1][0]+x, y-coord[i-1][1], coord[i][0]+x, y-coord[i][1], lineColor); 
	for(i = 0; i < entries; ++i)
		if(radius != 0)
			lcdDrawCircle(coord[i][0]+x, y-coord[i][1], radius, 1, dotColor);
}


