#include "touchpad.h"
#include "glcd.h"

volatile static struct cal cal = {
	1, 1, 0, 0
};

static const SPIConfig spicfg = {
	NULL,
	GPIOC,
	TP_CS,
	SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

void tpInit(SPIDriver *spip) {
	spiStart(spip, &spicfg);	
}

static __inline uint16_t readX(void) {
	return lld_readX();
}

static __inline uint16_t readY(void) {
	return lld_readY();
}

uint8_t tpIRQ(void) {
	return (!palReadPad(TP_PORT, TP_IRQ));
}

static uint16_t tpReadRealX(void) {
	uint32_t results = 0;
	uint16_t i, x;

	for(i=0; i<CONVERSIONS; i++) {
		readX();	
		results += readX();
	}

	x = (((SCREEN_WIDTH-1) * (results/CONVERSIONS)) / 2048);

	return x;
}

static uint16_t tpReadRealY(void) {
	uint32_t results = 0;
	uint16_t i, y;

	for(i=0; i<CONVERSIONS; i++) {
		readY();
		results += readY();
	}

	y = (((SCREEN_HEIGHT-1) * (results/CONVERSIONS)) / 2048);

	return y;
}

uint16_t tpReadX(void) {
	uint16_t x, y;

	x = cal.xm * tpReadRealX() + cal.xn;
	y = cal.ym * tpReadRealY() + cal.yn;

	switch(lcdGetOrientation()) {
		case portrait:
			return x;
		case landscape:
			return SCREEN_HEIGHT - y;
		case portraitInv:
			return SCREEN_WIDTH - x;
		case landscapeInv:
			return y;
	}

	return x;
}

uint16_t tpReadY(void) {
	uint16_t x, y;

	x = cal.xm * tpReadRealX() + cal.xn;
	y = cal.ym * tpReadRealY() + cal.yn;

	switch(lcdGetOrientation()) {
		case portrait:
			return y;
		case landscape:
			return x;
		case portraitInv:
			return SCREEN_HEIGHT - y;
		case landscapeInv:
			return SCREEN_WIDTH - x;
	}

	return y;
}

static void tpDrawCross(uint16_t x, uint16_t y) {
	lcdDrawLine(x-15, y, x-2, y, 0xffff);
	lcdDrawLine(x+2, y, x+15, y, 0xffff);
	lcdDrawLine(x, y-15, x, y-2, 0xffff);
	lcdDrawLine(x, y+2, x, y+15, 0xffff);
  
	lcdDrawLine(x-15, y+15, x-7, y+15, RGB565CONVERT(184,158,131));
	lcdDrawLine(x-15, y+7, x-15, y+15, RGB565CONVERT(184,158,131));

	lcdDrawLine(x-15, y-15, x-7, y-15, RGB565CONVERT(184,158,131));
	lcdDrawLine(x-15, y-7, x-15, y-15, RGB565CONVERT(184,158,131));

	lcdDrawLine(x+7, y+15, x+15, y+15, RGB565CONVERT(184,158,131));
	lcdDrawLine(x+15, y+7, x+15, y+15, RGB565CONVERT(184,158,131));

	lcdDrawLine(x+7, y-15, x+15, y-15, RGB565CONVERT(184,158,131));
	lcdDrawLine(x+15, y-15, x+15, y-7, RGB565CONVERT(184,158,131));    
}

void tpCalibrate(void) {
	uint16_t cross[2][2] = {{40,40}, {200, 280}};
	uint16_t points[2][2];
	uint8_t i;

	lcdSetOrientation(portrait);
	lcdClear(Red);
	lcdDrawString(40, 10, "Touchpad Calibration", White, Red);

	for(i=0; i<2; i++) {
		tpDrawCross(cross[i][0], cross[i][1]);
		while(!tpIRQ());
		points[i][0] = tpReadRealX();
		points[i][1] = tpReadRealY();
		while(tpIRQ());
		lcdFillArea(cross[i][0]-15, cross[i][1]-15, cross[i][0]+16, cross[i][1]+16, Red);
	}

	cal.xm = ((float)cross[1][0] - (float)cross[0][0]) / ((float)points[1][0] - (float)points[0][0]);
	cal.ym = ((float)cross[1][1] - (float)cross[0][1]) / ((float)points[1][1] - (float)points[0][1]);

	cal.xn = (float)cross[0][0] - cal.xm * (float)points[0][0];
	cal.yn = (float)cross[0][1] - cal.ym * (float)points[0][1];
}

