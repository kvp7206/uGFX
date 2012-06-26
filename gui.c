#include "gui.h"

static struct guiNode_t *firstGUI = NULL;

static uint16_t addNode(struct guiNode_t *newNode) {
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

static void deleteNode(char *name) {
	struct guiNode_t *pointer, *pointer1;

	if(firstGUI != NULL) {
		if(strcmp(firstGUI->name, name) == 0) {
			pointer = firstGUI->next;
			chHeapFree(firstGUI);
			firstGUI = pointer;
		} else {
			pointer = firstGUI;

			while(pointer->next != NULL) {
				pointer1 = pointer->next;

				if(strcmp(firstGUI->name, name) == 0) {
					pointer->next = pointer1->next;
					chHeapFree(pointer1);
					break;
				}

				pointer = pointer1;

			}
		}
	}
}

void guiPrintNodes(BaseSequentialStream *chp) {
	struct guiNode_t *pointer = firstGUI;

	chprintf(chp, "\r\n\nguiNodes:\r\n\n");

	while(pointer != NULL) {
		chprintf(chp, "x0:      %d\r\n", pointer->x0);
		chprintf(chp, "y0:      %d\r\n", pointer->y0);
		chprintf(chp, "x1:      %d\r\n", pointer->x1);
		chprintf(chp, "y1:      %d\r\n", pointer->y1);
		chprintf(chp, "name:    %s\r\n", pointer->name);
		chprintf(chp, "active:  %d\r\n", *(pointer->active));
		chprintf(chp, "state:   %d\r\n", *(pointer->state));
		chprintf(chp, "*next:   0x%x\r\n", pointer->next);
		chprintf(chp, "\r\n\n");
		pointer = pointer->next;
	}
}

static void guiThread(const uint16_t interval) {
	uint16_t x, y;
	struct guiNode_t *node;

	chRegSetThreadName("GUI");

	while(TRUE) {
		for(node = firstGUI; node; node = node->next) {
			// check if GUI element is active
			if(*(node->active) == active) {
				x = tpReadX();
				y = tpReadY();

				// we got a button
				if(node->type == button) {
					if(x >= node->x0 && x <= node->x1 && y >= node->y0 && y <= node->y1)
						*(node->state) = 1;
					else
						*(node->state) = 0;
				} else {
					*(node->state) = 0;
				}

				// we got a slider
				if(node->type == slider) {

				}

				// we got a wheel
				if(node->type == wheel) {

				}

				// we got a keymatrix
				if(node->type == keymatrix) {

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

uint8_t guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, uint16_t fontColor, uint16_t buttonColor, char *name, uint8_t *active, uint8_t *state) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;
	
	newNode->type = button;
	newNode->name = name;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->x1 = x1;
	newNode->y1 = y1;
	newNode->name = str;
	newNode->active = active;
	newNode->state = state;	

	if(addNode(newNode) != 1)
		return 0;
	
	lcdDrawRectString(x0, y0, x1, y1, str, fontColor, buttonColor);

	chHeapFree(newNode);

	return 1;
}

uint8_t guiDrawSlider(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t orientation, uint16_t frameColor, uint16_t bkColor, uint16_t valueColor, char *name, uint8_t *active, uint8_t *value) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;

	newNode->type = slider;
	newNode->type = name;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->x1 = x1;
	newNode->y1 = y1;
	newNode->state = value;
	newNode->active = active;
	newNode->orientation = orientation;

	if(addNode(newNode) != 1)
		return 0;

	lcdDrawRect(x0, y0, x1, y1, frame, frameColor);

	chHeapFree(newNode);

	return 1;
}

uint8_t guiDrawWheel(uint16_t x0, uint16_t y0, uint16_t radius1, uint16_t radius2, uint16_t bkColor, uint16_t valueColor, char *name, uint8_t *active, uint8_t *value) {
	struct guiNode_t *newNode;

	newNode = chHeapAlloc(NULL, sizeof(struct guiNode_t));
	if(newNode == NULL)
		return 0;

	newNode->type = wheel;
	newNode->name = name;
	newNode->x0 = x0;
	newNode->y0 = y0;
	newNode->r1 = radius1;
	newNode->r2 = radius2;
	newNode->active = active;
	newNode->state = value;

	if(addNode(newNode) != 1)
		return 0;

	// lcdDraw functions

	chHeapFree(newNode);

	return 1;
}

