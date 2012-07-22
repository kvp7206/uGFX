#include "ch.h"
#include "hal.h"
#include "glcd.h"
#include "test.h"
#include "console.h"

static GLCDDriver GLCDD1;
static GLCDConsole CON1;

int main(void) {
	halInit();
	chSysInit();

	// init LCD and clear it
    lcdInit(&GLCDD1);
    lcdClear(Black);

	// init console
	lcdConsoleInit(&CON1, 0, 0, lcdGetWidth(), lcdGetHeight(), font_Small, Black, White);

	// use test thread for console output
	TestThread((BaseSequentialStream*)&CON1);

	while (TRUE) {

		chThdSleepMilliseconds(200);	
	}

	return 0;
}
