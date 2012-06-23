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


