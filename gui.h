#ifndef GUI_H
#define GUI_H

#include "ch.h"
#include "hal.h"
#include "glcd.h"
#include "touchpad.h"

static struct guiNode_t {
	uint8_t type;
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

enum {button, slider, keymatrix};
enum {horizontal, vertical};
enum {inactive, active};

/*
 * Description: creates the GUI thread
 *
 * param:		- interval:	thread sleep in milliseconds after each GUI element update
 *				- priority: priority of the thread
 *
 * return:		pointer to created thread
 */
Thread *guiInit(uint16_t interval, tprio_t priority);

/*
 * Description: prints all GUI elements structs (linked list)
 * 
 * param:		- chp: pointer to output stream
 *
 * return:		none
 */
void guiPrintNode(BaseSequentialStream *chp);

/*
 * Description: draws a button on the screen and keeps it's state up to date
 *
 * param:		- x0, y0, x1, y1:	start and end coordinates of the button's rectangle
 *				- str:				string that gets drawn into the rectangle - button's lable
 *				- fontColor:		color of the lable
 *				- buttonColor:		color of the rectangle
 *				- active:			pass pointer to variable which holds the state 'active' or 'inactive'
 *				- state:			pass pointer to variable whcih will keep the state of the button (pressed / unpressed)'
 *
 * return:		1 if button successfully created
 */
uint8_t guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *active, uint8_t *state);

#ifdef __cplusplus
}
#endif

#endif
