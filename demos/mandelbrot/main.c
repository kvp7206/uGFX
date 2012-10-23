#include "ch.h"
#include "hal.h"
#include "gdisp.h"

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispSetOrientation(GDISP_ROTATE_270);

	mandelbrotInit(128, 128, 512);

	while(TRUE) {
	
	}
}

