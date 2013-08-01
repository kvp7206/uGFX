#include "gfx.h"

static GListener gl;
static GHandle   ghList1;

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = FALSE;

	// Apply the list parameters
	wi.g.width = 100;
	wi.g.height = 80;
	wi.g.y = 10;
	wi.g.x = 10;
	wi.text = "List Name";

	// Create the actual list
	ghList1 = gwinListCreate(NULL, &wi, FALSE);
}

int main(void) {
	GEvent* pe;

	// Initialize the display
	gfxInit();

	// Set the widget defaults
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gdispClear(Red);

	// Attach the mouse input
	gwinAttachMouse(0);

	// create the widget
	createWidgets();

	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	// Add some items to the list widget
	gwinListAddItem(ghList1, "Item 0", FALSE);
	gwinListAddItem(ghList1, "Item 1", FALSE);
	gwinListAddItem(ghList1, "Item 2", FALSE);
	gwinListAddItem(ghList1, "Item 3", FALSE);
	gwinListAddItem(ghList1, "Item 4", FALSE);
	gwinListAddItem(ghList1, "Item 5", FALSE);
	gwinListAddItem(ghList1, "Item 6", FALSE);
	gwinListAddItem(ghList1, "Item 7", FALSE);
	gwinListAddItem(ghList1, "Item 8", FALSE);
	gwinListAddItem(ghList1, "Item 9", FALSE);
	gwinListAddItem(ghList1, "Item 10", FALSE);
	gwinListAddItem(ghList1, "Item 11", FALSE);
	gwinListAddItem(ghList1, "Item 12", FALSE);
	gwinListAddItem(ghList1, "Item 13", FALSE);

	gwinSetVisible(ghList1, TRUE);

	while(1) {
		// Get an Event
		pe = geventEventWait(&gl, TIME_INFINITE);

	}

	return 0;
}
