#include "gfx.h"

int main(void) {
	font_t font1;

    // Initialize uGFX and the underlying system
    gfxInit();

    // Get the fonts we want to use
	font1 = gdispOpenFont("Archangelsk Regular 12");

	// Demonstrate our other fonts
	gdispDrawString(10, 10, "привет мир", font1, Yellow);

	// Wait forever
    while(TRUE) {
    	gfxSleepMilliseconds(500);
    }   
}

