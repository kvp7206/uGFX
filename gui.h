#ifndef GUI_H
#define GUI_H

struct button_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint32_t *state;
	uint16_t interval;
};

struct keymatrix_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t off;
	uint16_t size;
	uint16_t digits;
	uint32_t *number;
	uint16_t interval;
};

/*
 * Description: starts main GUI thread which keeps X and Y coordinates of touchpad updated for guiDraw() threads
 *
 * param:
 *		- updateInterval: update interval in milliseconds until next coordinates read-out
 *
 * return: none
 */
void guiInit(uint16_t updateIntervl);

/*
 * Description:	draws button and creates thread which keeps pressed/unpressed state up-to-date
 *
 * param:
 *		- x0, y0, x1, y1: 	coordinates where button gets drawn
 *		- str:				string written centered into button
 *		- fontColor:		color of string
 *		- buttonColor:		color of button
 *		- state:			pointer to variable which keeps state (1 = pressed, 0 = unpressed)
 *
 * return: pointer to created thread
 */
Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint16_t inverval, uint8_t *state);

/*
 * Description: draws keymatrix
 *
 * param:
 *
 * return: pointer to created thread
 */
Thread *guiDrawKeymatrix(uint16_t x0, uint16_t y0, uint16_t size, uint16_t space, uint16_t fontColor, uint16_t buttonColor, uint16_t inverval, uint16_t digits, uint32_t *number);

#endif

