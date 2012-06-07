#ifndef GUI_H
#define GUI_H

struct buttonStruct_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint8_t *state;
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
Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *state);

#endif

