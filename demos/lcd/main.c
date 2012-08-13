#include "ch.h"
#include "hal.h"
#include "gdisp.h"

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispClear(Black);

	// gdispDrawXXX(...);

	while (TRUE) {
		
		chThdSleepMilliseconds(100);
	}
}

