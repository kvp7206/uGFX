#include "gfx.h"

static GListener	gl;
static GHandle		ghSlider1, ghSlider2;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = TRUE;

	// create Slider1
	wi.g.y = 10; wi.g.x = 10; wi.g.width = 200; wi.g.height = 20; wi.text = "S1";
	ghSlider1 = gwinSliderCreate(NULL, &wi);

	// create Slider2
	wi.g.y = 40; wi.g.x = 10; wi.g.width = 20; wi.g.height = 200; wi.text = "S2";
	ghSlider2 = gwinSliderCreate(NULL, &wi);
}

int main(void) {
	GEvent* pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(White);

	// Attach the mouse input
	gwinAttachMouse(0);

	// create the widget
	createWidgets();

	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
			case GEVENT_GWIN_SLIDER:
				printf("Slider %s = %d\n", gwinGetText(((GEventGWinSlider *)pe)->slider), ((GEventGWinSlider *)pe)->position);
				break;

			default:
				break;
		}
	}

	return 0;
}

