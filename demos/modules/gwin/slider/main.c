/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
