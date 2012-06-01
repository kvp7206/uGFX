#ifndef GRAPH_H
#define GRAPH_H

void graphDrawSystem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void graphDrawDots(int16_t coord[][2], uint16_t entries, uint16_t radius, uint16_t color);
void graphDrawNet(int16_t coord[][2], uint16_t entries, uint16_t radisu, uint16_t lineColor, uint16_t dotColor);

#endif
