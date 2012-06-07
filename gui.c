#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

volatile uint16_t x, y;

static msg_t buttonThread(struct buttonStruct_t *a) {
	while(TRUE) {
		if(x >= a->x0 && x <= a->x1 && y >= a->y0 && y <= a->y1)
			*(a->state) = 1;
		else
			*(a->state) = 0;
		chThdSleepMilliseconds(50);
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

Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *state) {
	struct buttonStruct_t *buttonStruct;
	Thread *tp = NULL;

	buttonStruct = chHeapAlloc(NULL, sizeof(struct buttonStruct_t));

	buttonStruct->x0 = x0;
	buttonStruct->y0 = y0;
	buttonStruct->x1 = x1;
	buttonStruct->y1 = y1;
	buttonStruct->state = state;

	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), NORMALPRIO+1, buttonThread, buttonStruct);

	return tp;
}

