#ifndef WORKER_H
#define WORKER_H

#define GLCD_WORKER_SIZE 2048

enum glcd_action {	GLCD_SET_CURSOR,
					GLCD_DRAW_PIXEL,
				};

enum glcd_result {	GLCD_DONE,
					GLCD_FAILED,
					GLCD_PROGRESS
				};

#define _glcd_msg_base			\
	enum glcd_action action;	\
	enum glcd_result result;

struct glcd_msg_base {
	_glcd_msg_base
};

struct glcd_msg_set_cursor {
	_glcd_msg_base

	uint16_t x;
	uint16_t y;
};

struct glcd_msg_draw_pixel {
	_glcd_msg_base

	uint16_t x;
	uint16_t y;
	uint16_t color;
};

#endif

