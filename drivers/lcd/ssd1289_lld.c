#include "ssd1289_lld.h"

#ifdef LCD_USE_SSD1289

uint8_t orientation;
uint16_t DeviceCode;
extern uint16_t lcd_width, lcd_height;

#ifdef LCD_USE_GPIO

static __inline void lld_lcdWriteIndex(uint16_t index) {
    Clr_RS;
    Set_RD;
  
	lld_lcdWriteGPIO(index);

    Clr_WR;
    Set_WR;
}

static __inline void lld_lcdWriteData(uint16_t data) {
	Set_RS;

	lld_lcdWriteGPIO(data);

	Clr_WR;
	Set_WR;
}

static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) { 
    Clr_CS;
    lld_lcdWriteIndex(lcdReg);    
    lld_lcdWriteData(lcdRegValue);  
    Set_CS; 
}

static __inline uint16_t lld_lcdReadData(void) {
	uint16_t value;

	Set_RS;
	Set_WR;
	Clr_RD;

	value = lld_lcdReadGPIO(LCD_DATA_PORT);

    Set_RD;

	return value;
}

static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
    uint16_t lcdRAM;

    Clr_CS;
    lld_lcdWriteIndex(lcdReg);
    lcdRAM = lld_lcdReadData();
                 
    Set_CS;

    return lcdRAM;
}

__inline void lld_lcdWriteStreamStart(void) {
	Clr_CS
	lld_lcdWriteIndex(0x0022);
}

__inline void lld_lcdWriteStreamStop(void) {
	Set_CS;
}

__inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
	uint16_t i;

	Set_RS;

	for(i = 0; i < size; i++) {
		lld_lcdWriteGPIO(buffer[i]);
		Clr_WR;
		Set_WR;
	}
}

#endif

#ifdef LCD_USE_SPI
	/* TODO */
#endif

#ifdef LCD_USE_FSMC

#define LCD_REG              (*((volatile uint16_t *) 0x60000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile uint16_t *) 0x60020000)) /* RS = 1 */

static __inline void lld_lcdWriteIndex(uint16_t index) {
	LCD_REG = index;
}

static __inline void lld_lcdWriteData(uint16_t data) {
	LCD_RAM = data;
}

static __inline void lld_lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) {
	LCD_REG = lcdReg;
	LCD_RAM = lcdRegValue;
}

static __inline uint16_t lld_lcdReadData(void) {
	return (LCD_RAM);
}

static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
	LCD_REG = lcdReg;
	return (LCD_RAM);
}

__inline void lld_lcdWriteStreamStart(void) {
	LCD_REG = 0x0022;
}

__inline void lld_lcdWriteStreamStop(void) {

}

__inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
	uint16_t i;
	for(i = 0; i < size; i++)
		LCD_RAM = buffer[i];
}
#endif

static __inline void lld_lcdDelay(uint16_t us) {
	chThdSleepMicroseconds(us);
}

void lld_lcdSetPowerMode(uint8_t powerMode) {
	switch(powerMode) {
		case powerOff:
			lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
			lld_lcdWriteReg(0x0007, 0x0000);	// halt operation
			lld_lcdWriteReg(0x0000, 0x0000);	// turn off oszillator
			lld_lcdWriteReg(0x0010, 0x0001);	// enter sleepmode
			break;
		case powerOn:
			lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
			lld_lcdInit();
			break;
		case sleepOn:
			lld_lcdWriteReg(0x0010, 0x0001);	// enter sleep mode
			break;
		case sleepOff:
			lld_lcdWriteReg(0x0010, 0x0000);	// leave sleep mode
			break;
	}
}

void lld_lcdSetCursor(uint16_t x, uint16_t y) {
	if(PORTRAIT) {
		lld_lcdWriteReg(0x004e, x); 
		lld_lcdWriteReg(0x004f, y); 
	} else if(LANDSCAPE) {
		lld_lcdWriteReg(0x004e, y); 
		lld_lcdWriteReg(0x004f, x); 
	} 
}

void lld_lcdSetOrientation(uint8_t newOrientation) {
    orientation = newOrientation;

    switch(orientation) {
        case portrait:
            lld_lcdWriteReg(0x0001, 0x2B3F); 
            lld_lcdWriteReg(0x0011, 0x6070);
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscape:
            lld_lcdWriteReg(0x0001, 0x293F);
            lld_lcdWriteReg(0x0011, 0x6078);
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
        case portraitInv:
            lld_lcdWriteReg(0x0001, 0x693F);
            lld_lcdWriteReg(0x0011, 0x6040);
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscapeInv:
            lld_lcdWriteReg(0x0001, 0x6B3F);
            lld_lcdWriteReg(0x0011, 0x6048);
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
    }
}

void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    lld_lcdSetCursor(x0, y0);

    switch(lcdGetOrientation()) {
        case portrait:
            lld_lcdWriteReg(0x44, ((x1-1) << 8) | x0);
            lld_lcdWriteReg(0x45, y0);
            lld_lcdWriteReg(0x46, y1-1);
            break;
        case landscape:
            lld_lcdWriteReg(0x44, ((y1-1) << 8) | y1);
            lld_lcdWriteReg(0x45, x0);
            lld_lcdWriteReg(0x46, x1-1);
            break;
        case portraitInv:
            lld_lcdWriteReg(0x44, ((x1-1) << 8) | x0);
            lld_lcdWriteReg(0x45, y0);
            lld_lcdWriteReg(0x46, y1-1);
            break;
        case landscapeInv:
            lld_lcdWriteReg(0x44, ((y1-1) << 8) | y1);
            lld_lcdWriteReg(0x45, x0);
            lld_lcdWriteReg(0x46, x1-1);
            break;
    }
}

void lld_lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint32_t index = 0, area;

	area = ((x1-x0)*(y1-y0));

    lld_lcdSetWindow(x0, y0, x1, y1);


    lld_lcdWriteStreamStart();

    for(index = 0; index < area; index++)
        lld_lcdWriteData(color);

    lld_lcdWriteStreamStop();
}

void lld_lcdClear(uint16_t color) {
    uint32_t index = 0;

    lld_lcdSetCursor(0, 0);
    lld_lcdWriteStreamStart();

    for(index = 0; index < SCREEN_WIDTH * SCREEN_HEIGHT; index++)
    	lld_lcdWriteData(color);

    lld_lcdWriteStreamStop();
}

uint16_t lld_lcdGetPixelColor(uint16_t x, uint16_t y) {
    uint16_t dummy;

    lld_lcdSetCursor(x,y);
    lld_lcdWriteStreamStart();

    dummy = lld_lcdReadData();
    dummy = lld_lcdReadData();

    lld_lcdWriteStreamStop();

	return dummy;
}

void lld_lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    lld_lcdSetCursor(x, y);
    lld_lcdWriteReg(0x0022, color);
}

void lld_lcdInit(void) {
#ifdef LCD_USE_FSMC
	/* FSMC setup. TODO: this only works for STM32F1 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);
	int FSMC_Bank = 0;
	/* timing structure */
	/* from datasheet:
        address setup: 0ns
        address hold: 0ns
        Data setup: 5ns
        Data hold: 5ns
        Data access: 250ns
        output hold: 100ns
	 */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = FSMC_BTR1_ADDSET_1 | FSMC_BTR1_DATAST_1;

	/* Bank1 NOR/SRAM control register configuration */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif
	DeviceCode = lld_lcdReadReg(0x0000);

	lld_lcdWriteReg(0x0000,0x0001);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0003,0xA8A4);    	lld_lcdDelay(5);   
    lld_lcdWriteReg(0x000C,0x0000);    	lld_lcdDelay(5);   
    lld_lcdWriteReg(0x000D,0x080C);    	lld_lcdDelay(5);   
    lld_lcdWriteReg(0x000E,0x2B00);    	lld_lcdDelay(5);   
    lld_lcdWriteReg(0x001E,0x00B0);    	lld_lcdDelay(5);  
	lld_lcdWriteReg(0x0001,0x2B3F);		lld_lcdDelay(5);
    lld_lcdWriteReg(0x0002,0x0600);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0010,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0011,0x6070);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0005,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0006,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0016,0xEF1C);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0017,0x0003);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0007,0x0133);    	lld_lcdDelay(5);         
    lld_lcdWriteReg(0x000B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x000F,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0041,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0042,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0048,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0049,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004A,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004B,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0044,0xEF00);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0045,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0046,0x013F);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0030,0x0707);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0031,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0032,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0033,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0034,0x0507);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0035,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0036,0x0204);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0037,0x0502);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003A,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x003B,0x0302);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0023,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0024,0x0000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x0025,0x8000);    	lld_lcdDelay(5);
    lld_lcdWriteReg(0x004f,0x0000);		lld_lcdDelay(5);      
    lld_lcdWriteReg(0x004e,0x0000);		lld_lcdDelay(5);
}

uint16_t lld_lcdGetOrientation(void) {
	return orientation;
}

uint16_t lld_lcdGetHeight(void) {
	return lcd_height;
}

uint16_t lld_lcdGetWidth(void) {
	return lcd_width;
}

#endif

