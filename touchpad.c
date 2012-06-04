#include "touchpad.h"
#include "glcd.h"

static struct cal cal;

static void spicb(SPIDriver *spip);
static const SPIConfig spicfg = {
	NULL,
	GPIOC,
	TP_CS,
	SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

void tpInit(void) {
	spiStart(&SPID1, &spicfg);	
}

static __inline uint16_t readX(void) {
	uint8_t txbuf[1];
	uint8_t rxbuf[2];
	uint16_t x;

	txbuf[0] = 0xd0;
	SET_CS(0);
	spiSend(&SPID1, 1, txbuf);
	spiReceive(&SPID1, 2, rxbuf);
	SET_CS(1);

	x = rxbuf[0] << 4;
	x |= rxbuf[1] >> 4;
	
	return x;
}

static __inline uint16_t readY(void) {
	uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0x90;
    SET_CS(0);
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    SET_CS(1);

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

	return y;
}

uint8_t __inline tpIRQ(void) {
	return (!palReadPad(TP_PORT, TP_IRQ));
}

static uint16_t tpReadRealX(void) {
	uint32_t results = 0;
	uint16_t i, x;

	for(i=0; i<CONVERSIONS; i++) {
		readX();	
		results += readX();
	}

	x = (((lcdGetHeight()-1) * (results/CONVERSIONS)) / 2048);

	return x;
}

static uint16_t tpReadRealY(void) {
	uint32_t results = 0;
	uint16_t i, y;

	for(i=0; i<CONVERSIONS; i++) {
		readY();
		results += readY();
	}

	y = (((lcdGetWidth()-1) * (results/CONVERSIONS)) / 2048);

	return y;
}

uint16_t tpReadX(void) {
	return cal.xm * tpReadRealX() + cal.xn;
}

uint16_t tpReadY(void) {
	return cal.ym * tpReadRealY() + cal.yn;
}

void tpDrawCross(uint16_t x, uint16_t y) {
	lcdDrawLine(x-15, y, x-2, y, 0xffff);
	lcdDrawLine(x+2, y, x+15, y, 0xffff);
	lcdDrawLine(x, y-15, x, y-2, 0xffff);
	lcdDrawLine(x, y+2, x, y+15, 0xffff);
  
	lcdDrawLine(x-15, y+15, x-7, y+15, RGB565CONVERT(184, 158, 131));
	lcdDrawLine(x-15, y+7, x-15, y+15, RGB565CONVERT(184, 158, 131));

	lcdDrawLine(x-15, y-15, x-7, y-15, RGB565CONVERT(184, 158, 131));
	lcdDrawLine(x-15, y-7, x-15, y-15, RGB565CONVERT(184, 158, 131));

	lcdDrawLine(x+7, y+15, x+15, y+15, RGB565CONVERT(184, 158, 131));
	lcdDrawLine(x+15, y+7, x+15, y+15, RGB565CONVERT(184, 158, 131));

	lcdDrawLine(x+7, y-15, x+15, y-15, RGB565CONVERT(184, 158, 131));
	lcdDrawLine(x+15, y-15, x+15, y-7, RGB565CONVERT(184, 158, 131));    
}

void tpCalibrate(void) {
	uint16_t cross[2][2] = {{40,40}, {200, 280}};
	uint16_t points[2][2];
	uint8_t i;
	char buffer[32];

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

