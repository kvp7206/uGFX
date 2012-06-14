#include "ch.h"
#include "hal.h"
#include "glcd.h"

int main(void) {
	halInit();
	chSysInit();
	
	lcdInit();
	lcdClear(Black);
	lcdDrawString(100, 100, "Hello World", White, Black);
	lcdDrawCircle(150, 150, 10, filled, Green);
	lcdDrawLine(0, 0, lcdGetWidth(), lcdGetHeight(), Yellow);

	while (TRUE) {
		
		chThdSleepMilliseconds(200);	
	}

	return 0;
}
