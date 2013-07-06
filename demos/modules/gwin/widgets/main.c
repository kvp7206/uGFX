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

// include our chibios logo in a .gif format
#include "image_chibios.h"

static GListener	gl;
static GHandle		ghConsole;
static GHandle		ghTabButtons, ghTabSliders, ghTabCheckboxes, ghTabLabels, ghTabRadios, ghTabImages;
static GHandle		ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2;
static GHandle		ghLabel1;
static GHandle		ghRadio1, ghRadio2;
static GHandle	ghImage1;

#define	ScrWidth		gdispGetWidth()
#define	ScrHeight		gdispGetHeight()

#define TAB_HEIGHT		30
#define BUTTON_WIDTH	50
#define BUTTON_HEIGHT	30
#define SLIDER_WIDTH	20
#define CHECKBOX_WIDTH	80
#define CHECKBOX_HEIGHT	20
#define RADIO_WIDTH		50
#define RADIO_HEIGHT	20
#define GROUP_TABS		0
#define GROUP_R1R2		1

static GHandle createTab(GWidgetInit *pwi) {
	GHandle		gh;

	gh = gwinCreateRadio(NULL, pwi, GROUP_TABS);
	gwinSetCustomDraw(gh, gwinRadioDraw_Tab, 0);
	gwinSetVisible(gh, TRUE);
	return gh;
}

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

	// Create the gwin windows/widgets
	{
		GWidgetInit		wi;

		// Create the Tabs
		wi.g.show = FALSE; wi.g.width = ScrWidth/6; wi.g.height = TAB_HEIGHT; wi.g.y = 0;
		wi.g.x = 0*wi.g.width; wi.text = "Buttons";		ghTabButtons	= createTab(&wi);
		wi.g.x = 1*wi.g.width; wi.text = "Sliders";		ghTabSliders	= createTab(&wi);
		wi.g.x = 2*wi.g.width; wi.text = "Checkbox";	ghTabCheckboxes	= createTab(&wi);
		wi.g.x = 3*wi.g.width; wi.text = "Labels";		ghTabLabels		= createTab(&wi);
		wi.g.x = 4*wi.g.width; wi.text = "Radios";		ghTabRadios		= createTab(&wi);
		wi.g.x = 5*wi.g.width; wi.text = "Images";		ghTabImages		= createTab(&wi);

		// Buttons
		wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT; wi.g.y = TAB_HEIGHT+5;
		wi.g.x = 0+0*(BUTTON_WIDTH+1); wi.text = "B1"; ghButton1 = gwinCreateButton(NULL, &wi);
		wi.g.x = 0+1*(BUTTON_WIDTH+1); wi.text = "B2"; ghButton2 = gwinCreateButton(NULL, &wi);
		wi.g.x = 0+2*(BUTTON_WIDTH+1); wi.text = "B3"; ghButton3 = gwinCreateButton(NULL, &wi);
		wi.g.x = 0+3*(BUTTON_WIDTH+1); wi.text = "B4"; ghButton4 = gwinCreateButton(NULL, &wi);

		// Horizontal Sliders
		wi.g.width = ScrWidth/2-2; wi.g.height = SLIDER_WIDTH; wi.g.x = ScrWidth/2+1;
		wi.g.y = ScrHeight/2-2*(SLIDER_WIDTH+1); wi.text = "S1"; ghSlider1 = gwinCreateSlider(NULL, &wi);
		wi.g.y = ScrHeight/2-1*(SLIDER_WIDTH+1); wi.text = "S2"; ghSlider2 = gwinCreateSlider(NULL, &wi);

		// Vertical Sliders
		wi.g.width = SLIDER_WIDTH; wi.g.height = ScrHeight/2-2; wi.g.y = ScrHeight/2+1;
		wi.g.x = 0+0*(SLIDER_WIDTH+1); wi.text = "S3"; ghSlider3 = gwinCreateSlider(NULL, &wi);
		wi.g.x = 0+1*(SLIDER_WIDTH+1); wi.text = "S4"; ghSlider4 = gwinCreateSlider(NULL, &wi);

		// Checkboxes - for the 2nd checkbox we apply special drawing before making it visible
		wi.g.width = CHECKBOX_WIDTH; wi.g.height = CHECKBOX_HEIGHT; wi.g.x = 0;
		wi.g.y = TAB_HEIGHT+5+0*(CHECKBOX_HEIGHT+1); wi.text = "C1"; ghCheckbox1 = gwinCreateCheckbox(NULL, &wi);
		wi.g.y = TAB_HEIGHT+5+1*(CHECKBOX_HEIGHT+1); wi.text = "C2"; ghCheckbox2 = gwinCreateCheckbox(NULL, &wi);
	    gwinSetCustomDraw(ghCheckbox2, gwinCheckboxDraw_CheckOnRight, 0);

	    // Labels
		wi.g.width = 0;		// dynamic width
		wi.g.y = TAB_HEIGHT+5+2*(CHECKBOX_HEIGHT+1); wi.text = "L1"; ghLabel1 = gwinLabelCreate(NULL, &wi);

		// Radio Buttons
		wi.g.width = RADIO_WIDTH; wi.g.height = RADIO_HEIGHT; wi.g.y = TAB_HEIGHT+5;
		wi.g.x = 0*wi.g.width; wi.text = "Yes";		ghRadio1	= gwinCreateRadio(NULL, &wi, GROUP_R1R2);
		wi.g.x = 1*wi.g.width; wi.text = "No";		ghRadio2	= gwinCreateRadio(NULL, &wi, GROUP_R1R2);

		// Image
		wi.g.x = ScrWidth-210; wi.g.y = TAB_HEIGHT + 10; wi.g.width = 200; wi.g.height = 200;
		ghImage1 = gwinImageCreate(NULL, &wi);
		gwinImageOpenMemory(ghImage1, image_chibios);
		gwinImageCache(ghImage1);

		// Console - we apply some special colors before making it visible
		wi.g.width = ScrWidth/2-1; wi.g.height = ScrHeight/2-1;
		wi.g.x = ScrWidth/2+1; wi.g.y = ScrHeight/2+1;
		ghConsole = gwinCreateConsole(NULL, &wi.g);
	    gwinSetColor(ghConsole, Yellow);
	    gwinSetBgColor(ghConsole, Black);
	}

    // Assign toggles and dials to the buttons & sliders etc.
	#if GINPUT_NEED_TOGGLE
		gwinAttachToggle(ghButton1, 0, 0);
		gwinAttachToggle(ghButton2, 0, 1);
	#endif
	#if GINPUT_NEED_DIAL
		gwinAttachDial(ghSlider1, 0, 0);
		gwinAttachDial(ghSlider3, 0, 1);
	#endif

	// Make the console visible
	gwinSetVisible(ghConsole, TRUE);
	gwinClear(ghConsole);

	// Press the Buttons Tab
	gwinPressRadio(ghTabButtons);

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
		case GEVENT_GWIN_RADIO:
			gwinPrintf(ghConsole, "Radio Group %u=%s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->radio));

			// Is this the tab radio's
			if (((GEventGWinRadio *)pe)->group == GROUP_TABS) {

				// Do some special animation for Label1
				if (((GEventGWinRadio *)pe)->radio == ghTabLabels) {
					gwinSetBgColor(ghLabel1, gwinGetDefaultBgColor());
					gwinSetColor(ghLabel1, gwinGetDefaultColor());
				}

				// Set control visibility depending on the tab selected
				gwinSetVisible(ghButton1, ((GEventGWinRadio *)pe)->radio == ghTabButtons);
				gwinSetVisible(ghButton2, ((GEventGWinRadio *)pe)->radio == ghTabButtons);
				gwinSetVisible(ghButton3, ((GEventGWinRadio *)pe)->radio == ghTabButtons);
				gwinSetVisible(ghButton4, ((GEventGWinRadio *)pe)->radio == ghTabButtons);
				gwinSetVisible(ghSlider1, ((GEventGWinRadio *)pe)->radio == ghTabSliders);
				gwinSetVisible(ghSlider2, ((GEventGWinRadio *)pe)->radio == ghTabSliders);
				gwinSetVisible(ghSlider3, ((GEventGWinRadio *)pe)->radio == ghTabSliders);
				gwinSetVisible(ghSlider4, ((GEventGWinRadio *)pe)->radio == ghTabSliders);
				gwinSetVisible(ghCheckbox1, ((GEventGWinRadio *)pe)->radio == ghTabCheckboxes);
				gwinSetVisible(ghCheckbox2, ((GEventGWinRadio *)pe)->radio == ghTabCheckboxes);
				gwinSetVisible(ghLabel1, ((GEventGWinRadio *)pe)->radio == ghTabLabels);
				gwinSetVisible(ghRadio1, ((GEventGWinRadio *)pe)->radio == ghTabRadios);
				gwinSetVisible(ghRadio2, ((GEventGWinRadio *)pe)->radio == ghTabRadios);
				gwinSetVisible(ghImage1, ((GEventGWinRadio *)pe)->radio == ghTabImages);

				// Do some special animation for Label1
				if (((GEventGWinRadio *)pe)->radio == ghTabLabels) {
					gfxSleepMilliseconds(1000);
				    gwinSetBgColor(ghLabel1, Blue);
				    gwinSetColor(ghLabel1, Yellow);
					gwinSetText(ghLabel1, "Very Big Label", FALSE);

					gfxSleepMilliseconds(1000);
				    gwinSetBgColor(ghLabel1, Yellow);
				    gwinSetColor(ghLabel1, Red);
					gwinSetText(ghLabel1, "L1", FALSE);
				}
			}
			break;
		default:
			gwinPrintf(ghConsole, "Unknown %d\n", pe->type);
			break;
		}
	}
	return 0;
}
