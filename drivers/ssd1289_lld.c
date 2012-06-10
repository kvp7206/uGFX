#include "ssd1289_lld.h"

#ifdef LCD_USE_SSD1289

extern uint16_t DeviceCode;
extern uint16_t lcd_width, lcd_height;

__inline void lcdWriteIndex(uint16_t index) {
    Clr_RS;
    Set_RD;
  
    palWritePort(LCD_DATA_PORT, index);

    Clr_WR;
    Set_WR;
}

__inline void lcdWriteData(uint16_t data) {
	Set_RS;

	palWritePort(LCD_DATA_PORT, data);

	Clr_WR;
	Set_WR;
}

__inline void lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) { 
    Clr_CS;
    lcdWriteIndex(lcdReg);    
    lcdWriteData(lcdRegValue);  
    Set_CS; 
}

__inline uint16_t lcdReadData(void) {
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

__inline uint16_t lcdReadReg(uint16_t lcdReg) {
    uint16_t lcdRAM;

    Clr_CS;
    lcdWriteIndex(lcdReg);
    lcdRAM = lcdReadData();
                 
    Set_CS;

    return lcdRAM;
}

void lld_lcdInit(void) {
	DeviceCode = lcdReadReg(0x0000);

	lcdWriteReg(0x0000,0x0001);		lcdDelay(5);
    lcdWriteReg(0x0003,0xA8A4);    	lcdDelay(5);   
    lcdWriteReg(0x000C,0x0000);    	lcdDelay(5);   
    lcdWriteReg(0x000D,0x080C);    	lcdDelay(5);   
    lcdWriteReg(0x000E,0x2B00);    	lcdDelay(5);   
    lcdWriteReg(0x001E,0x00B0);    	lcdDelay(5);  
	lcdWriteReg(0x0001,0x2B3F);		lcdDelay(5);
    lcdWriteReg(0x0002,0x0600);    	lcdDelay(5);
    lcdWriteReg(0x0010,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0011,0x6070);    	lcdDelay(5);
    lcdWriteReg(0x0005,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0006,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0016,0xEF1C);    	lcdDelay(5);
    lcdWriteReg(0x0017,0x0003);    	lcdDelay(5);
    lcdWriteReg(0x0007,0x0133);    	lcdDelay(5);         
    lcdWriteReg(0x000B,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x000F,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0041,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0042,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0048,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0049,0x013F);    	lcdDelay(5);
    lcdWriteReg(0x004A,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x004B,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0044,0xEF00);    	lcdDelay(5);
    lcdWriteReg(0x0045,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0046,0x013F);    	lcdDelay(5);
    lcdWriteReg(0x0030,0x0707);    	lcdDelay(5);
    lcdWriteReg(0x0031,0x0204);    	lcdDelay(5);
    lcdWriteReg(0x0032,0x0204);    	lcdDelay(5);
    lcdWriteReg(0x0033,0x0502);    	lcdDelay(5);
    lcdWriteReg(0x0034,0x0507);    	lcdDelay(5);
    lcdWriteReg(0x0035,0x0204);    	lcdDelay(5);
    lcdWriteReg(0x0036,0x0204);    	lcdDelay(5);
    lcdWriteReg(0x0037,0x0502);    	lcdDelay(5);
    lcdWriteReg(0x003A,0x0302);    	lcdDelay(5);
    lcdWriteReg(0x003B,0x0302);    	lcdDelay(5);
    lcdWriteReg(0x0023,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0024,0x0000);    	lcdDelay(5);
    lcdWriteReg(0x0025,0x8000);    	lcdDelay(5);
    lcdWriteReg(0x004f,0);			lcdDelay(5);      
    lcdWriteReg(0x004e,0);			lcdDelay(5);
}

uint16_t lld_lcdGetHeight(void) {
	return lcd_height;
}

uint16_t lld_lcdGetWidth(void) {
	return lcd_width;
}

#endif

