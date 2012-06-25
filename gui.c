#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

uint16_t x, y;
unsigned char buffer[32];

static void TouchPadThread(uint16_t updateInterval) {
	chRegSetThreadName("GUI");

	while(TRUE) {
		x = tpReadX();
		y = tpReadY();

		chThdSleepMilliseconds(updateInterval);
	}
}

static void buttonThread(struct button_t *a) {
	uint16_t x0, y0, x1, y1;

	x0 = a->x0;
	y0 = a->y0;
	x1 = a->x1;
	y1 = a->y1;

	while(TRUE) {
		if(*(a->active) == active) {
			if(x >= x0 && x <= x1 && y >= y0 && y <= y1)
				*(a->state) = 1;
			else
				*(a->state) = 0;
		} else {
			*(a->state) = 0;
		}

		chThdSleepMilliseconds(a->interval);
	}
}

static void barThread(struct bar_t *a) {
	uint16_t percent = 0, value = 0, value_old = 0;

	while(TRUE) {
		if(*(a->active) == active) {
			percent = *(a->percent);
			if(percent > 100)
				percent = 100;	

			if(a->orientation == horizontal) {
				value = ((((a->x1)-(a->x0)) * percent) / 100);
				if(value_old > value || value == 0)
					lcdFillArea(a->x0+1, a->y0+1, a->x1, a->y1, a->bkColor);
				else
					lcdDrawRect(a->x0+1, a->y0+1, a->x0+value, a->y1, filled, a->valueColor);	
			} else if(a->orientation == vertical) {
				value = ((((a->y1)-(a->y0)) * percent) / 100);
				if(value_old > value || value == 0)
					lcdFillArea(a->x0+1, a->y0+1, a->x1, a->y1, a->bkColor);
				else
					lcdDrawRect(a->x0+1, a->y0+1, a->x1, a->y0+value, filled, a->valueColor);
			}

			value_old = value;
		}

		chThdSleepMilliseconds(a->interval);
	}
}

void guiInit(uint16_t updateInterval) {
	Thread *tp = NULL;
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), HIGHPRIO-1, TouchPadThread, updateInterval);
}

Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint16_t interval, uint8_t *active, uint8_t *state) {
	struct button_t *button;
	Thread *tp = NULL;

	button = chHeapAlloc(NULL, sizeof(struct button_t));
	button->x0 = x0;
	button->y0 = y0;
	button->x1 = x1;
	button->y1 = y1;
	button->state = state;
	button->active = active;
	button->interval = interval;

	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), NORMALPRIO, buttonThread, button);

	return tp;
}

Thread *guiDrawBarGraph(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t orientation, uint16_t frameColor, uint16_t bkColor, uint16_t valueColor, uint16_t interval, uint8_t *active, uint16_t *percent) {
	struct bar_t *bar;
	Thread *tp = NULL;

	bar = chHeapAlloc(NULL, sizeof(struct bar_t));
	bar->x0 = x0;
	bar->y0 = y0;
	bar->x1 = x1;
	bar->y1 = y1;
	bar->orientation = orientation;
	bar->frameColor = frameColor;
	bar->bkColor = bkColor;
	bar->valueColor = valueColor;
	bar->percent = percent;
	bar->interval = interval;
	bar->active = active;

	lcdDrawRect(x0, y0, x1, y1, frame, frameColor);
	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), NORMALPRIO, barThread, bar);

	return tp;
}
