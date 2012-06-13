#include "ch.h"
#include "hal.h"
#include "glcd.h"

static WORKING_AREA(waThread1, 128);
static void Thread1(void *arg) {
	(void)arg;

	while (TRUE) {
		palTogglePad(GPIOD, GPIOD_LED1);
		chThdSleepMilliseconds(500);
	}
}

int main(void) {
	halInit();
	chSysInit();
	
	lcdInit();
	lcdClear(Black);
	lcdDrawString(100, 100, "Hello World", White, Black);
	lcdDrawCircle(150, 150, 10, filled, Green);
	lcdDrawLine(0, 0, lcdGetWidth(), lcdGetHeight(), Yellow);

	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
	
	while (TRUE) {
		
		chThdSleepMilliseconds(200);	
	}

	return 0;
}
