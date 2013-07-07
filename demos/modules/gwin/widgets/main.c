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

/**
 * This demo demonstrates many of the GWIN widgets.
 * On the "Radio" tab try playing with the color radio buttons.
 * On the "Checkbox" tab try playing with the "Disable All" checkbox.
 */

/* Our custom yellow style */
static const GWidgetStyle YellowWidgetStyle = {
	Yellow,							// window background

	// enabled color set
	{
		HTML2COLOR(0x0000FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xE0E0E0),		// progress - inactive area
	},

	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0),		// progress - active area
	},

	// pressed color set
	{
		HTML2COLOR(0xFF00FF),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000),		// progress - active area
	},
};

/* The variables we need */
static GListener	gl;
static GHandle		ghConsole;
static GHandle		ghTabButtons, ghTabSliders, ghTabCheckboxes, ghTabLabels, ghTabRadios, ghTabImages;
static GHandle		ghButton1, ghButton2, ghButton3, ghButton4;
static GHandle		ghSlider1, ghSlider2, ghSlider3, ghSlider4;
static GHandle		ghCheckbox1, ghCheckbox2, ghCheckDisableAll;
static GHandle		ghLabel1;
static GHandle		ghRadio1, ghRadio2;
static GHandle		ghRadioBlack, ghRadioWhite, ghRadioYellow;
static GHandle		ghImage1;

/* Some useful macros */
#define	ScrWidth			gdispGetWidth()
#define	ScrHeight			gdispGetHeight()

#define TAB_HEIGHT			30
#define LABEL_HEIGHT		40
#define BUTTON_WIDTH		50
#define BUTTON_HEIGHT		30
#define SLIDER_WIDTH		20
#define CHECKBOX_WIDTH		80
#define CHECKBOX_HEIGHT		20
#define RADIO_WIDTH			50
#define RADIO_HEIGHT		20
#define COLOR_WIDTH			80
#define DISABLEALL_WIDTH	100
#define GROUP_TABS			0
#define GROUP_YESNO			1
#define GROUP_COLORS		2

/**
 * Create all the widgets.
 * With the exception of the Tabs they are all created invisible.
 */
static void createWidgets(void) {
	GWidgetInit		wi;

	wi.customDraw = 0; wi.customParam = 0; wi.customStyle = 0;

	// Create the Tabs
	wi.g.show = TRUE; wi.customDraw = gwinRadioDraw_Tab;
	wi.g.width = ScrWidth/6; wi.g.height = TAB_HEIGHT; wi.g.y = 0;
	wi.g.x = 0*wi.g.width; wi.text = "Buttons";		ghTabButtons	= gwinRadioCreate(NULL, &wi, GROUP_TABS);
	wi.g.x = 1*wi.g.width; wi.text = "Sliders";		ghTabSliders	= gwinRadioCreate(NULL, &wi, GROUP_TABS);
	wi.g.x = 2*wi.g.width; wi.text = "Checkbox";	ghTabCheckboxes	= gwinRadioCreate(NULL, &wi, GROUP_TABS);
	wi.g.x = 3*wi.g.width; wi.text = "Radios";		ghTabRadios		= gwinRadioCreate(NULL, &wi, GROUP_TABS);
	wi.g.x = 4*wi.g.width; wi.text = "Labels";		ghTabLabels		= gwinRadioCreate(NULL, &wi, GROUP_TABS);
	wi.g.x = 5*wi.g.width; wi.text = "Images";		ghTabImages		= gwinRadioCreate(NULL, &wi, GROUP_TABS);

	// Buttons
	wi.g.show = FALSE; wi.customDraw = 0;
	wi.g.width = BUTTON_WIDTH; wi.g.height = BUTTON_HEIGHT; wi.g.y = TAB_HEIGHT+5;
	wi.g.x = 0+0*(BUTTON_WIDTH+1); wi.text = "B1"; ghButton1 = gwinButtonCreate(NULL, &wi);
	wi.g.x = 0+1*(BUTTON_WIDTH+1); wi.text = "B2"; ghButton2 = gwinButtonCreate(NULL, &wi);
	wi.g.x = 0+2*(BUTTON_WIDTH+1); wi.text = "B3"; ghButton3 = gwinButtonCreate(NULL, &wi);
	wi.g.x = 0+3*(BUTTON_WIDTH+1); wi.text = "B4"; ghButton4 = gwinButtonCreate(NULL, &wi);

	// Horizontal Sliders
	wi.g.width = ScrWidth/2-2; wi.g.height = SLIDER_WIDTH; wi.g.x = ScrWidth/2+1;
	wi.g.y = ScrHeight/2-2*(SLIDER_WIDTH+1); wi.text = "S1"; ghSlider1 = gwinSliderCreate(NULL, &wi);
	wi.g.y = ScrHeight/2-1*(SLIDER_WIDTH+1); wi.text = "S2"; ghSlider2 = gwinSliderCreate(NULL, &wi);

	// Vertical Sliders
	wi.g.width = SLIDER_WIDTH; wi.g.height = ScrHeight/2-2; wi.g.y = ScrHeight/2+1;
	wi.g.x = 0+0*(SLIDER_WIDTH+1); wi.text = "S3"; ghSlider3 = gwinSliderCreate(NULL, &wi);
	wi.g.x = 0+1*(SLIDER_WIDTH+1); wi.text = "S4"; ghSlider4 = gwinSliderCreate(NULL, &wi);

	// Checkboxes - for the 2nd checkbox we apply special drawing before making it visible
	wi.g.width = CHECKBOX_WIDTH; wi.g.height = CHECKBOX_HEIGHT; wi.g.x = 0;
	wi.g.y = TAB_HEIGHT+5+0*(CHECKBOX_HEIGHT+1); wi.text = "C1"; ghCheckbox1 = gwinCheckboxCreate(NULL, &wi);
	wi.customDraw = gwinCheckboxDraw_CheckOnRight;
	wi.g.y = TAB_HEIGHT+5+1*(CHECKBOX_HEIGHT+1); wi.text = "C2"; ghCheckbox2 = gwinCheckboxCreate(NULL, &wi);
	wi.customDraw = 0; wi.g.width = DISABLEALL_WIDTH;
	wi.g.y = TAB_HEIGHT+5+2*(CHECKBOX_HEIGHT+1); wi.text = "Disable All"; ghCheckDisableAll = gwinCheckboxCreate(NULL, &wi);

    // Labels
	wi.g.width = 0;	wi.g.height = LABEL_HEIGHT;	// dynamic width, fixed height
	wi.g.y = TAB_HEIGHT+5+2*(CHECKBOX_HEIGHT+1); wi.text = "Label"; ghLabel1 = gwinLabelCreate(NULL, &wi);

	// Radio Buttons
	wi.g.width = RADIO_WIDTH; wi.g.height = RADIO_HEIGHT; wi.g.y = TAB_HEIGHT+5;
	wi.g.x = 0*wi.g.width; wi.text = "Yes";		ghRadio1	= gwinRadioCreate(NULL, &wi, GROUP_YESNO);
	wi.g.x = 1*wi.g.width; wi.text = "No";		ghRadio2	= gwinRadioCreate(NULL, &wi, GROUP_YESNO);
	wi.g.width = COLOR_WIDTH; wi.g.y += RADIO_HEIGHT+5;
	wi.g.x = 0*wi.g.width; wi.text = "Black";	ghRadioBlack	= gwinRadioCreate(NULL, &wi, GROUP_COLORS);
	wi.g.x = 1*wi.g.width; wi.text = "White";	ghRadioWhite	= gwinRadioCreate(NULL, &wi, GROUP_COLORS);
	wi.g.x = 2*wi.g.width; wi.text = "Yellow";	ghRadioYellow	= gwinRadioCreate(NULL, &wi, GROUP_COLORS);
	gwinRadioPress(ghRadioWhite);

	// Image
	wi.g.x = ScrWidth-210; wi.g.y = TAB_HEIGHT + 10; wi.g.width = 200; wi.g.height = 200;
	ghImage1 = gwinImageCreate(NULL, &wi.g);
	gwinImageOpenMemory(ghImage1, image_chibios);
	gwinImageCache(ghImage1);

	// Console - we apply some special colors before making it visible
	wi.g.width = ScrWidth/2-1; wi.g.height = ScrHeight/2-1;
	wi.g.x = ScrWidth/2+1; wi.g.y = ScrHeight/2+1;
	ghConsole = gwinConsoleCreate(NULL, &wi.g);
    gwinSetColor(ghConsole, Yellow);
    gwinSetBgColor(ghConsole, Black);
}

/**
 * Set the visibility of widgets based on which tab is selected.
 */
static void setTab(GHandle tab) {
	/* Make sure everything is invisible first */
	gwinSetVisible(ghButton1, FALSE);	gwinSetVisible(ghButton2, FALSE);
	gwinSetVisible(ghButton3, FALSE);	gwinSetVisible(ghButton4, FALSE);
	gwinSetVisible(ghSlider1, FALSE);	gwinSetVisible(ghSlider2, FALSE);
	gwinSetVisible(ghSlider3, FALSE);	gwinSetVisible(ghSlider4, FALSE);
	gwinSetVisible(ghCheckbox1, FALSE);	gwinSetVisible(ghCheckbox2, FALSE);	gwinSetVisible(ghCheckDisableAll, FALSE);
	gwinSetVisible(ghLabel1, FALSE);
	gwinSetVisible(ghRadio1, FALSE);	gwinSetVisible(ghRadio2, FALSE);
	gwinSetVisible(ghRadioWhite, FALSE);gwinSetVisible(ghRadioBlack, FALSE);gwinSetVisible(ghRadioYellow, FALSE);
	gwinSetVisible(ghImage1, FALSE);

	/* Turn on widgets depending on the tab selected */
	if (tab == ghTabButtons) {
		gwinSetVisible(ghButton1, TRUE);	gwinSetVisible(ghButton2, TRUE);
		gwinSetVisible(ghButton3, TRUE);	gwinSetVisible(ghButton4, TRUE);
	} else if (tab == ghTabSliders) {
		gwinSetVisible(ghSlider1, TRUE);	gwinSetVisible(ghSlider2, TRUE);
		gwinSetVisible(ghSlider3, TRUE);	gwinSetVisible(ghSlider4, TRUE);
	} else if (tab == ghTabCheckboxes) {
		gwinSetVisible(ghCheckbox1, TRUE);	gwinSetVisible(ghCheckbox2, TRUE);	gwinSetVisible(ghCheckDisableAll, TRUE);
	} else if (tab == ghTabLabels) {
		gwinSetVisible(ghLabel1, TRUE);
	} else if (tab == ghTabRadios) {
		gwinSetVisible(ghRadio1, TRUE);		gwinSetVisible(ghRadio2, TRUE);
		gwinSetVisible(ghRadioWhite, TRUE);	gwinSetVisible(ghRadioBlack, TRUE);	gwinSetVisible(ghRadioYellow, TRUE);
	} else if (tab == ghTabImages) {
		gwinSetVisible(ghImage1, TRUE);
	}
}

/**
 * Set the enabled state of every widget (except the tabs etc)
 */
static void setEnabled(bool_t ena) {
	gwinSetEnabled(ghButton1, ena);		gwinSetEnabled(ghButton2, ena);
	gwinSetEnabled(ghButton3, ena);		gwinSetEnabled(ghButton4, ena);
	gwinSetEnabled(ghSlider1, ena);		gwinSetEnabled(ghSlider2, ena);
	gwinSetEnabled(ghSlider3, ena);		gwinSetEnabled(ghSlider4, ena);
	gwinSetEnabled(ghCheckbox1, ena);	gwinSetEnabled(ghCheckbox2, ena);	//gwinSetEnabled(ghCheckDisableAll, TRUE);
	gwinSetEnabled(ghLabel1, ena);
	gwinSetEnabled(ghRadio1, ena);		gwinSetEnabled(ghRadio2, ena);
	gwinSetEnabled(ghRadioWhite, ena);	gwinSetEnabled(ghRadioBlack, ena);	gwinSetEnabled(ghRadioYellow, ena);
	gwinSetEnabled(ghImage1, ena);
}

int main(void) {
	GEvent *			pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// Connect the mouse
	#if GINPUT_NEED_MOUSE
		gwinAttachMouse(0);
	#endif

	// Create the gwin windows/widgets
	createWidgets();

    // Assign toggles and dials to specific buttons & sliders etc.
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

    // We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	// Press the Tab we want visible
	gwinRadioPress(ghTabButtons);

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

			// If it is the Disable All checkbox then do that.
			if (((GEventGWinCheckbox *)pe)->checkbox == ghCheckDisableAll) {
				gwinPrintf(ghConsole, "%s All\n", ((GEventGWinCheckbox *)pe)->isChecked ? "Disable" : "Enable");
				setEnabled(!((GEventGWinCheckbox *)pe)->isChecked);
			}
			break;

		case GEVENT_GWIN_RADIO:
			gwinPrintf(ghConsole, "Radio Group %u=%s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->radio));

			switch(((GEventGWinRadio *)pe)->group) {
			case GROUP_TABS:

				// Set control visibility depending on the tab selected
				setTab(((GEventGWinRadio *)pe)->radio);

				// Do some special animation for Label1 to demonstrate auto width sizing
				if (((GEventGWinRadio *)pe)->radio == ghTabLabels) {
					gwinPrintf(ghConsole, "Change Label Text\n");
					gfxSleepMilliseconds(1000);
					gwinSetText(ghLabel1, "Very Big Label", FALSE);

					gfxSleepMilliseconds(1000);
					gwinSetText(ghLabel1, "Label", FALSE);
				}
				break;

			case GROUP_COLORS:
				{
					const GWidgetStyle	*pstyle;

					gwinPrintf(ghConsole, "Change Color Scheme\n");

					if (((GEventGWinRadio *)pe)->radio == ghRadioYellow)
						pstyle = &YellowWidgetStyle;
					else if (((GEventGWinRadio *)pe)->radio == ghRadioBlack)
						pstyle = &BlackWidgetStyle;
					else
						pstyle = &WhiteWidgetStyle;

					// Clear the screen to the new color - we avoid the console area as it can't redraw itself
					#if GDISP_NEED_CLIP
						gdispUnsetClip();
					#endif
					gdispFillArea(0, 0, ScrWidth, ScrHeight/2, pstyle->background);
					gdispFillArea(0, ScrHeight/2, ScrWidth/2, ScrHeight/2, pstyle->background);

					// Update the style on all controls
					gwinSetDefaultStyle(pstyle, TRUE);
				}
				break;
			}
			break;

		default:
			gwinPrintf(ghConsole, "Unknown %d\n", pe->type);
			break;
		}
	}
	return 0;
}
