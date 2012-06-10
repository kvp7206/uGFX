#ifndef SSD1289_H
#define SSD1289_H

#include "glcd.h"

#ifndef LCD_USE_SSD1289

void lld_lcdWriteIndex(uint16_t index);
void lld_lcdWriteData(uint16_t data);
void lld_lcdWriteData(uint16_t data);
uint16_t lld_lcdGetHeight(void);
uint16_t lld_lcdGetWidth(void);

#endif
#endif
