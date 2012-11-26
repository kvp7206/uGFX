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

void mandelbrot(float x1, float y1, float x2, float y2) {
	unsigned int i,j, width, height;
	uint16_t iter;
	color_t color;
	float fwidth, fheight;
	
	float sy = y2 - y1;
	float sx = x2 - x1;
	const int MAX = 512;
	
	width = (unsigned int)gdispGetWidth();
	height = (unsigned int)gdispGetHeight();
	fwidth = width;
	fheight = height;
	
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			float cy = j * sy / fheight + y1;
			float cx = i * sx / fwidth + x1;
			float x=0.0f, y=0.0f, xx=0.0f, yy=0.0f;
			for(iter=0; iter <= MAX && xx+yy<4.0f; iter++) {
				xx = x*x;
				yy = y*y;
				y = 2.0f*x*y + cy;
				x = xx - yy + cx;
			}
			//color = ((iter << 8) | (iter&0xFF));
			color = RGB2COLOR(iter<<7, iter<<4, iter);
			gdispDrawPixel(i, j, color);
		}
	}
}

int main(void) {
	float cx, cy;
	float zoom = 1.0f;

	halInit();
	chSysInit();

	gdispInit();
	gdispSetOrientation(GDISP_ROTATE_270);

	/* where to zoom in */
	cx = -0.086f;
	cy = 0.85f;

	while(TRUE) {
		mandelbrot(-2.0f*zoom+cx, -1.5f*zoom+cy, 2.0f*zoom+cx, 1.5f*zoom+cy);

		zoom *= 0.7f;
		if(zoom <= 0.00001f)
			zoom = 1.0f;	
	}
}

