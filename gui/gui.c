/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui.h"

static struct guiNode_t *firstGUI = NULL;
uint16_t x, y; 	// global touchpad coordinates

static uint8_t addElement(struct guiNode_t *newNode) {
	struct guiNode_t *new;

	if(firstGUI == NULL) {
		firstGUI = chHeapAlloc(NULL, sizeof(struct guiNode_t));
		if(firstGUI == NULL)
			return 0;

		*firstGUI = *newNode;
		firstGUI->next = NULL;
	} else {
		new = firstGUI;
		while(new->next != NULL)
			new = new->next;

		new->next = chHeapAlloc(NULL, sizeof(struct guiNode_t));
		if(new->next == NULL)
			return 0;

		new = new->next;
		*new = *newNode;
		new->next = NULL;
	}

	return 1;
}

static uint8_t deleteElement(char *label) {
	struct guiNode_t *pointer, *pointer1;

	if(firstGUI != NULL) {
		if(strcmp(firstGUI->label, label) == 0) {
			pointer = firstGUI->next;
			chHeapFree(firstGUI);
			firstGUI = pointer;
		} else {
			pointer = firstGUI;

			while(pointer->next != NULL) {
				pointer1 = pointer->next;

				if(strcmp(firstGUI->label, label) == 0) {
					pointer->next = pointer1->next;
					chHeapFree(pointer1);
					break;
				}

				pointer = pointer1;

			}
		}

		return 1; // successful
	}

	return 0;	// not successful
}

void guiPrintElements(BaseSequentialStream *chp) {
	struct guiNode_t *pointer = firstGUI;

	chprintf(chp, "\r\n\nguiNodes:\r\n\n");

	while(pointer != NULL) {
		chprintf(chp, "x0:      %d\r\n", pointer->x0);
		chprintf(chp, "y0:      %d\r\n", pointer->y0);
		chprintf(chp, "x1:      %d\r\n", pointer->x1);
		chprintf(chp, "y1:      %d\r\n", pointer->y1);
		chprintf(chp, "label:    %s\r\n", pointer->label);
		chprintf(chp, "active:  %d\r\n", *(pointer->active));
		chprintf(chp, "state:   %d\r\n", *(pointer->state));
		chprintf(chp, "*next:   0x%x\r\n", pointer->next);
		chprintf(chp, "\r\n\n");
		pointer = pointer->next;
	}
}

static inline void buttonUpdate(struct guiNode_t *node) {
	if(x >= node->x0 && x <= node->x1 && y >= node->y0 && y <= node->y1) {
		*(node->state) = 1;
	} else {
		*(node->state) = 0;
	} 
}

static inline void sliderUpdate(struct guiNode_t *node) {
	uint16_t length = 1;

	if(node->orientation == horizontal)
		length = node->x1 - node->x0;
	else if(node->orientation == vertical)
		length = node->y1 - node->y0;

	if(node->mode == modePassive) {
		node->percentage = *(node->state);
	} else if(node->mode == modeActive) {
		if(x >= node->x0 && x <= node->x1 && y >= node->y0 && y <= node->y1) {
			if(node->orientation == horizontal) {
				node->percentage = (((x - node->x0) * 100) / length);
			} else if(node->orientation == vertical) {
				node->percentage = (((y - node->y0) * 100) / length);
			}
		}

		*(node->state) = node->percentage;
	}

	// a bit of safety here
	if(node->percentage > 100)
		node->percentage = 100;
	
	if(node->orientation == horizontal) {
		node->value = ((((node->x1)-(node->x0)) * node->percentage) / 100);
		if(node->oldValue > node->value || node->value == 0)
			lcdFillArea(node->x0+1, node->y0+1, node->x1, node->y1, node->bkColor);
		else
			lcdDrawRect(node->x0+1, node->y0+1, node->x0+node->value, node->y1, filled, node->valueColor);	
	} else if(node->orientation == vertical) {
		node->value = ((((node->y1)-(node->y0)) * node->percentage) / 100);
		if(node->oldValue > node->value || node->value == 0)
			lcdFillArea(node->x0+1, node->y0+1, node->x1, node->y1, node->bkColor);
		else
			lcdDrawRect(node->x0+1, node->y0+1, node->x1, node->y0+node->value, filled, node->valueColor);
	}

	node->oldValue = node->value;
}

static inline void wheelUpdate(struct guiNode_t *node) {
	(void)node;
}

static inline void keymatrixUpdate(struct guiNode_t *node) {
	(void)node;
}

static void guiThread(const uint16_t interval) {
	struct guiNode_t *node;

	chRegSetThreadName("GUI");

	while(TRUE) {
		for(node = firstGUI; node; node = node->next) {
			// check if GUI element is set active
			if(*(node->active) == active) {
				x = tpReadX();
				y = tpReadY();

				switch(node->type) {
					case button:
						buttonUpdate(node);
						break;
					case slider:
						sliderUpdate(node);
						break;
					case wheel:
						wheelUpdate(node);
						break;
					case keymatrix:
						keymatrixUpdate(node);
						break;
				}
			}
		}
		
		chThdSleepMilliseconds(interval);
	}
}

Thread *guiInit(uint16_t interval, tprio_t priority) {
	Thread *tp = NULL;

	tp = chThdCreateFromHeap(NULL, THD_WA_SIZE(512), priority, guiThread, interval);

	return tp;
}

uint8_t guiDeleteElement(char *label) {
	return deleteElement(label);
}

uint8_t guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, font_t font, uint16_t fontColor, uint16_t buttonColor, uint16_t shadow, char *label, uint8_t *active, uint8_t *state) {
	struct guiNode_t *newNode;
	uint16_t i;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;
	
	newNode->type = button;
	newNode->label = label;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->x1 = x1;
	newNode->y1 = y1;
	newNode->shadow = shadow;
	newNode->active = active;
	newNode->state = state;	

	if(addElement(newNode) != 1)
		return 0;
	
	lcdDrawRectString(x0, y0, x1, y1, str, font, fontColor, buttonColor);

	if(shadow != 0) {
		for(i = 0; i < shadow; i++) {
			lcdDrawLine(x0+shadow, y1+i, x1+shadow-1, y1+i, Black);
			lcdDrawLine(x1+i, y0+shadow, x1+i, y1+shadow-1, Black);
		}
	}

	chHeapFree(newNode);

	return 1;
}

uint8_t guiDrawSlider(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t orientation, uint8_t mode, uint16_t frameColor, uint16_t bkColor, uint16_t valueColor, char *label, uint8_t *active, uint8_t *value) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;

	newNode->type = slider;
	newNode->label = label;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->x1 = x1;
	newNode->y1 = y1;
	newNode->mode = mode;
	newNode->bkColor = bkColor;
	newNode->valueColor = valueColor;
	newNode->state = value;
	newNode->active = active;
	newNode->orientation = orientation;
	newNode->percentage = 0;

	if(addElement(newNode) != 1)
		return 0;

	(void)bkColor;
	(void)valueColor;
	
	// lcdDraw functions
	lcdDrawRect(x0, y0, x1, y1, frame, frameColor);
	
	chHeapFree(newNode);

	return 1;
}

uint8_t guiDrawWheel(uint16_t x0, uint16_t y0, uint16_t radius1, uint16_t radius2, uint16_t bkColor, uint16_t valueColor, char *label, uint8_t *active, uint8_t *value) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;

	newNode->type = wheel;
	newNode->label = label;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->r1 = radius1;
	newNode->r2 = radius2;
	newNode->active = active;
	newNode->state = value;

	if(addElement(newNode) != 1)
		return 0;

	(void)bkColor;
	(void)valueColor;
	// lcdDraw functions

	chHeapFree(newNode);

	return 1;
}

uint8_t guiDrawKeymatrix(uint16_t x0, uint16_t y0, uint16_t size, uint16_t space, uint16_t shadow, uint16_t buttonColor, uint16_t fontColor, font_t font, char *label, uint8_t *active, uint8_t *value) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;

	newNode->type = keymatrix;
	newNode->label = label;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->shadow = shadow;
	newNode->active = active;
	newNode->state = value;

	if(addElement(newNode) != 1)
		return 0;

	// lcdDraw functions
	(void)size;
	(void)space;
	(void)buttonColor;
	(void)fontColor;
	(void)font;

	chHeapFree(newNode);

	return 1;
}

