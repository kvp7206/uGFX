#include "gfx.h"

static GListener gl;
static GHandle   ghList1;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = TRUE;

	// Apply the list parameters
	wi.g.width = 300;
	wi.g.height = 200;
	wi.g.y = 10;
	wi.g.x = 10;
	wi.text = "List Name";

	// Create the actual list
	ghList1 = gwinListCreate(NULL, &wi);
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

	// Add some items to the list widget
	gwinListAddItem(ghList1, "Item 0", TRUE);
	gwinListAddItem(ghList1, "Item 1", TRUE);
	gwinListAddItem(ghList1, "Item 2", TRUE);
	gwinListAddItem(ghList1, "Item 3", TRUE);
	gwinListAddItem(ghList1, "Item 4", TRUE);

	gwinRedraw(ghList1);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

	}

	return 0;
}
