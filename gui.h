#ifndef GUI_H
#define GUI_H

struct buttonStruct_t {
	uint16_t x0;
	uint16_t y0;
	uint16_t x1;
	uint16_t y1;
	uint8_t *state;
};

Thread *guiDrawButton(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, unsigned char *str, uint16_t fontColor, uint16_t buttonColor, uint8_t *state);

#endif

