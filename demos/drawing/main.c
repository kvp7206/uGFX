#include "ch.h"
#include "hal.h"
#include "glcd.h"

static GLCDDriver GLCDD1;

int main(void) {
	halInit();
	chSysInit();
	
	lcdInit(&GLCDD1);
	lcdClear(Black);
	lcdDrawString(100, 100, "Hello World", White, Black);

	lcdMoveCursor(10,10,White, Black);
	chprintf((BaseSequentialStream *)&GLCDD1, "chTimeNow: %d", chTimeNow());

	lcdDrawCircle(150, 150, 10, filled, Green);
	lcdDrawLine(0, 0, lcdGetWidth(), lcdGetHeight(), Yellow);

	while (TRUE) {
		
		chThdSleepMilliseconds(200);	
	}

	return 0;
}
