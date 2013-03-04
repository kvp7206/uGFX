#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "matterhorn1.h"
#include "matterhorn2.h"
#include "fruits1.h"

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispSetOrientation(GDISP_ROTATE_90);

	while(1) {
		gdispBlitArea(0, 0, fruits1.width, fruits1.height, (const pixel_t*)fruits1.pixel_data);
		chThdSleepMilliseconds(3000);

		gdispBlitArea(0, 0, matterhorn1.width, matterhorn1.height, (const pixel_t*)matterhorn1.pixel_data);
		chThdSleepMilliseconds(3000);

		gdispBlitArea(0, 0, matterhorn2.width, matterhorn2.height, (const pixel_t*)matterhorn2.pixel_data);
		chThdSleepMilliseconds(3000);
	}
}

