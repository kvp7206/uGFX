#include "ch.h"
#include "hal.h"

#include "s6d1121_lld.h"
#include "chprintf.h"

#ifdef LCD_USE_S6D1121

static uint8_t orientation;
extern uint16_t lcd_width, lcd_height;

/* all interfaces use RST via GPIO */
/* TODO: option to disable RST; assumes RST is tied high */
#define LCD_RST_LOW		palClearPad(LCD_RST_GPIO, LCD_RST_PIN)
#define LCD_RST_HIGH	palSetPad(LCD_RST_GPIO, LCD_RST_PIN)

#define s6d1121_delay(n) 	halPolledDelay(MS2RTT(n));

static uint16_t buf[((SCREEN_HEIGHT > SCREEN_WIDTH ) ? SCREEN_HEIGHT : SCREEN_WIDTH)];

#if defined(LCD_USE_GPIO)

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


static inline void lld_lcddelay(void)
{
	asm volatile ("nop");
	asm volatile ("nop");
}

static inline void lld_lcdwrite(uint16_t db)
{
	LCD_D4_GPIO->BSRR.W=((~db&0xFFF0)<<16)|(db&0xFFF0);
	LCD_D0_GPIO->BSRR.W=((~db&0x000F)<<16)|(db&0x000F);

	LCD_WR_LOW;
	lld_lcddelay();
	LCD_WR_HIGH;
}

static __inline uint16_t lld_lcdReadData(void) {
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

#ifndef STM32F4XX
    // change pin mode back to digital output
    LCD_DATA_PORT->CRH = 0x33333333;
    LCD_DATA_PORT->CRL = 0x33333333;
#else
#endif
   	LCD_RD_HIGH;

	return value;
}

static __inline uint16_t lld_lcdReadReg(uint16_t lcdReg) {
    uint16_t lcdRAM;

    LCD_CS_LOW;
    LCD_RS_LOW;
    lld_lcdwrite(lcdReg);
    LCD_RS_HIGH;
    lcdRAM = lld_lcdReadData();

    LCD_CS_HIGH;

    return lcdRAM;
}

void lld_lcdWriteIndex(uint16_t lcdReg) {
	LCD_RS_LOW;

	lld_lcdwrite(lcdReg);

	LCD_RS_HIGH;
}

void lld_lcdWriteData(uint16_t lcdData) {
	lld_lcdwrite(lcdData);
}

void lld_lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue) {
	LCD_CS_LOW;

	lld_lcdWriteIndex(lcdReg);
	lld_lcdWriteData(lcdRegValue);

	LCD_CS_HIGH;
}

static __inline void lld_lcdWriteStreamStart(void) {
	LCD_CS_LOW;
	lld_lcdWriteIndex(0x0022);
}

static __inline void lld_lcdWriteStreamStop(void) {
	LCD_CS_HIGH;
}

__inline void lld_lcdWriteStream(uint16_t *buffer, uint16_t size) {
	uint16_t i;

	for(i = 0; i < size; i++) {
		lld_lcdwrite(buffer[i]);
	}
}

__inline void lld_lcdReadStreamStart(void) {
	/* TODO */
}

__inline void lld_lcdReadStreamStop(void) {
	/* TODO */
}

__inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
	/* TODO */
}

#elif defined(LCD_USE_FSMC)

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
	return LCD_RAM;
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

__inline void lld_lcdReadStreamStart(void) {
	LCD_REG = 0x0022;
}

__inline void lld_lcdReadStreamStop(void) {

}

__inline void lld_lcdReadStream(uint16_t *buffer, size_t size) {
	uint16_t i;
	/* throw away first value read */
	volatile uint16_t dummy = LCD_RAM;

	for(i = 0; i < size; i++) {
		buffer[i] = LCD_RAM;
	}
}

#endif

void lld_lcdSetPowerMode(uint8_t powerMode) {
	/* TODO: implement */
}

void lld_lcdInit(void) {
	palSetPadMode(LCD_RST_GPIO, LCD_RST_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	// A Good idea to reset the module before using
	LCD_RST_LOW;
	s6d1121_delay(2);
	LCD_RST_HIGH;         // Hardware Reset
	s6d1121_delay(2);

#ifdef LCD_USE_GPIO
	// IO Default Configurations
	palSetPadMode(LCD_CS_GPIO, LCD_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_WR_GPIO, LCD_WR_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_RD_GPIO, LCD_RD_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_RS_GPIO, LCD_RS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetPadMode(LCD_BL_GPIO, LCD_BL_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	palSetGroupMode(LCD_D0_GPIO, 0x0000000F, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);
	palSetGroupMode(LCD_D4_GPIO, 0x0000FFF0, 0, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);

	LCD_CS_HIGH;
	LCD_RD_HIGH;
	LCD_WR_HIGH;
	LCD_BL_LOW;


#elif defined(LCD_USE_FSMC)
#if defined(STM32F1XX)
	/* FSMC setup. TODO: this only works for STM32F1 */
	rccEnableAHB(RCC_AHBENR_FSMCEN, 0);

	/* TODO: pin setup */
#elif defined(STM32F4XX)
	/* STM32F4 FSMC init */
	rccEnableAHB3(RCC_AHB3ENR_FSMCEN, 0);

	/* set pins to FSMC mode */
	IOBus busD = {GPIOD, (1 << 0) | (1 << 1) | (1 << 4) | (1 << 5) | (1 << 7) | (1 << 8) |
							(1 << 9) | (1 << 10) | (1 << 11) | (1 << 14) | (1 << 15), 0};

	IOBus busE = {GPIOE, (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11) | (1 << 12) |
						(1 << 13) | (1 << 14) | (1 << 15), 0};

	palSetBusMode(&busD, PAL_MODE_ALTERNATE(12));
	palSetBusMode(&busE, PAL_MODE_ALTERNATE(12));
#else
#error "FSMC not implemented for this device"
#endif
	int FSMC_Bank = 0;
	/* FSMC timing */
	FSMC_Bank1->BTCR[FSMC_Bank+1] = (10) | (10 << 8) | (10 << 16);

	/* Bank1 NOR/SRAM control register configuration */
	FSMC_Bank1->BTCR[FSMC_Bank] = FSMC_BCR1_MWID_0 | FSMC_BCR1_WREN | FSMC_BCR1_MBKEN;
#endif

	lld_lcdWriteReg(0x11,0x2004);
	lld_lcdWriteReg(0x13,0xCC00);
	lld_lcdWriteReg(0x15,0x2600);
	lld_lcdWriteReg(0x14,0x252A);
	lld_lcdWriteReg(0x12,0x0033);
	lld_lcdWriteReg(0x13,0xCC04);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0xCC06);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0xCC4F);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x13,0x674F);
	lld_lcdWriteReg(0x11,0x2003);

	s6d1121_delay(1);

	// Gamma Setting
	lld_lcdWriteReg(0x30,0x2609);
	lld_lcdWriteReg(0x31,0x242C);
	lld_lcdWriteReg(0x32,0x1F23);
	lld_lcdWriteReg(0x33,0x2425);
	lld_lcdWriteReg(0x34,0x2226);
	lld_lcdWriteReg(0x35,0x2523);
	lld_lcdWriteReg(0x36,0x1C1A);
	lld_lcdWriteReg(0x37,0x131D);
	lld_lcdWriteReg(0x38,0x0B11);
	lld_lcdWriteReg(0x39,0x1210);
	lld_lcdWriteReg(0x3A,0x1315);
	lld_lcdWriteReg(0x3B,0x3619);
	lld_lcdWriteReg(0x3C,0x0D00);
	lld_lcdWriteReg(0x3D,0x000D);

	lld_lcdWriteReg(0x16,0x0007);
	lld_lcdWriteReg(0x02,0x0013);
	lld_lcdWriteReg(0x03,0x0003);
	lld_lcdWriteReg(0x01,0x0127);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x08,0x0303);
	lld_lcdWriteReg(0x0A,0x000B);
	lld_lcdWriteReg(0x0B,0x0003);
	lld_lcdWriteReg(0x0C,0x0000);
	lld_lcdWriteReg(0x41,0x0000);
	lld_lcdWriteReg(0x50,0x0000);
	lld_lcdWriteReg(0x60,0x0005);
	lld_lcdWriteReg(0x70,0x000B);
	lld_lcdWriteReg(0x71,0x0000);
	lld_lcdWriteReg(0x78,0x0000);
	lld_lcdWriteReg(0x7A,0x0000);
	lld_lcdWriteReg(0x79,0x0007);
	lld_lcdWriteReg(0x07,0x0051);

	s6d1121_delay(1);

	lld_lcdWriteReg(0x07,0x0053);
	lld_lcdWriteReg(0x79,0x0000);

	lld_lcdResetWindow();
}

void lld_lcdSetCursor(uint16_t x, uint16_t y) {
	/* R20h - 8 bit
	 * R21h - 9 bit
	 */
	switch(lcdGetOrientation()) {
		case portraitInv:
			lld_lcdWriteReg(0x0020, (SCREEN_WIDTH-1-x) & 0x00FF);
			lld_lcdWriteReg(0x0021, (SCREEN_HEIGHT-1-y) & 0x01FF);
			break;
		case portrait:
			lld_lcdWriteReg(0x0020, x & 0x00FF);
			lld_lcdWriteReg(0x0021, y & 0x01FF);
			break;
		case landscape:
			lld_lcdWriteReg(0x0020, y & 0x00FF);
			lld_lcdWriteReg(0x0021, x & 0x01FF);
			break;
		case landscapeInv:
			lld_lcdWriteReg(0x0020, (SCREEN_WIDTH - y - 1) & 0x00FF);
			lld_lcdWriteReg(0x0021, (SCREEN_HEIGHT - x - 1) & 0x01FF);
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

    lld_lcdResetWindow();
}

// Do not use now, will be fixed in future
void lld_lcdSetOrientation(uint8_t newOrientation) {
    orientation = newOrientation;

    switch(orientation) {
        case portrait:
        	lld_lcdWriteReg(0x0001,0x0127);
            lld_lcdWriteReg(0x03, 0b0011);
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscape:
        	lld_lcdWriteReg(0x0001,0x0027);
        	lld_lcdWriteReg(0x0003, 0b1011);
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
        case portraitInv:
        	lld_lcdWriteReg(0x0001,0x0127);
        	lld_lcdWriteReg(0x0003, 0b0000);
            lcd_height = SCREEN_HEIGHT;
            lcd_width = SCREEN_WIDTH;
            break;
        case landscapeInv:
        	lld_lcdWriteReg(0x0001,0x0027);
        	lld_lcdWriteReg(0x0003, 0b1000);
            lcd_height = SCREEN_WIDTH;
            lcd_width = SCREEN_HEIGHT;
            break;
    }
}

void lld_lcdResetWindow(void) {
	switch(lcdGetOrientation()) {
        case portrait:
        case portraitInv:
        	lld_lcdSetWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case landscape:
        case landscapeInv:
        	lld_lcdSetWindow(0, 0, SCREEN_HEIGHT, SCREEN_WIDTH);
            break;
    }
}

void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	/* HSA / HEA are 8 bit
	 * VSA / VEA are 9 bit
	 * use masks 0x00FF and 0x01FF to enforce this
	 */
	switch(lcdGetOrientation()) {
        case portrait:
            lld_lcdWriteReg(0x46, (((x1-1) & 0x00FF) << 8) | (x0 & 0x00FF));
            lld_lcdWriteReg(0x48, y0 & 0x01FF);
            lld_lcdWriteReg(0x47, (y1-1) & 0x01FF);
            break;
        case landscape:
            lld_lcdWriteReg(0x46, (((y1-1) & 0x00FF) << 8) | (y1 & 0x00FF));
            lld_lcdWriteReg(0x48, x0 & 0x01FF);
            lld_lcdWriteReg(0x47, (x1-1) & 0x01FF);
            break;
        case portraitInv:
            lld_lcdWriteReg(0x46, (((SCREEN_WIDTH-x0-1) & 0x00FF) << 8) | ((SCREEN_WIDTH - x1) & 0x00FF));
            lld_lcdWriteReg(0x48, (SCREEN_HEIGHT-y1) & 0x01FF);
            lld_lcdWriteReg(0x47, (SCREEN_HEIGHT-y0-1) & 0x01FF);
            break;
        case landscapeInv:
            lld_lcdWriteReg(0x46, (((SCREEN_WIDTH - y0 - 1) & 0x00FF) << 8) | ((SCREEN_WIDTH - y1) & 0x00FF));
            lld_lcdWriteReg(0x48, (SCREEN_HEIGHT - x1) & 0x01FF);
            lld_lcdWriteReg(0x47, (SCREEN_HEIGHT - x0 - 1) & 0x01FF);
            break;
    }

	lld_lcdSetCursor(x0, y0);
}

void lld_lcdClear(uint16_t color) {
    uint32_t index = 0;
    lld_lcdSetCursor(0, 0);
    lld_lcdWriteStreamStart();

    for(index = 0; index < SCREEN_WIDTH * SCREEN_HEIGHT; index++)
    	lld_lcdWriteData(color);

    lld_lcdWriteStreamStop();
}

// Do not use!
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

uint16_t lld_lcdGetOrientation(void) {
	return orientation;
}

uint16_t lld_lcdGetHeight(void) {
	return lcd_height;
}

uint16_t lld_lcdGetWidth(void) {
	return lcd_width;
}

/* a positive lines value shifts the screen up, negative down */
/* TODO: test this */
void lld_lcdVerticalScroll(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, int16_t lines) {
	uint16_t row0, row1;
	uint16_t i;

	for(i = 0; i < ((y1-y0) - abs(lines)); i++) {
		if(lines > 0) {
			row0 = y0 + i + lines;
			row1 = y0 + i;
		} else {
			row0 = (y1 - i - 1) + lines;
			row1 = (y1 - i - 1);
		}

		/* read row0 into the buffer and then write at row1*/
		lld_lcdSetWindow(x0, row0, x1, row0);
		lld_lcdReadStreamStart();
		lld_lcdReadStream(buf, x1-x0);
		lld_lcdReadStreamStop();

		lld_lcdSetWindow(x0, row1, x1, row1);
		lld_lcdWriteStreamStart();
		lld_lcdWriteStream(buf, x1-x0);
		lld_lcdWriteStreamStop();
	}

	lld_lcdResetWindow();
}


#endif
