#include "touchpad.h"
#include "glcd.h"

static struct cal cal;
static int16_t x_cal = 0, y_cal = 0;

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
	return cal.xm * tpReadRealX() - cal.xn;
}

uint16_t tpReadY(void) {
	return cal.ym * tpReadRealY() - cal.yn;
}

void tpDrawCross(uint16_t x, uint16_t y) {
	lcdDrawLine(x-15,y,x-2,y,0xffff);
	lcdDrawLine(x+2,y,x+15,y,0xffff);
	lcdDrawLine(x,y-15,x,y-2,0xffff);
	lcdDrawLine(x,y+2,x,y+15,0xffff);
  
	lcdDrawLine(x-15,y+15,x-7,y+15,RGB565CONVERT(184,158,131));
	lcdDrawLine(x-15,y+7,x-15,y+15,RGB565CONVERT(184,158,131));

	lcdDrawLine(x-15,y-15,x-7,y-15,RGB565CONVERT(184,158,131));
	lcdDrawLine(x-15,y-7,x-15,y-15,RGB565CONVERT(184,158,131));

	lcdDrawLine(x+7,y+15,x+15,y+15,RGB565CONVERT(184,158,131));
	lcdDrawLine(x+15,y+7,x+15,y+15,RGB565CONVERT(184,158,131));

	lcdDrawLine(x+7,y-15,x+15,y-15,RGB565CONVERT(184,158,131));
	lcdDrawLine(x+15,y-15,x+15,y-7,RGB565CONVERT(184,158,131));    
}

void tpCalibrate(void) {
	int16_t cross[2][2] = {{40,40}, {200, 280}};
	int16_t points[2][2];
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

	cal.xm = (cross[1][0] - cross[0][0]) / (points[1][0] - points[0][0]);
	cal.ym = (cross[1][1] - cross[0][1]) / (points[1][1] - points[0][1]);

	cal.xn = cross[0][0] - cal.xm * points[0][0];
	cal.yn = cross[0][1] - cal.ym * points[0][1];

	sprintf(buffer, "cal->xm = %d", cal.xm);
	lcdDrawString(50, 50, buffer, White, Red);
	sprintf(buffer, "cal->ym = %d", cal.ym);
	lcdDrawString(50, 70, buffer, White, Red);
	sprintf(buffer, "cal->xn = %d", cal.xn);
	lcdDrawString(50, 90, buffer, White, Red);
	sprintf(buffer, "cal->yn = %d", cal.yn);
	lcdDrawString(50, 110, buffer, White, Red);
}

void tpCalibrate2(void) {
	uint16_t cross[3][2] = {{20,40}, {220,160}, {50,300}};
	uint16_t cal[3][2];
	uint8_t i, j;
	int16_t a, b;
	unsigned char buffer[32];

	lcdClear(Red);
	lcdDrawString(40, 10, "Touchpad Calibration", White, Red);
	
	for(i=0; i<3; i++) {
		tpDrawCross(cross[i][0], cross[i][1]);
		while(!tpIRQ());
		cal[i][0] = tpReadX();
		cal[i][1] = tpReadY();
		while(tpIRQ());
		lcdDrawRect(cross[i][0]-15, cross[i][1]-15, cross[i][0]+16, cross[i][1]+16, filled, Red);
	}

	for(i=0, j=0; i<3; i++) {
		sprintf(buffer, "X: %d", cal[i][0]);
		lcdDrawString(100, 100+(i*20)+j, buffer, White, Red);
		sprintf(buffer, "Y: %d", cal[i][1]);
		lcdDrawString(100, 120+(i*20)+j, buffer, White, Red);
		j += 40;
	}

	for(a=0, b=0, i=0; i<3; i++) {
		a += (cross[i][0] - cal[i][0]);
		b += (cross[i][1] - cal[i][1]);
	}

	x_cal = (a / 3);
	y_cal = (b / 3);
}

