#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "graph.h"
#include "math.h"

int data[5][2] = {
	{ 0, 0 },
	{ 10, 10 },
	{ 20, 20 },
	{ 30, 30 },
	{ 40, 40 }
};

int main(void) {
	halInit();
	chSysInit();

	gdispInit();
	gdispSetOrientation(GDISP_ROTATE_90);
	gdispClear(Black);
	
	Graph G1 = {
		gdispGetWidth()/2,
		gdispGetHeight()/2,
		-150,
		150,
		-110,
		110,
		21,
		5,
		TRUE,
		TRUE,
		White,
		Grey,
	};

	graphDrawSystem(&G1);
	
	uint16_t i;
	for(i = 0; i < 2500; i++)
		graphDrawDot(&G1, i-170, 80*sin(2*0.2*M_PI*i/180), 1, Blue);

	for(i = 0; i < 2500; i++)
		graphDrawDot(&G1, i/5-150, 95*sin(2*0.2*M_PI*i/180), 1, Green);

	while(TRUE) {
		chThdSleepMilliseconds(100);	
	}
}

