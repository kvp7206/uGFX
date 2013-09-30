#include "gfx.h"

static GListener gl;
static GHandle   ghCheckbox1;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = TRUE;

	// Apply the checkbox parameters	
	wi.g.width = 100;		// includes text
	wi.g.height = 20;
	wi.g.y = 10;
	wi.g.x = 10;
	wi.text = "Checkbox";

	// Create the actual checkbox 
	ghCheckbox1 = gwinCheckboxCreate(NULL, &wi);
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
			case GEVENT_GWIN_CHECKBOX:
				if (((GEventGWinCheckbox*)pe)->checkbox == ghCheckbox1) {
					// The state of our checkbox has changed
					printf("Checkbox state: %d\r\n", ((GEventGWinCheckbox*)pe)->isChecked);
				}
				break;

			default:
				break;
		}
	}

	return 0;
}

