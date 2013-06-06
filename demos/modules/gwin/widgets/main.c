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

static GListener	gl;
static GHandle		ghConsole;
static GHandle		ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2;

#define	ScrWidth		gdispGetWidth()
#define	ScrHeight		gdispGetHeight()

#define BUTTON_WIDTH	50
#define BUTTON_HEIGHT	30
#define SLIDER_WIDTH	10
#define CHECKBOX_WIDTH	80
#define CHECKBOX_HEIGHT	20

int main(void) {
	GEvent *			pe;

	// Initialize the display
	gfxInit();
	gdispClear(White);

	// Set the font
	gwinSetDefaultFont(gdispOpenFont("UI2"));

	// Create out gwin windows/widgets
	ghButton1 = gwinCreateButton(NULL, 0+0*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton2 = gwinCreateButton(NULL, 0+1*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton3 = gwinCreateButton(NULL, 0+2*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton4 = gwinCreateButton(NULL, 0+3*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghConsole = gwinCreateConsole(NULL, ScrWidth/2+1, ScrHeight/2+1, ScrWidth/2-1, ScrHeight/2-1);
	ghSlider1 = gwinCreateSlider(NULL, ScrWidth/2+1, ScrHeight/2-2*(SLIDER_WIDTH+1), ScrWidth/2-2, SLIDER_WIDTH);
	ghSlider2 = gwinCreateSlider(NULL, ScrWidth/2+1, ScrHeight/2-1*(SLIDER_WIDTH+1), ScrWidth/2-2, SLIDER_WIDTH);
	ghSlider3 = gwinCreateSlider(NULL, 0+1*(SLIDER_WIDTH+1), ScrHeight/2+1, SLIDER_WIDTH, ScrHeight/2-2);
	ghSlider4 = gwinCreateSlider(NULL, 0+1*(SLIDER_WIDTH+1), ScrHeight/2+1, SLIDER_WIDTH, ScrHeight/2-2);
	ghCheckbox1 = gwinCreateCheckbox(NULL, 0, BUTTON_HEIGHT+1, CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
	ghCheckbox2 = gwinCreateCheckbox(NULL, 0, BUTTON_HEIGHT+1+1*(CHECKBOX_HEIGHT+1), CHECKBOX_WIDTH, CHECKBOX_HEIGHT);

	// Color everything
    gwinSetColor(ghConsole, Yellow);
    gwinSetBgColor(ghConsole, Black);

    // Set the text on all the controls
    gwinSetText(ghButton1, "B1", FALSE);
    gwinSetText(ghButton2, "B2", FALSE);
    gwinSetText(ghButton3, "B3", FALSE);
    gwinSetText(ghButton4, "B4", FALSE);
    gwinSetText(ghSlider1, "S1", FALSE);
    gwinSetText(ghSlider2, "S2", FALSE);
    gwinSetText(ghSlider3, "S3", FALSE);
    gwinSetText(ghSlider4, "S4", FALSE);
    gwinSetText(ghCheckbox1, "C1", FALSE);
    gwinSetText(ghCheckbox2, "C2", FALSE);

    // Assign the mouse and dials to the buttons & sliders etc.
#if GINPUT_NEED_MOUSE
	gwinAttachMouse(ghSlider1, 0);
	gwinAttachMouse(ghSlider2, 0);
	gwinAttachMouse(ghSlider3, 0);
	gwinAttachMouse(ghSlider4, 0);
	gwinAttachMouse(ghButton1, 0);
	gwinAttachMouse(ghButton2, 0);
	gwinAttachMouse(ghButton3, 0);
	gwinAttachMouse(ghButton4, 0);
	gwinAttachMouse(ghCheckbox1, 0);
	gwinAttachMouse(ghCheckbox2, 0);
#endif
#if GINPUT_NEED_DIAL
	gwinAttachSliderDial(ghSlider1, 0);
	gwinAttachSliderDial(ghSlider3, 1);
#endif

    // We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(ghSlider1, &gl, 0);
	gwinAttachListener(ghSlider2, &gl, 0);
	gwinAttachListener(ghSlider3, &gl, 0);
	gwinAttachListener(ghSlider4, &gl, 0);
	gwinAttachListener(ghButton1, &gl, 0);
	gwinAttachListener(ghButton2, &gl, 0);
	gwinAttachListener(ghButton3, &gl, 0);
	gwinAttachListener(ghButton4, &gl, 0);
	gwinAttachListener(ghCheckbox1, &gl, 0);
	gwinAttachListener(ghCheckbox2, &gl, 0);

	// Draw everything on the screen
    gwinClear(ghConsole);
    gwinDraw(ghSlider1);
    gwinDraw(ghSlider2);
    gwinDraw(ghSlider3);
    gwinDraw(ghSlider4);
    gwinDraw(ghButton1);
    gwinDraw(ghButton2);
    gwinDraw(ghButton3);
    gwinDraw(ghButton4);
    gwinDraw(ghCheckbox1);
    gwinDraw(ghCheckbox2);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
		case GEVENT_GWIN_BUTTON:
			gwinPrintf(ghConsole, "Button %s\n", gwinGetText(((GEventGWinButton *)pe)->button));
			break;
		case GEVENT_GWIN_SLIDER:
			gwinPrintf(ghConsole, "Slider %s=%d\n", gwinGetText(((GEventGWinSlider *)pe)->slider), ((GEventGWinSlider *)pe)->position);
			break;
		case GEVENT_GWIN_CHECKBOX:
			gwinPrintf(ghConsole, "Checkbox %s=%s\n", gwinGetText(((GEventGWinCheckbox *)pe)->checkbox), ((GEventGWinCheckbox *)pe)->isChecked ? "Checked" : "UnChecked");
			break;
		default:
			gwinPrintf(ghConsole, "Unknown %d\n", pe->type);
			break;
		}
	}
	return 0;
}
