#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

volatile uint16_t x, y;

static msg_t buttonThread(struct button_t *a) {
	uint16_t x0, y0, x1, y1;

	x0 = a->x0;
	y0 = a->y0;
	x1 = a->x1;
	y1 = a->y1;

	while(TRUE) {
		if(x >= x0 && x <= x1 && y >= y0 && y <= y1)
			*(a->state) = 1;
		else
			*(a->state) = 0;

		chThdSleepMilliseconds(a->interval);
	}
}

static msg_t TouchPadThread(uint16_t updateInterval) {
	chRegSetThreadName("GUI");

	while(TRUE) {
		x = tpReadX();
		y = tpReadY();

		chThdSleepMilliseconds(updateInterval);
	}
}


void guiInit(uint16_t updateInterval) {
	Thread *tp = NULL;
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), HIGHPRIO-1, TouchPadThread, updateInterval);
}

Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint16_t interval, uint8_t *state) {
	struct button_t *button;
	Thread *tp = NULL;

	button = chHeapAlloc(NULL, sizeof(struct button_t));
	button->x0 = x0;
	button->y0 = y0;
	button->x1 = x1;
	button->y1 = y1;
	button->state = state;
	button->interval = interval;

	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(512), NORMALPRIO, buttonThread, button);

	return tp;
}

