/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    include/tdisp/tdisp.h
 * @brief   TDISP Graphic Driver subsystem header file.
 *
 * @addtogroup TDISP
 *
 * @brief		Module to drive character / dot matrix based displays
 * 
 * @details		The TDISP module provides high level abstraction to interface pixel oriented graphic displays.
 *				Due the TDISP module is completely encapsulated from the other modules, it's very fast and lightweight.
 *
 * @pre			GFX_USE_TDISP must be set to TRUE in gfxconf.h
 *
 * @{
 */

#ifndef _TDISP_H
#define _TDISP_H

#include "gfx.h"

#if GFX_USE_TDISP || defined(__DOXYGEN__)

/**
 * @brief	TDISP cursor shape definitions
 */
typedef enum cursorshape_e {
	cursorOff,
	cursorBlock,
	cursorBlinkingBlock,
	cursorUnderline,
	cursorBlinkingUnderline,
	cursorBar,
	cursorBlinkingBar
} cursorshape;

/**
 * @brief		TDISP display mode
 * 
 * @details		Display on of off definitions
 */
typedef enum displaymode_e {
	displayOff,
	displayOn
} displaymode;

/**
 * @brief		TDISP cursor movement
 * 
 * @details		Definitions for increasing or decreasing the cursor position after writing a character to
 * 			the display.
 */
typedef enum movemode_e {
	cursorIncrease,
	cursorDecrease
} movemode;

/**
 * @brief		TDISP cursor shift mode
 * 
 * @details		Definitions if the contents of the whole display shifts when writing a character to the display.
 * 			The cursor stays at the same position.
 */
typedef enum cursorshiftmode_e {
	shiftOn,
	shiftOff
} shiftmode;

/**
 * @brief		TDISP display scroll mode
 * 
 * @details		Definitions for scrolling the whole contents of the display to the right or left.
 * @note		Do not use this defintions with @p tdispControl, use the tdispScroll() function. That high-level
 * 			function is easier to use, and the scrollspeed can be defined.
 */
typedef enum displayscrollmode_e {
	displayScrollRight,
	displayScrollLeft
} displayscrollmode;

/**
 * @name		TDISP control values
 * @note		The low level driver may define extra control values
 * @{
 */
#define TDISP_CTRL_DISPLAY		0x0000
#define TDISP_CTRL_CURSOR		0x0001
#define TDISP_CTRL_MOVE			0x0002
#define TDISP_CTRL_SHIFT		0x0003
// #define TDISP_CTRL_SCROLL		0x0004

/** @} */

/**
 * @brief		The TDISP structure definition
 * 
 * @details		This structure contains the physical dimensions of the display, the
 * 			dimensions of a character in pixels and the maximum number of
 * 			custom characters that can be written to the display.
 */
typedef struct tdispStruct_t {
	coord_t		columns, rows;
	coord_t		charBitsX, charBitsY;
	uint16_t	maxCustomChars;
} tdispStruct;

/**
 * @brief	The TDISP structure
 */
extern tdispStruct	TDISP;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	TDISP driver initialisation
 * @note	This function is not implicitly invoked by @p halInit().
 *			It must be called manually.
 *
 * @return	TRUE if success, FALSE otherwise
 *
 * @init
 */
bool_t tdispInit(void);

/**
 * @brief		Clears the display
 * 
 * @details		The display gets cleared and the cursor is set in the home position ( 0, 0 )
 */
void tdispClear(void);

/**
 * @brief	Sets the cursor to it's home position ( 0, 0 )
 */
void tdispHome(void);

/**
 * @brief	Set cursor to a specified position
 *
 * @param[in] col	The column	(x)
 * @param[in] row	The row		(y)
 */
void tdispSetCursor(coord_t col, coord_t row);

/**
 * @brief	Store a custom character into the display
 *
 * @note	This usually must be done after each power-up since most
 *			LCDs lose their RAM content.
 *
 * @param[in] address		On which address to store the character from 0 up to (@p tdispGetNumCustomChars() - 1)
 * @param[in] charmap		The character to be stored.
 *
 * @note					The charmap is made up of @p tdispGetCharBitHieght() data values. Each data value is
 * 							made up of @p tdispGetCharBitWidth() bits of data. Note that bits in multiple rows are not
 * 							packed.
 */
void tdispCreateChar(uint8_t address, uint8_t *charmap);

/**
 * @brief	Draws a single character at the current cursor position and advances the cursor
 *
 * @param[in] c		The character to be drawn
 *
 * @note			Writing past the end of a row leaves the cursor in an undefined position.
 */
void tdispDrawChar(char c);

/**
 * @brief	Draws a string at the current cursor position and advances the cursor
 *
 * @param[in] s		The string to be drawn
 *
 * @note			Any characters written past the end of a row may or may not be displayed on
 * 					the next row. The cursor is also left in an undefined position.
 */
void tdispDrawString(char *s);

/**
 * @brief		Scrolls the display to the left or right by an amout of positions with a certain delay between each position
 * 
 * @param[in] direction	defines which direction the display has to scroll
 * 			displayScrollLeft: scrolls the display to the left
 * 			displayScrollRight: scrolls the display to the right
 * @param[in] amount	Defines the amout of characters the display must be scrolled
 * @param[in] delay	Defines the delay time in milliseconds between each position of the scroll. With this option the scroll can be made
 * 			visible to the user. If you define 0, the scroll will be to fast to observe.
 */
void tdispScroll(uint16_t direction, uint16_t amount, uint16_t delay);

/**
 * @brief		Set the backlight using PWM-driver
 * 
 * @param[in] percentage	Sets the brightness of the display in %.
 * 			0% = backlight is off or very low
 * 			100% = backlight is set to full brightness
 */
void tdispSetBacklight(uint16_t percentage);

/**
 * @brief		Control different display properties
 * @note		A wrapper macro exists for each option, please use them
 *			instead of this function manually unless calling a low
 *			level driver specific value.
 *
 * @param[in] what	What you want to control
 * @param[in] value	The value to be assigned
 */
void tdispControl(uint16_t what, uint16_t value);

/**
 * @brief		Set cursor move mode
 *
 * @param[in] mode	Set if the cursor position increases or decreases everytime a character is entered. 
 * 			mode = cursorIncrease	increases the cursor position
 * 			mode = cursorDecrease	decreases the cursor position
 * @note		cursorIncrease is default after initialisation of the display.
 *
 */
#define tdispSetMoveMode(mode)			tdispControl(TDISP_CTRL_MOVE, (movemode)(mode))

/**
 * @brief		Set display shift mode
 *
 * @param[in] mode	Shifts the display when entering characters. The cursor position stays the same.
 * 			mode = shiftOn	shifts the display when writing a character to the display
 * 			mode = shiftOff	shrift the cursor when writing a character to the display
 * @note		shiftOff is default after initialisation of the display.
 *
 */
#define tdispSetShiftMode(mode)			tdispControl(TDISP_CTRL_SHIFT, (shiftmode)(mode))

/**
 * @brief		Set display mode
 *
 * @param[in] mode	Sets the display on or off.
 * 			mode = displayOn puts the display on
 * 			mode = displayOff puts the display off
 * @note		displayOn is default after initialisation of the display
 *
 */
#define tdispDisplayMode(mode)			tdispControl(TDISP_CTRL_DISPLAY, (displaymode)(mode))

/**
 * @brief		Set the cursor shape.
 *
 * @param[in] shape	The shape to set the cursor.
 * 			Valid shapes are: 
 *			cursorOff = hide cursor
 *			cursorBlock = show cursor as a block
 *			cursorBlinkingBlock = show cursor as a blinking block
 *			cursorUnderline = show a cursor as a thin line under a character
 *			cursorBlinkingUnderline = show a cursor as a blinking thin line under a character
 *			cursorBar = show a cursor as a thin vertical bar near a character
 *			cursorBlinkingBar = show a cursor as a blinking thin vertical bar near a character
 * @note		Not all shapes are necessarily supported. The driver will make a similar
 * 			choice if the one specified is not available.
 */
#define tdispSetCursorShape(shape)		tdispControl(TDISP_CTRL_CURSOR, (cursorshape)(shape))

/**
 * @brief		Get the number of columns (width) in the display
 * 
 * @result		The number of columns in the display.
 */
#define tdispGetColumns()			(TDISP.columns)

/**
 * @brief		Get the number of rows (height) in the display
 * 
 * @result		The number of rows in the display
 */
#define tdispGetRows()				(TDISP.columns)

/**
 * @brief		Get the number of bits in width of a character
 * 
 * @result		The number of bits in width of a character
 */
#define tdispGetCharBitWidth()			(TDISP.charBitsX)

/**
 * @brief		Get the number of bits in height of a character
 * 
 * @result		The number of bits in height of a character
 */
#define tdispGetCharBitHeight()			(TDISP.charBitsY)

/**
 * @brief		Get the number of custom characters
 * 
 * @result		The maximum number of custom characters that can be programmed into the display
 */
#define tdispGetNumCustomChars()		(TDISP.maxCustomChars)

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_TDISP */

#endif /* _TDISP_H */
/** @} */

