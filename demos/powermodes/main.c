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

	// wait two seconds to see current LCD content
	chThdSleepSeconds(2);

	// brings LCD to sleep mode
	lcdSetPowerMode(sleepOn);

	// wait two seconds to see current LCD content
	chThdSleepSeconds(2);

	// brings LCD back from sleep mode
	// content displayed before gets displayed again
	lcdSetPowerMode(sleepOff);

	while (TRUE) {
		
		chThdSleepMilliseconds(200);	
	}

	return 0;
}
