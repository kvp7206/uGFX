#include "gfx.h"

#define GROUP1		0	// this will be the first radio button group
#define GROUP2		1	// this will be the second radio button group

static GListener	gl;
static GHandle		ghRadio11, ghRadio12, ghRadio13;	// the first group
static GHandle		ghRadio21, ghRadio22, ghRadio23;	// the second group

static void createWidgets(void) {
	GWidgetInit	wi;

	// Apply some default values for GWIN
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	wi.g.show = TRUE;

	// create Radio11
	wi.g.y = 10; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 1";
	ghRadio11 = gwinRadioCreate(NULL, &wi, GROUP1);

	// create Radio12
	wi.g.y = 50; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 2";
	ghRadio12 = gwinRadioCreate(NULL, &wi, GROUP1);

	// create Radio13
	wi.g.y = 90; wi.g.x = 10; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 3";
	ghRadio13 = gwinRadioCreate(NULL, &wi, GROUP1);

	// create Radio21
	wi.g.y = 10; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 1";
	ghRadio21 = gwinRadioCreate(NULL, &wi, GROUP2);

	// create Radio22
	wi.g.y = 50; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 2";
	ghRadio22 = gwinRadioCreate(NULL, &wi, GROUP2);

	// create Radio23
	wi.g.y = 90; wi.g.x = 120; wi.g.width = 80; wi.g.height = 20; wi.text = "Radio 3";
	ghRadio23 = gwinRadioCreate(NULL, &wi, GROUP2);

	// Check the first radio buttons
	gwinRadioPress(ghRadio11);
	gwinRadioPress(ghRadio21);
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
			case GEVENT_GWIN_RADIO:
				printf("group: %u   radio: %s\n", ((GEventGWinRadio *)pe)->group, gwinGetText(((GEventGWinRadio *)pe)->radio));
				break;

			default:
				break;
		}
	}

	return 0;
}

