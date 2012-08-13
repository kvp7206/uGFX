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

	gdispClear(Lime);	

	while (TRUE) {
		gdispDrawFillCircle(tpReadX(), tpReadY(), 3, Black);
	}
}

