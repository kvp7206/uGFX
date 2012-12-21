/*
    ChibiOS/GFX - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @file    src/gwin/graph.c
 * @brief   GWIN sub-system button code.
 *
 * @defgroup Graph Graph
 * @ingroup GWIN
 *
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gfx.h"

#if (GFX_USE_GWIN && GWIN_NEED_GRAPH) || defined(__DOXYGEN__)

#include "gwin/internal.h"

#define GGRAPH_FLG_CONNECTPOINTS			(GWIN_FIRST_CONTROL_FLAG<<0)
#define GGRAPH_ARROW_SIZE					5

static const GGraphStyle GGraphDefaultStyle = {
	{ GGRAPH_POINT_DOT, 0, White },			// point
	{ GGRAPH_LINE_DOT, 2, Gray },			// line
	{ GGRAPH_LINE_SOLID, 0, White },		// x axis
	{ GGRAPH_LINE_SOLID, 0, White },		// y axis
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// x grid
	{ GGRAPH_LINE_NONE, 0, White, 0 },		// y grid
	GWIN_GRAPH_STYLE_XAXIS_ARROWS|GWIN_GRAPH_STYLE_YAXIS_ARROWS		// flags
};

static void pointto(GGraphObject *gg, coord_t x, coord_t y, const GGraphPointStyle *style) {
	if (style->type == GGRAPH_POINT_NONE)
		return;

	// Convert to device space. Note the y-axis is inverted.
	x += gg->gwin.x + gg->xorigin;
	y = gg->gwin.y + gg->gwin.height - 1 - gg->yorigin - y;

	if (style->size <= 1) {
		gdispDrawPixel(x, y, style->color);
		return;
	}

	switch(style->type) {
	case GGRAPH_POINT_SQUARE:
		gdispDrawBox(x-style->size, y-style->size, 2*style->size, 2*style->size, style->color);
		break;
#if GDISP_NEED_CIRCLE
	case GGRAPH_POINT_CIRCLE:
		gdispDrawCircle(x, y, style->size, style->color);
		break;
#endif
	case GGRAPH_POINT_DOT:
	default:
		gdispDrawPixel(x, y, style->color);
		break;
	}
}

static void lineto(GGraphObject *gg, coord_t x0, coord_t y0, coord_t x1, coord_t y1, const GGraphLineStyle *style) {
	coord_t	dy, dx;
	coord_t addx, addy;
	coord_t P, diff, i;
	coord_t	run_on, run_off, run;

	if (style->type == GGRAPH_LINE_NONE)
		return;

	// Convert to device space. Note the y-axis is inverted.
	x0 += gg->gwin.x + gg->xorigin;
	y0 = gg->gwin.y + gg->gwin.height - 1 - gg->yorigin - y0;
	x1 += gg->gwin.x + gg->xorigin;
	y1 = gg->gwin.y + gg->gwin.height - 1 - gg->yorigin - y1;

	if (style->size <= 0) {
		// Use the driver to draw a solid line
		gdispDrawLine(x0, y0, x1, y1, style->color);
		return;
	}

	switch (style->type) {
	case GGRAPH_LINE_DOT:
		run_on = 1;
		run_off = -style->size;
		break;

	case GGRAPH_LINE_DASH:
		run_on = style->size;
		run_off = -style->size;
		break;

	case GGRAPH_LINE_SOLID:
	default:
		// Use the driver to draw a solid line
		gdispDrawLine(x0, y0, x1, y1, style->color);
		return;
	}

	// Use Bresenham's algorithm modified to draw a stylized line
	run = 0;
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
			if (run++ >= 0) {
				if (run >= run_on)
					run = run_off;
				gdispDrawPixel(x0, y0, style->color);
			}
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
			if (run++ >= 0) {
				if (run >= run_on)
					run = run_off;
				gdispDrawPixel(x0, y0, style->color);
			}
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

GHandle gwinCreateGraph(GGraphObject *gg, coord_t x, coord_t y, coord_t width, coord_t height) {
	if (!(gg = (GGraphObject *)_gwinInit((GWindowObject *)gg, x, y, width, height, sizeof(GGraphObject))))
		return 0;
	gg->gwin.type = GW_GRAPH;
	gg->xorigin = gg->yorigin = 0;
	gg->lastx = gg->lasty = 0;
	gwinGraphSetStyle(&gg->gwin, &GGraphDefaultStyle);
	return (GHandle)gg;
}

void gwinGraphSetStyle(GHandle gh, const GGraphStyle *pstyle) {
	#define gg	((GGraphObject *)gh)

	if (gh->type != GW_GRAPH)
		return;

	gg->style.point.type = pstyle->point.type;
	gg->style.point.size = pstyle->point.size;
	gg->style.point.color = pstyle->point.color;
	gg->style.line.type = pstyle->line.type;
	gg->style.line.size = pstyle->line.size;
	gg->style.line.color = pstyle->line.color;
	gg->style.xaxis.type = pstyle->xaxis.type;
	gg->style.xaxis.size = pstyle->xaxis.size;
	gg->style.xaxis.color = pstyle->xaxis.color;
	gg->style.yaxis.type = pstyle->yaxis.type;
	gg->style.yaxis.size = pstyle->yaxis.size;
	gg->style.yaxis.color = pstyle->yaxis.color;
	gg->style.xgrid.type = pstyle->xgrid.type;
	gg->style.xgrid.size = pstyle->xgrid.size;
	gg->style.xgrid.color = pstyle->xgrid.color;
	gg->style.xgrid.spacing = pstyle->xgrid.spacing;
	gg->style.ygrid.type = pstyle->ygrid.type;
	gg->style.ygrid.size = pstyle->ygrid.size;
	gg->style.ygrid.color = pstyle->ygrid.color;
	gg->style.ygrid.spacing = pstyle->ygrid.spacing;
	gg->style.flags = pstyle->flags;

	#undef gg
}

void gwinGraphSetOrigin(GHandle gh, coord_t x, coord_t y) {
	#define gg	((GGraphObject *)gh)

	if (gh->type != GW_GRAPH)
		return;

	gg->xorigin = x;
	gg->yorigin = y;

	#undef gg
}

void gwinGraphDrawAxis(GHandle gh) {
	#define gg	((GGraphObject *)gh)
	coord_t		i, xmin, ymin, xmax, ymax;

	if (gh->type != GW_GRAPH)
		return;

	xmin = -gg->xorigin;
	xmax = gh->width-gg->xorigin-1;
	ymin = -gg->yorigin;
	ymax = gh->height-gg->yorigin-1;

	// x grid - this code assumes that the GGraphGridStyle is a superset of GGraphListStyle
	if (gg->style.xgrid.type != GGRAPH_LINE_NONE && gg->style.xgrid.spacing >= 2) {
		for(i = gg->style.xgrid.spacing; i <= xmax; i += gg->style.xgrid.spacing)
			lineto(gg, i, ymin, i, ymax, (GGraphLineStyle *)&gg->style.xgrid);
		for(i = -gg->style.xgrid.spacing; i >= xmin; i -= gg->style.xgrid.spacing)
			lineto(gg, i, ymin, i, ymax, (GGraphLineStyle *)&gg->style.xgrid);
	}

	// y grid - this code assumes that the GGraphGridStyle is a superset of GGraphListStyle
	if (gg->style.ygrid.type != GGRAPH_LINE_NONE && gg->style.ygrid.spacing >= 2) {
		for(i = gg->style.ygrid.spacing; i <= ymax; i += gg->style.ygrid.spacing)
			lineto(gg, xmin, i, xmax, i, (GGraphLineStyle *)&gg->style.ygrid);
		for(i = -gg->style.ygrid.spacing; i >= ymin; i -= gg->style.ygrid.spacing)
			lineto(gg, xmin, i, xmax, i, (GGraphLineStyle *)&gg->style.ygrid);
	}

	// x axis
	lineto(gg, xmin, 0, xmax, 0, &gg->style.xaxis);
	if ((gg->style.flags & GWIN_GRAPH_STYLE_XAXIS_NEGATIVE_ARROWS)) {
		if (xmin > 0 || xmin < -(GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, xmin, 0, xmin+GGRAPH_ARROW_SIZE, GGRAPH_ARROW_SIZE, &gg->style.xaxis);
			lineto(gg, xmin, 0, xmin+GGRAPH_ARROW_SIZE, -GGRAPH_ARROW_SIZE, &gg->style.xaxis);
		}
	}
	if ((gg->style.flags & GWIN_GRAPH_STYLE_XAXIS_POSITIVE_ARROWS)) {
		if (xmax < 0 || xmax > (GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, xmax, 0, xmax-GGRAPH_ARROW_SIZE, GGRAPH_ARROW_SIZE, &gg->style.xaxis);
			lineto(gg, xmax, 0, xmax-GGRAPH_ARROW_SIZE, -GGRAPH_ARROW_SIZE, &gg->style.xaxis);
		}
	}

	// y axis
	lineto(gg, 0, ymin, 0, ymax, &gg->style.yaxis);
	if ((gg->style.flags & GWIN_GRAPH_STYLE_YAXIS_NEGATIVE_ARROWS)) {
		if (ymin > 0 || ymin < -(GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, 0, ymin, GGRAPH_ARROW_SIZE, ymin+GGRAPH_ARROW_SIZE, &gg->style.yaxis);
			lineto(gg, 0, ymin, -GGRAPH_ARROW_SIZE, ymin+GGRAPH_ARROW_SIZE, &gg->style.yaxis);
		}
	}
	if ((gg->style.flags & GWIN_GRAPH_STYLE_YAXIS_POSITIVE_ARROWS)) {
		if (ymax < 0 || ymax > (GGRAPH_ARROW_SIZE+1)) {
			lineto(gg, 0, ymax, GGRAPH_ARROW_SIZE, ymax-GGRAPH_ARROW_SIZE, &gg->style.yaxis);
			lineto(gg, 0, ymax, -GGRAPH_ARROW_SIZE, ymax-GGRAPH_ARROW_SIZE, &gg->style.yaxis);
		}
	}

	#undef gg
}

void gwinGraphStartSet(GHandle gh) {
	if (gh->type != GW_GRAPH)
		return;

	gh->flags &= ~GGRAPH_FLG_CONNECTPOINTS;
}

void gwinGraphDrawPoint(GHandle gh, coord_t x, coord_t y) {
	#define gg	((GGraphObject *)gh)

	if (gh->type != GW_GRAPH)
		return;

	if ((gh->flags & GGRAPH_FLG_CONNECTPOINTS)) {
		// Draw the line
		lineto(gg, gg->lastx, gg->lasty, x, y, &gg->style.line);

		// Redraw the previous point because the line may have overwritten it
		pointto(gg, gg->lastx, gg->lasty, &gg->style.point);

	} else
		gh->flags |= GGRAPH_FLG_CONNECTPOINTS;

	// Save this point for next time.
	gg->lastx = x;
	gg->lasty = y;

	// Draw this point.
	pointto(gg, x, y, &gg->style.point);

	#undef gg
}

void gwinGraphDrawPoints(GHandle gh, const GGraphPoint *points, unsigned count) {
	#define gg	((GGraphObject *)gh)
	unsigned			i;
	const GGraphPoint	*p;

	if (gh->type != GW_GRAPH)
		return;

	// Draw the connecting lines
	for(p = points, i = 0; i < count; p++, i++) {
		if ((gh->flags & GGRAPH_FLG_CONNECTPOINTS)) {
			// Draw the line
			lineto(gg, gg->lastx, gg->lasty, p->x, p->y, &gg->style.line);

			// Redraw the previous point because the line may have overwritten it
			if (i == 0)
				pointto(gg, gg->lastx, gg->lasty, &gg->style.point);

		} else
			gh->flags |= GGRAPH_FLG_CONNECTPOINTS;

		// Save this point for next time.
		gg->lastx = p->x;
		gg->lasty = p->y;
	}


	// Draw the points.
	for(p = points, i = 0; i < count; p++, i++)
		pointto(gg, p->x, p->y, &gg->style.point);

	#undef gg
}

#endif /* GFX_USE_GWIN && GWIN_NEED_GRAPH */
/** @} */

