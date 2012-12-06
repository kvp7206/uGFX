#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "math.h"

int data[5][2] = {
	{ 0, 0 },
	{ 10, 10 },
	{ 20, 20 },
	{ 30, 30 },
	{ 40, 40 }
};

GGraphObject	g;

GGraphStyle GraphStyle2 = {
	{ GGRAPH_POINT_DOT, 0, Green },			// point
	{ GGRAPH_LINE_DOT, 2, Gray },			// line
	{ GGRAPH_LINE_SOLID, 0, White },		// x axis
	{ GGRAPH_LINE_SOLID, 0, White },		// y axis
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// x grid
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// y grid
	GWIN_GRAPH_STYLE_XAXIS_ARROWS|GWIN_GRAPH_STYLE_YAXIS_ARROWS		// flags
};

int main(void) {
	GHandle		gh;
	uint16_t 	i;
	
	halInit();
	chSysInit();

	gdispInit();
	gdispClear(Black);
	
	gh = gwinCreateGraph(0, 0, gdispGetWidth(), gdispGetHeight());

	gwinGraphSetOrigin(gh, 150, 150);
	gwinGraphDrawAxis(gh);
	
	for(i = 0; i < 2500; i++)
		gwinGraphDrawPoint(gh, i-170, 80*sin(2*0.2*M_PI*i/180));

	gwinGraphStartSet(gh);
	gwinGraphSetStyle(gh, &GraphStyle2);
	
	for(i = 0; i < 2500; i++)
		gwinGraphDrawPoint(gh, i/5-150, 95*sin(2*0.2*M_PI*i/180));

	while(TRUE) {
		chThdSleepMilliseconds(100);	
	}
}

