#include "ch.h"
#include "hal.h"
#include "gui.h"
#include "glcd.h"
#include "touchpad.h"

__inline void lld_lcdWriteGPIO(uint16_t data) {
	palWritePort(LCD_DATA_PORT, data);
}

__inline uint16_t lld_lcdReadGPIO(void) {
	uint16_t value;

    // change pin mode to digital input
    LCD_DATA_PORT->CRH = 0x44444444;
    LCD_DATA_PORT->CRL = 0x44444444;

    value = palReadPort(LCD_DATA_PORT); // dummy
    value = palReadPort(LCD_DATA_PORT);

    // change pin mode back to digital output
    LCD_DATA_PORT->CRH = 0x33333333;
    LCD_DATA_PORT->CRL = 0x33333333;

	return value;
}

// GLCD driver object
static GLCDDriver GLCDD1;

int main(void) {
	uint8_t setActive, setState, clearActive, clearState;

	halInit();
	chSysInit();

	// Initializes the LCD
    lcdInit(&GLCDD1);

	// Initializes the touchpad
	tpInit(&SPID1);

	// clear the entire LCD screen
    lcdClear(Black);

	// Initializes the GUI thread
	// 10ms interval
	// HIGHPRIO-2 thread priority level
	guiInit(10, HIGHPRIO-2);

	// set the following buttons to active
	// buttons wouldn't have any effect if you set these variables to 'inactive'
	setActive = active;
	clearActive = active;
	
	// draw a button to set, and one to clear the LED
	guiDrawButton(10, 10, 60, 60, "Set", Black, Yellow, &setActive, &setState);
	guiDrawButton(70, 10, 120, 60, "Clear", Black, Red, &clearActive, &clearState);
	
	while (TRUE) {

		// check if button 'set' is pressed
		if(setState)
			palSetPad(GPIOD, GPIOD_LED3);

		// check if button 'clear' is pressed
		if(clearState)
			palClearPad(GPIOD, GPIOD_LED3);
	
		chThdSleepMilliseconds(200);	
	}

	return 0;
}
