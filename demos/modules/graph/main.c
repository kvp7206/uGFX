#include "ch.h"
#include "hal.h"
#include "gfx.h"
#include "math.h"

const GGraphPoint data[5] = {
	{ -40, -40 },
	{ 70, 40 },
	{ 140, 60 },
	{ 210, 60 },
	{ 280, 200 }
};

GGraphObject	g;

GGraphStyle GraphStyle1 = {
	{ GGRAPH_POINT_DOT, 0, Blue },			// point
	{ GGRAPH_LINE_NONE, 2, Gray },			// line
	{ GGRAPH_LINE_SOLID, 0, White },		// x axis
	{ GGRAPH_LINE_SOLID, 0, White },		// y axis
	{ GGRAPH_LINE_DASH, 5, Gray, 50 },		// x grid
	{ GGRAPH_LINE_DOT, 7, Yellow, 50 },		// y grid
	GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS	// flags
};

GGraphStyle GraphStyle2 = {
	{ GGRAPH_POINT_SQUARE, 5, Red },		// point
	{ GGRAPH_LINE_DOT, 2, Pink },			// line
	{ GGRAPH_LINE_SOLID, 0, White },		// x axis
	{ GGRAPH_LINE_SOLID, 0, White },		// y axis
	{ GGRAPH_LINE_DASH, 5, Gray, 50 },		// x grid
	{ GGRAPH_LINE_DOT, 7, Yellow, 50 },		// y grid
	GWIN_GRAPH_STYLE_POSITIVE_AXIS_ARROWS	// flags
};

int main(void) {
	GHandle		gh;
	uint16_t 	i;
	
	halInit();
	chSysInit();

	gdispInit();
	gdispClear(Black);
	
	gh = gwinCreateGraph(&g, 0, 0, gdispGetWidth(), gdispGetHeight());

	gwinGraphSetOrigin(gh, gwinGetWidth(gh)/2, gwinGetHeight(gh)/2);
	gwinGraphSetStyle(gh, &GraphStyle1);
	gwinGraphDrawAxis(gh);
	
	for(i = 0; i < gwinGetWidth(gh); i++)
		gwinGraphDrawPoint(gh, i-gwinGetWidth(gh)/2, 80*sin(2*0.2*M_PI*i/180));

	gwinGraphStartSet(gh);
	GraphStyle1.point.color = Green;
	gwinGraphSetStyle(gh, &GraphStyle1);
	
	for(i = 0; i < gwinGetWidth(gh)*5; i++)
		gwinGraphDrawPoint(gh, i/5-gwinGetWidth(gh)/2, 95*sin(2*0.2*M_PI*i/180));

	gwinGraphStartSet(gh);
	gwinGraphSetStyle(gh, &GraphStyle2);

	gwinGraphDrawPoints(gh, data, sizeof(data)/sizeof(data[0]));

	while(TRUE) {
		chThdSleepMilliseconds(100);	
	}
}

