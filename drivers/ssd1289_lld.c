#include "ssd1289_lld.h"

#ifdef LCD_USE_SSD1289

extern uint16_t lcd_width, lcd_height;

void lld_lcdWriteIndex(uint16_t index) {
    Clr_RS;
    Set_RD;
  
    palWritePort(LCD_DATA_PORT, index);

    Clr_WR;
    Set_WR;
}

void lld_lcdWriteData(uint16_t data) {
	Set_RS;

	palWritePort(LCD_DATA_PORT, data);

	Clr_WR;
	Set_WR;
}

uint16_t lld_lcdReadData(void) {
	uint16_t value;

	Set_RS;
	Set_WR;
	Clr_RD;

	// change pin mode to digital input
	LCD_DATA_PORT->CRH = 0x44444444;
	LCD_DATA_PORT->CRL = 0x44444444;

    value = palReadPort(LCD_DATA_PORT); // dummy
    value = palReadPort(LCD_DATA_PORT); 

    // change pin mode back to digital output
    LCD_DATA_PORT->CRH = 0x33333333;
    LCD_DATA_PORT->CRL = 0x33333333;

    Set_RD;

	return value;
}

uint16_t lld_lcdGetHeight(void) {
	return lcd_height;
}

uint16_t lld_lcdGetWidth(void) {
	return lcd_width;
}

#endif

