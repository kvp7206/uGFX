#ifndef SSD1289_H
#define SSD1289_H

#include "glcd.h"

#ifndef LCD_USE_SSD1289

void lcdWriteIndex(uint16_t index);
void lcdWriteData(uint16_t data);
void lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue);
uint16_t lcdReadData(void);
uint16_t lcdReadReg(uint16_t lcdReg);
uint16_t lld_lcdInit(void);
uint16_t lld_lcdGetHeight(void);
uint16_t lld_lcdGetWidth(void);

#endif
#endif
