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

#ifndef GRAPH_H
#define GRAPH_H

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Description: does draw axis arrows and grid
 *
 * param:
 *		- x0, y0, x1, y1:	location of arrows
 *		- gridX, gridY:		grid size in X and Y direction
 *		- color:			color of arrows
 *
 * return: none
 */
void graphDrawSystem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t gridX, uint16_t gridY, uint16_t color);

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
void graphDrawDots(int coord[][2], uint16_t entries, uint16_t radius, uint16_t color);

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
void graphDrawNet(int coord[][2], uint16_t entries, uint16_t radius, uint16_t lineColor, uint16_t dotColor);

#ifdef __cplusplus
}
#endif

#endif
