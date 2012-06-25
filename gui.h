#ifndef GUI_H
#define GUI_H

struct button_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint8_t *state;
	uint8_t *active;
	uint16_t interval;
};

struct bar_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint16_t orientation;
	uint16_t frameColor;
	uint16_t bkColor;
	uint16_t valueColor;
	uint16_t interval;
	uint8_t *percent;
	uint8_t *active;
};

enum {horizontal, vertical};
enum {inactive, active};

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Description: starts main GUI thread which keeps X and Y coordinates of touchpad updated for guiDraw() threads
 *
 * param:
 *		- updateInterval: update interval in milliseconds until next coordinates read-out
 *
 * return: pointer to created thread
 */
Thread *guiInit(uint16_t updateIntervl);

/*
 * Description:	draws button and creates thread which keeps pressed/unpressed state up-to-date
 *
 * param:
 *		- x0, y0, x1, y1: 	coordinates where button gets drawn
 *		- str:				string written centered into button
 *		- fontColor:		color of string
 *		- buttonColor:		color of button
 *		- interval:			interval in ms which updates state
 *		- active:			pointer to uint8_t. 1 = thread active, 0 = thread inactive
 *		- state:			pointer to variable which keeps state (1 = pressed, 0 = unpressed)
 *
 * return: pointer to created thread
 */
Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, char *str, uint16_t fontColor, uint16_t buttonColor, uint16_t inverval, uint8_t *active, uint8_t *state);

/*
 * Description: draws a bar graph and updates it's value
 *
 * param:
 *		- x0, y0, x1, y1:	coordinates where bar graph gets drawn
 *		- orientation:		horizontal or vertical
 *		- frameColor:		color of the frame
 *		- bkColor:			color of piece inside bar with is not set
 *		- valueColor:		color of value that will be drawn into bar
 *		- interval:			interval in ms which updates percentage
 *		- active:			pointer to uint8_t. 1 = thread active, 0 = thread inactive
 *		- percent:			pointer value from 0 to 100 percent
 *
 * return : pointer to created thread
 */
Thread *guiDrawBarGraph(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t orientation, uint16_t frameColor, uint16_t bkColor, uint16_t valueColor, uint16_t interval, uint8_t *active, uint16_t *percent);

#ifdef __cplusplus
}
#endif

#endif

