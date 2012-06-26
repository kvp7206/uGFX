#ifndef GUI_H
#define GUI_H

#include "ch.h"
#include "hal.h"
#include "glcd.h"
#include "touchpad.h"

static struct guiNode_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint8_t *active;
	uint8_t *state;
	char *name;
	struct guiNode_t *next;
};

#ifdef __cplusplus
extern "C" {
#endif

enum {horizontal, vertical};
enum {inactive, active};

Thread *guiInit(uint16_t interval);
void printNode(BaseSequentialStream *chp);
uint8_t guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *active, uint8_t *state);

#ifdef __cplusplus
}
#endif

#endif
