#ifndef GRAPH_H
#define GRAPH_H

/*
 * Description: does draw axis arrows and grid
 *
 * param:
 *		- x0, y0, x1, y1:	location of arrows
 *		- color:			color of arrows
 *
 * return: none
 */
void graphDrawSystem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/*
 * Description: does draw coordinates into graph as dots
 *
 * param:
 *		- coord:		two dimensionl array containing X and Y coordinates
 *		- entries:		amount of coordinates passed (sizeof(coord)/sizeof(coord[0])
 *		- radius:		size of the dots
 *		- color:		color of the dots
 *
 * return: none
 */
void graphDrawDots(int16_t coord[][2], uint16_t entries, uint16_t radius, uint16_t color);

/*
 * Description: does draw coordinates into graph connected by lines
 *
 * param:
 *		- coord:		two dimensional array containing X and Y coordinates
 *		- entries:		amount of coordinates passed (sizeof(coord)/sizeof(coord[0])
 *		- radius:		if not 0, graphDrawDots() gets invoked
 *		- lineColor:	color of the lines
 *		- dotColor:		color of dots, if radius != 0
 *
 * return: none
 */	
void graphDrawNet(int16_t coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor);

#endif
