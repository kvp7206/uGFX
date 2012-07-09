#ifndef GUI_H
#define GUI_H

#include "ch.h"
#include "hal.h"
#include "glcd.h"
#include "chprintf.h"
#include "touchpad.h"
#include <string.h>

struct guiNode_t {
	uint8_t type;
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint16_t r1;
	uint16_t r2;
	uint16_t shadow;
	uint16_t bkColor;
	uint16_t valueColor;
	uint8_t orientation;
	uint8_t *active;
	uint8_t *state;
	char *label;
	struct guiNode_t *next;
};

#ifdef __cplusplus
extern "C" {
#endif

enum {button, slider, wheel, keymatrix};
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
void guiPrintElements(BaseSequentialStream *chp);

/*
 * Description: deletes a GUI element from the linked list
 *
 * param:		- label: label of the element (parameter of each guiDrawXXX function)
 *
 * return:		1 if successful, 0 otherwise
 */
uint8_t guiDeleteElement(char *label);

/*
 * Description: draws a button on the screen and keeps it's state up to date
 *
 * param:		- x0, y0, x1, y1:	start and end coordinates of the button's rectangle
 *				- str:				string that gets drawn into the rectangle - button's lable
 *				- fontColor:		color of the lable
 *				- buttonColor:		color of the rectangle
 *				- shadow:			draws a black shadow with N pixels size if != 0 
 *				- active:			pass pointer to variable which holds the state 'active' or 'inactive'
 *				- state:			pass pointer to variable whcih will keep the state of the button (pressed / unpressed)'
 *
 * return:		1 if button successfully created
 */
uint8_t guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, font_t font, uint16_t fontColor, uint16_t buttonColor, uint16_t shadow, char *label, uint8_t *active, uint8_t *state);

uint8_t guiDrawSlider(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t orientation, uint16_t frameColor, uint16_t bkColor, uint16_t valueColor, char *label, uint8_t *active, uint8_t *value);

uint8_t guiDrawWheel(uint16_t x0, uint16_t y0, uint16_t radius1, uint16_t radius2, uint16_t bkColor, uint16_t valueColor, char *label, uint8_t *active, uint8_t *value);

uint8_t guiDrawKeymatrix(uint16_t x0, uint16_t y0, uint16_t buttonSize, uint16_t space, uint16_t shadow, uint16_t buttonColor, uint16_t fontColor, font_t font, char *label, uint8_t *active, uint8_t *value);

#ifdef __cplusplus
}
#endif

#endif
