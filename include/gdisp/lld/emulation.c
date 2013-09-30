/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gdisp/lld/emulation.c
 * @brief	GDISP emulation routines for stuff the driver dosen't support
 *
 * @addtogroup GDISP
 *
 * @details	Even though this is a software emulation of a low level driver
 *			most validation doesn't need to happen here as eventually
 *			we call a real low level driver routine and if validation is
 *			required - it will do it.
 *
 * @{
 */
#ifndef GDISP_EMULATION_C
#define GDISP_EMULATION_C

#if GFX_USE_GDISP

/* Include the low level driver information */
#include "gdisp/lld/gdisp_lld.h"

/* Declare the GDISP structure */
GDISPDriver	GDISP;

#if !GDISP_HARDWARE_CLEARS 
	void gdisp_lld_clear(color_t color) {
		gdisp_lld_fill_area(0, 0, GDISP.Width, GDISP.Height, color);
	}
#endif

#if !GDISP_HARDWARE_LINES 
	void gdisp_lld_draw_line(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		int16_t dy, dx;
		int16_t addx, addy;
		int16_t P, diff, i;

		#if GDISP_HARDWARE_FILLS || GDISP_HARDWARE_SCROLL
		// speed improvement if vertical or horizontal
		if (x0 == x1) {
			if (y1 > y0)
				gdisp_lld_fill_area(x0, y0, 1, y1-y0+1, color);
			else
				gdisp_lld_fill_area(x0, y1, 1, y0-y1+1, color);
			return;
		}
		if (y0 == y1) {
			if (x1 > x0)
				gdisp_lld_fill_area(x0, y0, x1-x0+1, 1, color);
			else
				gdisp_lld_fill_area(x1, y0, x0-x1+1, 1, color);
			return;
		}
		#endif

		if (x1 >= x0) {
			dx = x1 - x0;
			addx = 1;
		} else {
			dx = x0 - x1;
			addx = -1;
		}
		if (y1 >= y0) {
			dy = y1 - y0;
			addy = 1;
		} else {
			dy = y0 - y1;
			addy = -1;
		}

		if (dx >= dy) {
			dy *= 2;
			P = dy - dx;
			diff = P - dx;

			for(i=0; i<=dx; ++i) {
				gdisp_lld_draw_pixel(x0, y0, color);
				if (P < 0) {
					P  += dy;
					x0 += addx;
				} else {
					P  += diff;
					x0 += addx;
					y0 += addy;
				}
			}
		} else {
			dx *= 2;
			P = dx - dy;
			diff = P - dy;

			for(i=0; i<=dy; ++i) {
				gdisp_lld_draw_pixel(x0, y0, color);
				if (P < 0) {
					P  += dx;
					y0 += addy;
				} else {
					P  += diff;
					x0 += addx;
					y0 += addy;
				}
			}
		}
	}
#endif

#if !GDISP_HARDWARE_FILLS
	void gdisp_lld_fill_area(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		#if GDISP_HARDWARE_SCROLL
			gdisp_lld_vertical_scroll(x, y, cx, cy, cy, color);
		#elif GDISP_HARDWARE_LINES
			coord_t x1, y1;
			
			x1 = x + cx - 1;
			y1 = y + cy;
			for(; y < y1; y++)
				gdisp_lld_draw_line(x, y, x1, y, color);
		#else
			coord_t x0, x1, y1;
			
			x0 = x;
			x1 = x + cx;
			y1 = y + cy;
			for(; y < y1; y++)
				for(x = x0; x < x1; x++)
					gdisp_lld_draw_pixel(x, y, color);
		#endif
	}
#endif

#if !GDISP_HARDWARE_BITFILLS
	void gdisp_lld_blit_area_ex(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
			coord_t x0, x1, y1;
			
			x0 = x;
			x1 = x + cx;
			y1 = y + cy;
			buffer += srcy*srccx+srcx;
			srccx -= cx;
			for(; y < y1; y++, buffer += srccx)
				for(x=x0; x < x1; x++)
					gdisp_lld_draw_pixel(x, y, *buffer++);
	}
#endif

#if GDISP_NEED_CLIP && !GDISP_HARDWARE_CLIP
	void gdisp_lld_set_clip(coord_t x, coord_t y, coord_t cx, coord_t cy) {
		#if GDISP_NEED_VALIDATION
			if (x >= GDISP.Width || y >= GDISP.Height || cx < 0 || cy < 0)
				return;
			if (x < 0) x = 0;
			if (y < 0) y = 0;
			if (x+cx > GDISP.Width) cx = GDISP.Width - x;
			if (y+cy > GDISP.Height) cy = GDISP.Height - y;
		#endif
		GDISP.clipx0 = x;
		GDISP.clipy0 = y;
		GDISP.clipx1 = x+cx;
		GDISP.clipy1 = y+cy;
	}
#endif

#if GDISP_NEED_CIRCLE && !GDISP_HARDWARE_CIRCLES
	void gdisp_lld_draw_circle(coord_t x, coord_t y, coord_t radius, color_t color) {
		coord_t a, b, P;

		a = 0;
		b = radius;
		P = 1 - radius;

		do {
			gdisp_lld_draw_pixel(x+a, y+b, color);
			gdisp_lld_draw_pixel(x+b, y+a, color);
			gdisp_lld_draw_pixel(x-a, y+b, color);
			gdisp_lld_draw_pixel(x-b, y+a, color);
			gdisp_lld_draw_pixel(x+b, y-a, color);
			gdisp_lld_draw_pixel(x+a, y-b, color);
			gdisp_lld_draw_pixel(x-a, y-b, color);
			gdisp_lld_draw_pixel(x-b, y-a, color);
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a <= b);
	}
#endif

#if GDISP_NEED_CIRCLE && !GDISP_HARDWARE_CIRCLEFILLS
	void gdisp_lld_fill_circle(coord_t x, coord_t y, coord_t radius, color_t color) {
		coord_t a, b, P;
		
		a = 0;
		b = radius;
		P = 1 - radius;

		do {
			gdisp_lld_draw_line(x-a, y+b, x+a, y+b, color);
			gdisp_lld_draw_line(x-a, y-b, x+a, y-b, color);
			gdisp_lld_draw_line(x-b, y+a, x+b, y+a, color);
			gdisp_lld_draw_line(x-b, y-a, x+b, y-a, color);
			if (P < 0)
				P += 3 + 2*a++;
			else
				P += 5 + 2*(a++ - b--);
		} while(a <= b);
	}
#endif

#if GDISP_NEED_ELLIPSE && !GDISP_HARDWARE_ELLIPSES
	void gdisp_lld_draw_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		int  dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
		long a2 = a*a, b2 = b*b;
		long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

		do {
			gdisp_lld_draw_pixel(x+dx, y+dy, color); /* I. Quadrant */
			gdisp_lld_draw_pixel(x-dx, y+dy, color); /* II. Quadrant */
			gdisp_lld_draw_pixel(x-dx, y-dy, color); /* III. Quadrant */
			gdisp_lld_draw_pixel(x+dx, y-dy, color); /* IV. Quadrant */

			e2 = 2*err;
			if(e2 <  (2*dx+1)*b2) {
				dx++;
				err += (2*dx+1)*b2;
			}
			if(e2 > -(2*dy-1)*a2) {
				dy--;
				err -= (2*dy-1)*a2;
			}
		} while(dy >= 0); 

		while(dx++ < a) { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
			gdisp_lld_draw_pixel(x+dx, y, color); /* -> Spitze der Ellipse vollenden */
			gdisp_lld_draw_pixel(x-dx, y, color);
	   }   
	}
#endif

#if GDISP_NEED_ELLIPSE && !GDISP_HARDWARE_ELLIPSEFILLS
	void gdisp_lld_fill_ellipse(coord_t x, coord_t y, coord_t a, coord_t b, color_t color) {
		int  dx = 0, dy = b; /* im I. Quadranten von links oben nach rechts unten */
		long a2 = a*a, b2 = b*b;
		long err = b2-(2*b-1)*a2, e2; /* Fehler im 1. Schritt */

		do {
			gdisp_lld_draw_line(x-dx,y+dy,x+dx,y+dy, color);
			gdisp_lld_draw_line(x-dx,y-dy,x+dx,y-dy, color);

			e2 = 2*err;
			if(e2 <  (2*dx+1)*b2) {
				dx++;
				err += (2*dx+1)*b2;
			}
			if(e2 > -(2*dy-1)*a2) {
				dy--;
				err -= (2*dy-1)*a2;
			}
		} while(dy >= 0); 

		while(dx++ < a) { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */
			gdisp_lld_draw_pixel(x+dx, y, color); /* -> Spitze der Ellipse vollenden */
			gdisp_lld_draw_pixel(x-dx, y, color);
	   }   
	}
#endif

#if GDISP_NEED_ARC && !GDISP_HARDWARE_ARCS

	#include <math.h>

	/*
	 * @brief				Internal helper function for gdispDrawArc()
	 *
	 * @note				DO NOT USE DIRECTLY!
	 *
	 * @param[in] x, y		The middle point of the arc
	 * @param[in] start		The start angle of the arc
	 * @param[in] end		The end angle of the arc
	 * @param[in] radius	The radius of the arc
	 * @param[in] color		The color in which the arc will be drawn
	 *
	 * @notapi
	 */
	static void _draw_arc(coord_t x, coord_t y, uint16_t start, uint16_t end, uint16_t radius, color_t color) {
	    if (/*start >= 0 && */start <= 180) {
	        float x_maxI = x + radius*cos(start*M_PI/180);
	        float x_minI;

	        if (end > 180)
	            x_minI = x - radius;
	        else
	            x_minI = x + radius*cos(end*M_PI/180);

	        int a = 0;
	        int b = radius;
	        int P = 1 - radius;

	        do {
	            if(x-a <= x_maxI && x-a >= x_minI)
	            	gdisp_lld_draw_pixel(x-a, y-b, color);
	            if(x+a <= x_maxI && x+a >= x_minI)
	            	gdisp_lld_draw_pixel(x+a, y-b, color);
	            if(x-b <= x_maxI && x-b >= x_minI)
	            	gdisp_lld_draw_pixel(x-b, y-a, color);
	            if(x+b <= x_maxI && x+b >= x_minI)
	            	gdisp_lld_draw_pixel(x+b, y-a, color);

	            if (P < 0) {
	                P = P + 3 + 2*a;
	                a = a + 1;
	            } else {
	                P = P + 5 + 2*(a - b);
	                a = a + 1;
	                b = b - 1;
	            }
	        } while(a <= b);
	    }

	    if (end > 180 && end <= 360) {
	        float x_maxII = x+radius*cos(end*M_PI/180);
	        float x_minII;

	        if(start <= 180)
	            x_minII = x - radius;
	        else
	            x_minII = x+radius*cos(start*M_PI/180);

	        int a = 0;
	        int b = radius;
	        int P = 1 - radius;

	        do {
	            if(x-a <= x_maxII && x-a >= x_minII)
	            	gdisp_lld_draw_pixel(x-a, y+b, color);
	            if(x+a <= x_maxII && x+a >= x_minII)
	            	gdisp_lld_draw_pixel(x+a, y+b, color);
	            if(x-b <= x_maxII && x-b >= x_minII)
	            	gdisp_lld_draw_pixel(x-b, y+a, color);
	            if(x+b <= x_maxII && x+b >= x_minII)
	            	gdisp_lld_draw_pixel(x+b, y+a, color);

	            if (P < 0) {
	                P = P + 3 + 2*a;
	                a = a + 1;
	            } else {
	                P = P + 5 + 2*(a - b);
	                a = a + 1;
	                b = b - 1;
	            }
	        } while (a <= b);
	    }
	}

	void gdisp_lld_draw_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color) {
		if(endangle < startangle) {
	        _draw_arc(x, y, startangle, 360, radius, color);
	        _draw_arc(x, y, 0, endangle, radius, color);
	    } else {
	        _draw_arc(x, y, startangle, endangle, radius, color);
		}
	}
#endif

#if GDISP_NEED_ARC && !GDISP_HARDWARE_ARCFILLS
	/*
	 * @brief				Internal helper function for gdispDrawArc()
	 *
	 * @note				DO NOT USE DIRECTLY!
	 *
	 * @param[in] x, y		The middle point of the arc
	 * @param[in] start		The start angle of the arc
	 * @param[in] end		The end angle of the arc
	 * @param[in] radius	The radius of the arc
	 * @param[in] color		The color in which the arc will be drawn
	 *
	 * @notapi
	 */
	static void _fill_arc(coord_t x, coord_t y, uint16_t start, uint16_t end, uint16_t radius, color_t color) {
	    if (/*start >= 0 && */start <= 180) {
	        float x_maxI = x + radius*cos(start*M_PI/180);
	        float x_minI;

	        if (end > 180)
	            x_minI = x - radius;
	        else
	            x_minI = x + radius*cos(end*M_PI/180);

	        int a = 0;
	        int b = radius;
	        int P = 1 - radius;

	        do {
	            if(x-a <= x_maxI && x-a >= x_minI)
	            	gdisp_lld_draw_line(x, y, x-a, y-b, color);
	            if(x+a <= x_maxI && x+a >= x_minI)
	            	gdisp_lld_draw_line(x, y, x+a, y-b, color);
	            if(x-b <= x_maxI && x-b >= x_minI)
	            	gdisp_lld_draw_line(x, y, x-b, y-a, color);
	            if(x+b <= x_maxI && x+b >= x_minI)
	            	gdisp_lld_draw_line(x, y, x+b, y-a, color);

	            if (P < 0) {
	                P = P + 3 + 2*a;
	                a = a + 1;
	            } else {
	                P = P + 5 + 2*(a - b);
	                a = a + 1;
	                b = b - 1;
	            }
	        } while(a <= b);
	    }

	    if (end > 180 && end <= 360) {
	        float x_maxII = x+radius*cos(end*M_PI/180);
	        float x_minII;

	        if(start <= 180)
	            x_minII = x - radius;
	        else
	            x_minII = x+radius*cos(start*M_PI/180);

	        int a = 0;
	        int b = radius;
	        int P = 1 - radius;

	        do {
	            if(x-a <= x_maxII && x-a >= x_minII)
	            	gdisp_lld_draw_line(x, y, x-a, y+b, color);
	            if(x+a <= x_maxII && x+a >= x_minII)
	            	gdisp_lld_draw_line(x, y, x+a, y+b, color);
	            if(x-b <= x_maxII && x-b >= x_minII)
	            	gdisp_lld_draw_line(x, y, x-b, y+a, color);
	            if(x+b <= x_maxII && x+b >= x_minII)
	            	gdisp_lld_draw_line(x, y, x+b, y+a, color);

	            if (P < 0) {
	                P = P + 3 + 2*a;
	                a = a + 1;
	            } else {
	                P = P + 5 + 2*(a - b);
	                a = a + 1;
	                b = b - 1;
	            }
	        } while (a <= b);
	    }
	}

	void gdisp_lld_fill_arc(coord_t x, coord_t y, coord_t radius, coord_t startangle, coord_t endangle, color_t color) {
		if(endangle < startangle) {
	        _fill_arc(x, y, startangle, 360, radius, color);
	        _fill_arc(x, y, 0, endangle, radius, color);
	    } else {
	        _fill_arc(x, y, startangle, endangle, radius, color);
		}
	}
#endif

#if GDISP_NEED_CONTROL && !GDISP_HARDWARE_CONTROL
	void gdisp_lld_control(unsigned what, void *value) {
		(void)what;
		(void)value;
		/* Ignore everything */
	}
#endif

#if GDISP_NEED_QUERY && !GDISP_HARDWARE_QUERY
void *gdisp_lld_query(unsigned what) {
	(void) what;
	return (void *)-1;
}
#endif

#if GDISP_NEED_MSGAPI
	void gdisp_lld_msg_dispatch(gdisp_lld_msg_t *msg) {
		switch(msg->action) {
		case GDISP_LLD_MSG_NOP:
			break;
		case GDISP_LLD_MSG_INIT:
			gdisp_lld_init();
			break;
		case GDISP_LLD_MSG_CLEAR:
			gdisp_lld_clear(msg->clear.color);
			break;
		case GDISP_LLD_MSG_DRAWPIXEL:
			gdisp_lld_draw_pixel(msg->drawpixel.x, msg->drawpixel.y, msg->drawpixel.color);
			break;
		case GDISP_LLD_MSG_FILLAREA:
			gdisp_lld_fill_area(msg->fillarea.x, msg->fillarea.y, msg->fillarea.cx, msg->fillarea.cy, msg->fillarea.color);
			break;
		case GDISP_LLD_MSG_BLITAREA:
			gdisp_lld_blit_area_ex(msg->blitarea.x, msg->blitarea.y, msg->blitarea.cx, msg->blitarea.cy, msg->blitarea.srcx, msg->blitarea.srcy, msg->blitarea.srccx, msg->blitarea.buffer);
			break;
		case GDISP_LLD_MSG_DRAWLINE:
			gdisp_lld_draw_line(msg->drawline.x0, msg->drawline.y0, msg->drawline.x1, msg->drawline.y1, msg->drawline.color);
			break;
		#if GDISP_NEED_CLIP
			case GDISP_LLD_MSG_SETCLIP:
				gdisp_lld_set_clip(msg->setclip.x, msg->setclip.y, msg->setclip.cx, msg->setclip.cy);
				break;
		#endif
		#if GDISP_NEED_CIRCLE
			case GDISP_LLD_MSG_DRAWCIRCLE:
				gdisp_lld_draw_circle(msg->drawcircle.x, msg->drawcircle.y, msg->drawcircle.radius, msg->drawcircle.color);
				break;
			case GDISP_LLD_MSG_FILLCIRCLE:
				gdisp_lld_fill_circle(msg->fillcircle.x, msg->fillcircle.y, msg->fillcircle.radius, msg->fillcircle.color);
				break;
		#endif
		#if GDISP_NEED_ELLIPSE
			case GDISP_LLD_MSG_DRAWELLIPSE:
				gdisp_lld_draw_ellipse(msg->drawellipse.x, msg->drawellipse.y, msg->drawellipse.a, msg->drawellipse.b, msg->drawellipse.color);
				break;
			case GDISP_LLD_MSG_FILLELLIPSE:
				gdisp_lld_fill_ellipse(msg->fillellipse.x, msg->fillellipse.y, msg->fillellipse.a, msg->fillellipse.b, msg->fillellipse.color);
				break;
		#endif
		#if GDISP_NEED_ARC
			case GDISP_LLD_MSG_DRAWARC:
				gdisp_lld_draw_circle(msg->drawarc.x, msg->drawarc.y, msg->drawarc.radius, msg->drawarc.startangle, msg->drawarc.endangle, msg->drawarc.color);
				break;
			case GDISP_LLD_MSG_FILLARC:
				gdisp_lld_fill_circle(msg->fillarc.x, msg->fillarc.y, msg->fillarc.radius, msg->fillarc.startangle, msg->fillarc.endangle, msg->fillarc.color);
				break;
		#endif
		#if GDISP_NEED_PIXELREAD
			case GDISP_LLD_MSG_GETPIXELCOLOR:
				msg->getpixelcolor.result = gdisp_lld_get_pixel_color(msg->getpixelcolor.x, msg->getpixelcolor.y);
				break;
		#endif
		#if GDISP_NEED_SCROLL
			case GDISP_LLD_MSG_VERTICALSCROLL:
				gdisp_lld_vertical_scroll(msg->verticalscroll.x, msg->verticalscroll.y, msg->verticalscroll.cx, msg->verticalscroll.cy, msg->verticalscroll.lines, msg->verticalscroll.bgcolor);
				break;
		#endif
		#if GDISP_NEED_CONTROL
			case GDISP_LLD_MSG_CONTROL:
				gdisp_lld_control(msg->control.what, msg->control.value);
				break;
		#endif
		#if GDISP_NEED_QUERY
			case GDISP_LLD_MSG_QUERY:
				msg->query.result = gdisp_lld_query(msg->query.what);
				break;
		#endif
		}
	}
#endif

#endif  /* GFX_USE_GDISP */
#endif	/* GDISP_EMULATION_C */
/** @} */

