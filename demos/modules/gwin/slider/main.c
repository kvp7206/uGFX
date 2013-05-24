/*
    ChibiOS/GFX - Copyright (C) 2012, 2013
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

#include "gfx.h"
#include "chprintf.h"

static GListener gl;

#define SLIDER_WIDTH	20

int main(void) {
	coord_t				swidth, sheight;
	GHandle				ghSliderH, ghSliderV, ghConsole;
	font_t				fui2;
	GEvent *			pe;
	GEventGWinSlider *	pSliderEvent;
	BaseSequentialStream *consout;

	gfxInit();		// Initialize the display

	// Get the display dimensions
	swidth = gdispGetWidth();
	sheight = gdispGetHeight();

	// Get the font
	fui2 = gdispOpenFont("UI2");

	// Create out gwin windows/widgets
	ghSliderH = gwinCreateSlider(NULL, 5, 5, swidth-10, SLIDER_WIDTH);
	ghSliderV = gwinCreateSlider(NULL, 5, 10+SLIDER_WIDTH, SLIDER_WIDTH, sheight-15+SLIDER_WIDTH);
	ghConsole = gwinCreateConsole(NULL, 10+SLIDER_WIDTH, 10+SLIDER_WIDTH, swidth-15-SLIDER_WIDTH, sheight-15-SLIDER_WIDTH, fui2);
	consout = gwinGetConsoleStream(ghConsole);

	// Color up the console window
    gwinSetColor(ghConsole, White);
    gwinSetBgColor(ghConsole, Blue);

    // Assign the mouse and dials to the sliders.
#if GINPUT_NEED_MOUSE
	gwinAttachSliderMouse(ghSliderH, 0);
	gwinAttachSliderMouse(ghSliderV, 0);
#endif
#if GINPUT_NEED_DIAL
	gwinAttachSliderDial(ghSliderV, 0);
	gwinAttachSliderDial(ghSliderH, 1);
#endif

    // We want to listen for slider events
	geventListenerInit(&gl);
	geventAttachSource(&gl, gwinGetSliderSource(ghSliderH), 0);
	geventAttachSource(&gl, gwinGetSliderSource(ghSliderV), 0);

	// Draw everything on the screen
    gwinClear(ghConsole);
    gwinSliderDraw(ghSliderH);
    gwinSliderDraw(ghSliderV);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
		case GEVENT_GWIN_SLIDER:
			pSliderEvent = (GEventGWinSlider *)pe;
			chprintf(consout, "%c=%d\n", pSliderEvent->slider == ghSliderH ? 'H' : 'V', pSliderEvent->position);
			break;
		}
	}

	return 0;
}
