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

static msg_t keymatrixThread(struct keymatrix_t *a) {
	uint16_t i, tpx, tpy, x0, y0, size, off;

	x0 = a->x0;
	y0 = a->y0;
	off = a->off;
	size = a->size;
	memset(a->number, 0, a->digits * sizeof(a->number));
	i = 0;

	while(TRUE) {
			while(!tpIRQ());	
			tpx = tpReadX();
			tpy = tpReadY();
			if(tpx >= x0 && tpx <= x0+size && tpy >= y0 && tpy <= y0+size)
				a->number[i] = 7;
			else if(tpx >= x0+off && tpx <= x0+off+size && tpy >= y0 && tpy <= y0+size)
				a->number[i] = 8;
			else if(tpx >= x0+off+off && tpx <= x0+off+off+size && tpy >= y0 && tpy <= y0+size)
				a->number[i] = 9;
			else if(tpx >= x0 && tpx <= x0+size && tpy >= y0+off && tpy <= y0+off+size)
				a->number[i] = 4;
			else if(tpx >= x0+off && tpx <= x0+off+size && tpy >= y0+off && tpy <= y0+off+size)
				a->number[i] = 5;
			else if(tpx >= x0+off+off && tpx <= x0+off+off+size && tpy >= y0+off && tpy <= y0+off+size)
				a->number[i] = 6;
			else if(tpx >= x0 && tpx <= x0+size && tpy >= y0+off+off && tpy <= y0+off+off+size)
				a->number[i] = 1;
			else if(tpx >= x0+off && tpx <= x0+off+size && tpy >= y0+off+off && tpy <= y0+off+off+size)
				a->number[i] = 2;
			else if(tpx >= x0+off+off && tpx <= x0+off+off+size && tpy >= y0+off+off && tpy <= y0+off+off+size)
				a->number[i] = 3;
			else if(tpx >= x0 && tpx <= x0+size && tpy >= y0+off+off+off && tpy <= y0+off+off+off+size)
				a->number[i] = 0;
			else if(tpx >= x0+off && tpx <= x0+off+size && tpy >= y0+off+off+off && tpy <= y0+off+off+off+size)
				a->number[i] = 0;
			else if(tpx >= x0+off+off && tpx <= x0+off+off+size && tpy >= y0+off+off+off && tpy <= y0+off+off+off+size)
				a->number[i] = 0;

			if(i >= a->digits) {
				i = 0;
				memset(a->number, 0, a->digits * sizeof(a->number));
			} else {
				i++;
			}
			
			while(tpIRQ());
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

Thread *guiDrawKeymatrix(uint16_t x0, uint16_t y0, uint16_t size, uint16_t space, uint16_t fontColor, uint16_t buttonColor, uint16_t interval, uint16_t digits, uint32_t *number) {
	struct keymatrix_t *keymatrix;
	Thread *tp = NULL;
	uint16_t off;
	uint8_t i, j;

	off = size+space;

	keymatrix = chHeapAlloc(NULL, sizeof(struct keymatrix_t));
	keymatrix->x0 = x0;
	keymatrix->y0 = y0;
	keymatrix->off = off;
	keymatrix->size = size;
	keymatrix->digits = digits;
	keymatrix->number = number;
	keymatrix->interval = interval;

	lcdDrawRectString(x0, y0, x0+size, y0+size, "7", fontColor, buttonColor);
	lcdDrawRectString(x0+off, y0, x0+off+size, y0+size, "8", fontColor, buttonColor);
	lcdDrawRectString(x0+off+off, y0, x0+off+off+size, y0+size, "9", fontColor, buttonColor);
	lcdDrawRectString(x0, y0+off, x0+size, y0+off+size, "4", fontColor, buttonColor);
	lcdDrawRectString(x0+off, y0+off, x0+off+size, y0+off+size, "5", fontColor, buttonColor);
	lcdDrawRectString(x0+off+off, y0+off, x0+off+off+size, y0+off+size, "6", fontColor, buttonColor);
	lcdDrawRectString(x0, y0+off+off, x0+size, y0+off+off+size, "1", fontColor, buttonColor);
	lcdDrawRectString(x0+off, y0+off+off, x0+off+size, y0+off+off+size, "2", fontColor, buttonColor);
	lcdDrawRectString(x0+off+off, y0+off+off, x0+off+off+size, y0+off+off+size, "3", fontColor, buttonColor);
	lcdDrawRectString(x0, y0+off+off+off, x0+size, y0+off+off+off+size, "*", fontColor, buttonColor);
	lcdDrawRectString(x0+off, y0+off+off+off, x0+off+size, y0+off+off+off+size, "0", fontColor, buttonColor);
	lcdDrawRectString(x0+off+off, y0+off+off+off, x0+off+off+size, y0+off+off+off+size, "#", fontColor, buttonColor);

	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(64), NORMALPRIO, keymatrixThread, keymatrix);

	return tp;
}

