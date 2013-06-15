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

static GListener	gl;
static GHandle		ghConsole;
static GHandle		ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2;

#define	ScrWidth		gdispGetWidth()
#define	ScrHeight		gdispGetHeight()

#define BUTTON_WIDTH	50
#define BUTTON_HEIGHT	30
#define SLIDER_WIDTH	20
#define CHECKBOX_WIDTH	80
#define CHECKBOX_HEIGHT	20

int main(void) {
	GEvent *			pe;

	// Initialize the display
	gfxInit();
	gdispClear(White);

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultColor(Black);
	gwinSetDefaultBgColor(White);

    // We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	// Connect the mouse
	#if GINPUT_NEED_MOUSE
		gwinAttachMouse(0);
	#endif

	// Create out gwin windows/widgets
	ghConsole = gwinCreateConsole(NULL, ScrWidth/2+1, ScrHeight/2+1, ScrWidth/2-1, ScrHeight/2-1);
	ghButton1 = gwinCreateButton(NULL, 0+0*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton2 = gwinCreateButton(NULL, 0+1*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton3 = gwinCreateButton(NULL, 0+2*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghButton4 = gwinCreateButton(NULL, 0+3*(BUTTON_WIDTH+1), 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	ghSlider1 = gwinCreateSlider(NULL, ScrWidth/2+1, ScrHeight/2-2*(SLIDER_WIDTH+1), ScrWidth/2-2, SLIDER_WIDTH);
	ghSlider2 = gwinCreateSlider(NULL, ScrWidth/2+1, ScrHeight/2-1*(SLIDER_WIDTH+1), ScrWidth/2-2, SLIDER_WIDTH);
	ghSlider3 = gwinCreateSlider(NULL, 0+0*(SLIDER_WIDTH+1), ScrHeight/2+1, SLIDER_WIDTH, ScrHeight/2-2);
	ghSlider4 = gwinCreateSlider(NULL, 0+1*(SLIDER_WIDTH+1), ScrHeight/2+1, SLIDER_WIDTH, ScrHeight/2-2);
	ghCheckbox1 = gwinCreateCheckbox(NULL, 0, BUTTON_HEIGHT+1, CHECKBOX_WIDTH, CHECKBOX_HEIGHT);
	ghCheckbox2 = gwinCreateCheckbox(NULL, 0, BUTTON_HEIGHT+1+1*(CHECKBOX_HEIGHT+1), CHECKBOX_WIDTH, CHECKBOX_HEIGHT);

	// Color everything and set special drawing for some widgets
    gwinSetColor(ghConsole, Yellow);
    gwinSetBgColor(ghConsole, Black);
    gwinSetCustomDraw(ghCheckbox2, gwinCheckboxDraw_CheckOnRight, 0);

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

    // Assign toggles and dials to the buttons & sliders etc.
	#if GINPUT_NEED_TOGGLE
		gwinAttachToggle(ghButton1, 0, 0);
		gwinAttachToggle(ghButton2, 0, 1);
	#endif
	#if GINPUT_NEED_DIAL
		gwinAttachDial(ghSlider1, 0, 0);
		gwinAttachDial(ghSlider3, 0, 1);
	#endif

	// Draw everything on the screen
	gwinClear(ghConsole);
	gwinSetVisible(ghButton1, TRUE);
	gwinSetVisible(ghButton2, TRUE);
	gwinSetVisible(ghButton3, TRUE);
	gwinSetVisible(ghButton4, TRUE);
	gwinSetVisible(ghSlider1, TRUE);
	gwinSetVisible(ghSlider2, TRUE);
	gwinSetVisible(ghSlider3, TRUE);
	gwinSetVisible(ghSlider4, TRUE);
	gwinSetVisible(ghCheckbox1, TRUE);
	gwinSetVisible(ghCheckbox2, TRUE);

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
