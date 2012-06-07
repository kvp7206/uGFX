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
	Thread *tp = NULL;
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(512), HIGHPRIO-1, TouchPadThread, NULL);
}

Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *state) {
	struct buttonStruct_t *buttonStruct;
	Thread *tp = NULL;

	buttonStruct = chHeapAlloc(NULL, sizeof(struct buttonStruct_t));

	buttonStruct->x0 = 100;
	buttonStruct->y0 = 100;
	buttonStruct->x1 = 200;
	buttonStruct->y1 = 200;
	buttonStruct->state = state;

	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(128), NORMALPRIO+1, buttonThread, buttonStruct);

	return tp;
}

