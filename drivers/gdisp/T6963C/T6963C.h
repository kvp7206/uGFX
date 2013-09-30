/*
 * T6963C.h
 *
 *  Created on: 27.07.2013
 *      Author: A
 */

#ifndef T6963C_H_
#define T6963C_H_


const unsigned char GLCD_GRAPHIC_AREA   =               (GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH);
const unsigned char GLCD_TEXT_AREA      =               (GLCD_PIXELS_PER_LINE / GLCD_FONT_WIDTH);
const unsigned int GLCD_GRAPHIC_SIZE    =               (GLCD_GRAPHIC_AREA * GLCD_NUMBER_OF_LINES);
const unsigned int GLCD_TEXT_SIZE       =               (GLCD_TEXT_AREA * (GLCD_NUMBER_OF_LINES/8));

const unsigned int GLCD_TEXT_HOME           =           0;
const unsigned int GLCD_GRAPHIC_HOME        =           (GLCD_TEXT_HOME + GLCD_TEXT_SIZE);
const unsigned char GLCD_OFFSET_REGISTER    =           2;
const unsigned int  GLCD_EXTERNAL_CG_HOME   =           (GLCD_OFFSET_REGISTER << 11);


const unsigned char T6963_SET_CURSOR_POINTER    =       0x21;
const unsigned char T6963_SET_OFFSET_REGISTER   =       0x22;
const unsigned char T6963_SET_ADDRESS_POINTER   =       0x24;

const unsigned char T6963_SET_TEXT_HOME_ADDRESS     =   0x40;
const unsigned char T6963_SET_TEXT_AREA             =   0x41;
const unsigned char T6963_SET_GRAPHIC_HOME_ADDRESS  =   0x42;
const unsigned char T6963_SET_GRAPHIC_AREA          =   0x43;

const unsigned char T6963_MODE_SET                  =   0x80;

const unsigned char T6963_DISPLAY_MODE              =   0x90;
const unsigned char T6963_CURSOR_BLINK_ON       =   0x01;
const unsigned char T6963_CURSOR_DISPLAY_ON     =   0x02;
const unsigned char T6963_TEXT_DISPLAY_ON       =   0x04;
const unsigned char T6963_GRAPHIC_DISPLAY_ON    =   0x08;


const unsigned char T6963_CURSOR_PATTERN_SELECT     =   0xA0;
const unsigned char T6963_CURSOR_1_LINE         =   0x00;
const unsigned char T6963_CURSOR_2_LINE         =   0x01;
const unsigned char T6963_CURSOR_3_LINE         =   0x02;
const unsigned char T6963_CURSOR_4_LINE         =   0x03;
const unsigned char T6963_CURSOR_5_LINE         =   0x04;
const unsigned char T6963_CURSOR_6_LINE         =   0x05;
const unsigned char T6963_CURSOR_7_LINE         =   0x06;
const unsigned char T6963_CURSOR_8_LINE         =   0x07;

const unsigned char T6963_SET_DATA_AUTO_WRITE       =   0xB0;
const unsigned char T6963_SET_DATA_AUTO_READ        =   0xB1;
const unsigned char T6963_AUTO_RESET                =   0xB2;

const unsigned char T6963_DATA_WRITE_AND_INCREMENT  =   0xC0;
const unsigned char T6963_DATA_READ_AND_INCREMENT   =   0xC1;
const unsigned char T6963_DATA_WRITE_AND_DECREMENT  =   0xC2;
const unsigned char T6963_DATA_READ_AND_DECREMENT   =   0xC3;
const unsigned char T6963_DATA_WRITE_AND_NONVARIALBE=   0xC4;
const unsigned char T6963_DATA_READ_AND_NONVARIABLE =   0xC5;

const unsigned char T6963_SCREEN_PEEK               =   0xE0;
const unsigned char T6963_SCREEN_COPY               =   0xE8;

const unsigned char T6963_BIT_SET                   =   0xF8;
const unsigned char T6963_BIT_RESET                 =   0xF0;


#endif /* T6963C_H_ */
