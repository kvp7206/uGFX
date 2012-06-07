#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

volatile uint16_t x, y;

static WORKING_AREA(waTouchPadThread, 512);
static msg_t TouchPadThread(void *arg) {
    (void)arg;
	unsigned char buffer[10];
	chRegSetThreadName("GUI");

	while(TRUE) {
		if(tpIRQ()) {
			x = tpReadX();
			y = tpReadY();

			lcdFillArea(10, 10, 80, 80, Black);
			sprintf(buffer, "X: %d", x);
			lcdDrawString(10, 10, buffer, White, Black);
			sprintf(buffer, "Y: %d", y);
			lcdDrawString(10, 25, buffer, White, Black);
		}

		chThdSleepMilliseconds(10);
	}
}


void guiInit(void) {
	chThdCreateStatic(TouchPadThread, sizeof(TouchPadThread), HIGHPRIO, TouchPadThread, NULL);	
}

void guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *state) {
	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);

}

