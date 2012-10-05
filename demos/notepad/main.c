#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchpad.h"

#define COLOR_SIZE	20
#define PEN_SIZE	20
#define OFFSET		3

#define COLOR_BOX(a)		(x >= a && x <= a + COLOR_SIZE)
#define PEN_BOX(a)			(y >= a && y <= a + COLOR_SIZE)
#define GET_COLOR(a)		(COLOR_BOX(a * COLOR_SIZE + OFFSET))
#define GET_PEN(a)			(PEN_BOX(a * 2 * PEN_SIZE + OFFSET))
#define DRAW_COLOR(a)		(a * COLOR_SIZE + OFFSET)
#define DRAW_PEN(a)			(a * 2 * PEN_SIZE + OFFSET)
#define DRAW_AREA(x, y)		(x >= PEN_SIZE + OFFSET + 3 && x <= gdispGetWidth() && \
							 y >= COLOR_SIZE + OFFSET + 3 && y <= gdispGetHeight())

static const SPIConfig spicfg = { 
    NULL,
    TP_CS_PORT,
    TP_CS,
    /* SPI_CR1_BR_2 | */ SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

TOUCHPADDriver TOUCHPADD1 = {
	&SPID1,
	&spicfg,
	TP_IRQ_PORT,
	TP_IRQ,
	TRUE
};

void drawScreen(void) {
	char *msg = "ChibiOS/GFX";
	uint16_t colorsize = COLOR_SIZE;
	uint16_t pensize = PEN_SIZE;

	gdispSetOrientation(GDISP_ROTATE_90);
	gdispClear(White);
	gdispDrawString(gdispGetWidth()-gdispGetStringWidth(msg, &fontUI2Double)-3, 3, msg, &fontUI2Double, Black);
	
	/* colors */
	gdispFillArea(0 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Black);	/* Black */
	gdispFillArea(1 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Red);		/* Red */
	gdispFillArea(2 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Yellow);	/* Yellow */
	gdispFillArea(3 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Green);	/* Green */
	gdispFillArea(4 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Blue);		/* Blue */
	gdispDrawBox (5 * COLOR_SIZE + 3, 3, COLOR_SIZE, COLOR_SIZE, Black);	/* White */

	/* pens */	
	gdispDrawString(OFFSET * 2, DRAW_PEN(1), "1", &fontLargeNumbers, Black);
	gdispDrawString(OFFSET * 2, DRAW_PEN(2), "2", &fontLargeNumbers, Black);
	gdispDrawString(OFFSET * 2, DRAW_PEN(3), "3", &fontLargeNumbers, Black);
	gdispDrawString(OFFSET * 2, DRAW_PEN(4), "4", &fontLargeNumbers, Black);
	gdispDrawString(OFFSET * 2, DRAW_PEN(5), "5", &fontLargeNumbers, Black);
}

int main(void) {
	volatile uint16_t x, y;
	color_t color = Black;
	uint16_t pen = 0;

	halInit();
	chSysInit();

	gdispInit();
	tpInit(&TOUCHPADD1);
	tpCalibrate();

	drawScreen();

	while (TRUE) {
		x = tpReadX();
		y = tpReadY();

		/* inside color box ? */
		if(y >= OFFSET && y <= COLOR_SIZE) {	 
			     if(GET_COLOR(0)) 	color = Black;
			else if(GET_COLOR(1))	color = Red;
			else if(GET_COLOR(2))	color = Yellow;
			else if(GET_COLOR(3))	color = Green;
			else if(GET_COLOR(4))	color = Blue;
			else if(GET_COLOR(5))	color = White;
		
		/* inside pen box ? */
		} else if(x >= OFFSET && x <= PEN_SIZE) {
			     if(GET_PEN(1))		pen = 0;
			else if(GET_PEN(2))		pen = 1;
			else if(GET_PEN(3))		pen = 2;
			else if(GET_PEN(4))		pen = 3;
			else if(GET_PEN(5))		pen = 4;		

		/* inside drawing area ? */
		} else if(DRAW_AREA(x, y)) {
			if(pen == 0)
				gdispDrawPixel(x, y, color);
			else
				gdispFillCircle(x, y, pen, color);
		}
	}
}

