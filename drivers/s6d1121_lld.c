#include "s6d1121_lld.h"

#ifdef LCD_USE_S6D1121

#define LCD_RST_LOW		palClearPad(LCD_RST_GPIO, LCD_RST_PIN)
#define LCD_RST_HIGH	palSetPad(LCD_RST_GPIO, LCD_RST_PIN)

#define LCD_CS_LOW		palClearPad(LCD_CS_GPIO, LCD_CS_PIN)
#define LCD_CS_HIGH		palSetPad(LCD_CS_GPIO, LCD_CS_PIN)

#define LCD_RS_LOW		palClearPad(LCD_RS_GPIO, LCD_RS_PIN)
#define LCD_RS_HIGH		palSetPad(LCD_RS_GPIO, LCD_RS_PIN)

#define LCD_RD_LOW		palClearPad(LCD_RD_GPIO, LCD_RD_PIN)
#define LCD_RD_HIGH		palSetPad(LCD_RD_GPIO, LCD_RD_PIN)

#define LCD_WR_LOW		palClearPad(LCD_WR_GPIO, LCD_WR_PIN)
#define LCD_WR_HIGH		palSetPad(LCD_WR_GPIO, LCD_WR_PIN)

#define LCD_BL_LOW		palClearPad(LCD_BL_GPIO, LCD_BL_PIN)
#define LCD_BL_HIGH		palSetPad(LCD_BL_GPIO, LCD_BL_PIN)

static uint8_t orientation;
extern uint16_t lcd_width, lcd_height;

inline void lld_lcddelay(void)
{
	asm volatile ("nop");
	asm volatile ("nop");
}

inline void lld_lcdwrite(uint16_t db)
{
	LCD_D4_GPIO->BSRR.W=((~db&0xFFF0)<<16)|(db&0xFFF0);
	LCD_D0_GPIO->BSRR.W=((~db&0x000F)<<16)|(db&0x000F);

	LCD_WR_LOW;
	lld_lcddelay();
	LCD_WR_HIGH;
}

static __inline uint16_t lcdReadData(void) {
	uint16_t value=0;

	LCD_RS_HIGH;
	LCD_WR_HIGH;
	LCD_RD_LOW;

#ifndef STM32F4XX
	// change pin mode to digital input
	LCD_DATA_PORT->CRH = 0x47444444;
	LCD_DATA_PORT->CRL = 0x47444444;
#else

#endif

//    value = palReadPort(LCD_DATA_PORT); // dummy
//    value = palReadPort(LCD_DATA_PORT);

#ifndef STM32F4XX
    // change pin mode back to digital output
    LCD_DATA_PORT->CRH = 0x33333333;
    LCD_DATA_PORT->CRL = 0x33333333;
#else
#endif
   	LCD_RD_HIGH;

	return value;
}

static __inline uint16_t lcdReadReg(uint16_t lcdReg) {
    uint16_t lcdRAM;

    LCD_CS_LOW;
    LCD_RS_LOW;
    lld_lcdwrite(lcdReg);
    LCD_RS_HIGH;
    lcdRAM = lcdReadData();

    LCD_CS_HIGH;

    return lcdRAM;
}

void lcdWriteIndex(uint16_t lcdReg) {
	LCD_RS_LOW;

	lld_lcdwrite(lcdReg);

	LCD_RS_HIGH;
}

void lcdWriteData(uint16_t lcdData) {
	lld_lcdwrite(lcdData);
}

void lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue) {
	LCD_CS_LOW;

	lcdWriteIndex(lcdReg);
	lcdWriteData(lcdRegValue);

	LCD_CS_HIGH;
}

void lld_lcdInit(void) {
	// ChibiOS HAL has a nice and precise delay function,
	// halPolledDelay(US2RTT(x)); or halPolledDelay(MS2RTT(x));
	// Why Not use it?
	// IO Default Configurations
	palSetPadMode(LCD_CS_GPIO, LCD_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_WR_GPIO, LCD_WR_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_RD_GPIO, LCD_RD_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_RST_GPIO, LCD_RST_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_RS_GPIO, LCD_RS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_BL_GPIO, LCD_BL_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	palSetGroupMode(LCD_D0_GPIO, 0x0000000F, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetGroupMode(LCD_D4_GPIO, 0x0000FFF0, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	LCD_CS_HIGH;
	LCD_RST_HIGH;
	LCD_RD_HIGH;
	LCD_WR_HIGH;
	LCD_BL_LOW;

	// A Good idea to reset the module before using
	LCD_RST_LOW;
	halPolledDelay(MS2RTT(2));
	LCD_RST_HIGH;         // Hardware Reset
	halPolledDelay(MS2RTT(2));

	lcdWriteReg(0x11,0x2004);
	lcdWriteReg(0x13,0xCC00);
	lcdWriteReg(0x15,0x2600);
	lcdWriteReg(0x14,0x252A);
	lcdWriteReg(0x12,0x0033);
	lcdWriteReg(0x13,0xCC04);

	halPolledDelay(MS2RTT(1));

	lcdWriteReg(0x13,0xCC06);

	halPolledDelay(MS2RTT(1));

	lcdWriteReg(0x13,0xCC4F);

	halPolledDelay(MS2RTT(1));

	lcdWriteReg(0x13,0x674F);
	lcdWriteReg(0x11,0x2003);

	halPolledDelay(MS2RTT(1));

	// Gamma Setting
	lcdWriteReg(0x30,0x2609);
	lcdWriteReg(0x31,0x242C);
	lcdWriteReg(0x32,0x1F23);
	lcdWriteReg(0x33,0x2425);
	lcdWriteReg(0x34,0x2226);
	lcdWriteReg(0x35,0x2523);
	lcdWriteReg(0x36,0x1C1A);
	lcdWriteReg(0x37,0x131D);
	lcdWriteReg(0x38,0x0B11);
	lcdWriteReg(0x39,0x1210);
	lcdWriteReg(0x3A,0x1315);
	lcdWriteReg(0x3B,0x3619);
	lcdWriteReg(0x3C,0x0D00);
	lcdWriteReg(0x3D,0x000D);

	lcdWriteReg(0x16,0x0007);
	lcdWriteReg(0x02,0x0013);
	lcdWriteReg(0x03,0x0003);
	lcdWriteReg(0x01,0x0127);

	halPolledDelay(MS2RTT(1));

	lcdWriteReg(0x08,0x0303);
	lcdWriteReg(0x0A,0x000B);
	lcdWriteReg(0x0B,0x0003);
	lcdWriteReg(0x0C,0x0000);
	lcdWriteReg(0x41,0x0000);
	lcdWriteReg(0x50,0x0000);
	lcdWriteReg(0x60,0x0005);
	lcdWriteReg(0x70,0x000B);
	lcdWriteReg(0x71,0x0000);
	lcdWriteReg(0x78,0x0000);
	lcdWriteReg(0x7A,0x0000);
	lcdWriteReg(0x79,0x0007);
	lcdWriteReg(0x07,0x0051);

	halPolledDelay(MS2RTT(1));

	lcdWriteReg(0x07,0x0053);
	lcdWriteReg(0x79,0x0000);
}

void lld_lcdSetCursor(uint16_t x, uint16_t y) {
	if(PORTRAIT) {
		lcdWriteReg(0x0020, x);
		lcdWriteReg(0x0021, y);
	} else if(LANDSCAPE) {
		lcdWriteReg(0x0020, y);
		lcdWriteReg(0x0021, x);
	}
}

// Do not use now, will be fixed in future
void lld_lcdSetOrientation(uint8_t newOrientation) {
    orientation = newOrientation;

    switch(orientation) {
        case portrait:
            lcdWriteReg(0x03, 0x03);
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscape:
        	// Not implemented yet
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
        case portraitInv:
        	// Not implemented yet
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscapeInv:
        	// Not implemented yet
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
    }
}

void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    lld_lcdSetCursor(x0, y0);

    switch(lcdGetOrientation()) {
        case portrait:
            lcdWriteReg(0x47, ((x0+x1-1) << 8) | x0);
            lcdWriteReg(0x48, y0);
            lcdWriteReg(0x47, y0+y1-1);
            break;
        case landscape:
            lcdWriteReg(0x47, ((y0+y1-1) << 8) | y1);
            lcdWriteReg(0x48, x0);
            lcdWriteReg(0x47, x0+x1-1);
            break;
        case portraitInv:
            lcdWriteReg(0x47, ((x0+x1-1) << 8) | x0);
            lcdWriteReg(0x48, y0);
            lcdWriteReg(0x47, y0+y1-1);
            break;
        case landscapeInv:
            lcdWriteReg(0x47, ((y0+y1-1) << 8) | y1);
            lcdWriteReg(0x48, x0);
            lcdWriteReg(0x47, x0+x1-1);
            break;
    }
}

void lld_lcdClear(uint16_t color) {
    uint32_t index = 0;

    lld_lcdSetCursor(0,0);
   	LCD_CS_LOW;
    lcdWriteIndex(0x0022);
    for(index = 0; index < SCREEN_WIDTH * SCREEN_HEIGHT; index++)
        lcdWriteData(color);
    LCD_CS_HIGH;
}

// Do not use!
uint16_t lld_lcdGetPixelColor(uint16_t x, uint16_t y) {
    uint16_t dummy;

    lld_lcdSetCursor(x,y);
    LCD_CS_LOW;
    lcdWriteIndex(0x0022);
    dummy = lcdReadData();
    dummy = lcdReadData();
    LCD_CS_HIGH;

	return dummy;
}

void lld_lcdDrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    lld_lcdSetCursor(x, y);
    lcdWriteReg(0x0022, color);
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

