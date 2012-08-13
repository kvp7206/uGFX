#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchpad.h"

TOUCHPADDriver TOUCHPADD1 = {
	&SPID1,
};

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispClear(Lime);

	tpInit(&TOUCHPADD1);
	tpCalibrate();

	while (TRUE) {
		gdispFillCircle(tpReadX(), tpReadY(), 3, Black); 
		
		chThdSleepMilliseconds(100);
	}
}

