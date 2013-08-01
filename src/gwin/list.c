/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gwin/list.h
 * @brief	GWIN list widget header file.
 *
 * @defgroup List List
 * @ingroup GWIN
 *
 * @{
 */

#include "gfx.h"

#if GFX_USE_GWIN && GWIN_NEED_LIST

#include "gwin/class_gwin.h"
#include <string.h>

// user for the default drawing routine
#define SCROLLWIDTH		16	// the border from the scroll buttons to the frame
#define ARROW			10	// arrow side length
#define TEXTGAP			1	// extra vertical padding for text

// Macro's to assist in data type conversions
#define gh2obj		((GListObject *)gh)
#define gw2obj		((GListObject *)gw)
#define qi2li		((ListItem *)qi)
#define qix2li		((ListItem *)qix)
#define ple			((GEventGWinList *)pe)

// Flags for the GListObject
#define GLIST_FLG_MULTISELECT		(GWIN_FIRST_CONTROL_FLAG << 0)
#define GLIST_FLG_HASIMAGES			(GWIN_FIRST_CONTROL_FLAG << 1)

// Flags on a ListItem.
#define GLIST_FLG_SELECTED			0x0001

typedef struct ListItem {
	gfxQueueASyncItem	q_item;		// This must be the first member in the struct

	uint16_t			flags;
	uint16_t			param;		// A parameter the user can specify himself
	const char*			text;
	#if GWIN_NEED_LIST_IMAGES
		gdispImage*		pimg;
	#endif
} ListItem;

static void sendListEvent(GWidgetObject *gw, int item) {
	GSourceListener*	psl;
	GEvent*				pe;

	// Trigger a GWIN list event
	psl = 0;

	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;

		ple->type = GEVENT_GWIN_LIST;
		ple->list = (GHandle)gw;
		ple->item = item;

		geventSendEvent(psl);
	}
}

static void gwinListDefaultDraw(GWidgetObject* gw, void* param) {
	(void)param;

	#if GDISP_NEED_CONVEX_POLYGON
		static const point upArrow[] = { {0, ARROW}, {ARROW, ARROW}, {ARROW/2, 0} };
		static const point downArrow[] = { {0, 0}, {ARROW, 0}, {ARROW/2, ARROW} };
	#endif

	const gfxQueueASyncItem*	qi;
	int							i;
	coord_t						x, y, iheight, iwidth;
	color_t						fill;
	const GColorSet *			ps;
	#if GWIN_NEED_LIST_IMAGES
		coord_t					sy;
	#endif

	ps = (gw->g.flags & GWIN_FLG_ENABLED) ? &gw->pstyle->enabled : &gw->pstyle->disabled;
	iheight = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight) + TEXTGAP;
	x = 1;

	// the scroll area
	if (gw2obj->cnt > (gw->g.height-2) / iheight) {
		iwidth = gw->g.width - (SCROLLWIDTH+3);
		gdispFillArea(gw->g.x+iwidth+2, gw->g.y+1, SCROLLWIDTH, gw->g.height-2, gdispBlendColor(ps->fill, gw->pstyle->background, 128));
		gdispDrawLine(gw->g.x+iwidth+1, gw->g.y+1, gw->g.x+iwidth+1, gw->g.y+gw->g.height-2, ps->edge);
		#if GDISP_NEED_CONVEX_POLYGON
			gdispFillConvexPoly(gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+(ARROW/2+1), upArrow, 3, ps->fill);
			gdispFillConvexPoly(gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+gw->g.height-(ARROW+ARROW/2+1), downArrow, 3, ps->fill);
		#else
			#warning "GWIN: Lists display better when GDISP_NEED_CONVEX_POLGON is turned on"
			gdispFillArea(gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+(ARROW/2+1), ARROW, ARROW, ps->fill);
			gdispFillArea(gw->g.x+iwidth+((SCROLLWIDTH-ARROW)/2+2), gw->g.y+gw->g.height-(ARROW+ARROW/2+1), ARROW, ARROW, ps->fill);
		#endif
	} else
		iwidth = gw->g.width - 2;

	#if GWIN_NEED_LIST_IMAGES
		if ((gw->g.flags & GLIST_FLG_HASIMAGES)) {
			x += iheight;
			iwidth -= iheight;
		}
	#endif


	// Find the top item
	for (qi = gfxQueueASyncPeek(&gw2obj->list_head), i = 0; i < gw2obj->top && qi; qi = gfxQueueASyncNext(qi), i++);

	// Draw until we run out of room or items
	for (y=1; y+iheight < gw->g.height-1 && qi; qi = gfxQueueASyncNext(qi), y += iheight) {
		fill = (qi2li->flags & GLIST_FLG_SELECTED) ? ps->fill : gw->pstyle->background;
		#if GWIN_NEED_LIST_IMAGES
			if ((gw->g.flags & GLIST_FLG_HASIMAGES)) {
				// Clear the image area
				gdispFillArea(gw->g.x+1, gw->g.y+y, x-1, iheight, fill);
				if (qi2li->pimg && gdispImageIsOpen(qi2li->pimg)) {
					// Calculate which image
					sy = (qi2li->flags & GLIST_FLG_SELECTED) ? 0 : (iheight-TEXTGAP);
					if (!(gw->g.flags & GWIN_FLG_ENABLED))
						sy += 2*(iheight-TEXTGAP);
					while (sy > qi2li->pimg->height)
						sy -= iheight-TEXTGAP;
					// Draw the image
					gdispImageSetBgColor(qi2li->pimg, fill);
					gdispImageDraw(qi2li->pimg, gw->g.x+1, gw->g.y+y, iheight-TEXTGAP, iheight-TEXTGAP, 0, sy);
				}
			}
		#endif
		gdispFillStringBox(gw->g.x+x, gw->g.y+y, iwidth, iheight, qi2li->text, gwinGetDefaultFont(), ps->text, fill, justifyLeft);
	}

	// Fill any remaining item space
	if (y < gw->g.height-1)
		gdispFillArea(gw->g.x+1, gw->g.y+y, iwidth, gw->g.height-1-y, gw->pstyle->background);

	// the list frame
	gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, ps->edge);
}

#if GINPUT_NEED_MOUSE
	// a mouse down has occurred over the list area
	static void MouseDown(GWidgetObject* gw, coord_t x, coord_t y) {
		const gfxQueueASyncItem*	qi;
		int							item, i, pgsz;
		coord_t						iheight;
		(void)						x;

		iheight = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight) + TEXTGAP;
		pgsz = (gw->g.height-2)/iheight;
		if (pgsz < 1) pgsz = 1;

		// Handle click over the scroll bar
		if (gw2obj->cnt > pgsz && x >= gw->g.width-(SCROLLWIDTH+2)) {
			if (y < 2*ARROW) {
				if (gw2obj->top > 0) {
					gw2obj->top--;
					_gwidgetRedraw(&gw->g);
				}
			} else if (y >= gw->g.height - 2*ARROW) {
				if (gw2obj->top < gw2obj->cnt - pgsz) {
					gw2obj->top++;
					_gwidgetRedraw(&gw->g);
				}
			} else if (y < gw->g.height/2) {
				if (gw2obj->top > 0) {
					if (gw2obj->top > pgsz)
						gw2obj->top -= pgsz;
					else
						gw2obj->top = 0;
					_gwidgetRedraw(&gw->g);
				}
			} else {
				if (gw2obj->top < gw2obj->cnt - pgsz) {
					if (gw2obj->top < gw2obj->cnt - 2*pgsz)
						gw2obj->top += pgsz;
					else
						gw2obj->top = gw2obj->cnt - pgsz;
					_gwidgetRedraw(&gw->g);
				}
			}
			return;
		}

		// Handle click over the list area
		item = gw2obj->top + y / iheight;

		if (item < 0 || item >= gw2obj->cnt)
			return;

		for(qi = gfxQueueASyncPeek(&gw2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
			if ((gw->g.flags & GLIST_FLG_MULTISELECT)) {
				if (item == i) {
					qi2li->flags ^= GLIST_FLG_SELECTED;
					break;
				}
			} else {
				if (item == i)
					qi2li->flags |= GLIST_FLG_SELECTED;
				else
					qi2li->flags &=~ GLIST_FLG_SELECTED;
			}
		}

		_gwidgetRedraw(&gw->g);
		sendListEvent(gw, item);
	}
#endif

#if GINPUT_NEED_TOGGLE
	// a toggle-on has occurred
	static void ToggleOn(GWidgetObject *gw, uint16_t role) {
		const gfxQueueASyncItem	*	qi;
		const gfxQueueASyncItem	*	qix;
		int							i;

		switch (role) {
			// select down
			case 0:
				for (i = 0, qi = gfxQueueASyncPeek(&gw2obj->list_head); qi; qi = gfxQueueASyncNext(qi), i++) {
					if ((qi2li->flags & GLIST_FLG_SELECTED)) {
						qix = gfxQueueASyncNext(qi);
						if (qix) {
							qi2li->flags &=~ GLIST_FLG_SELECTED;
							qix2li->flags |= GLIST_FLG_SELECTED;
							_gwidgetRedraw(&gw->g);
						}
						break;
					}
				}
				break;

			// select up
			case 1:
				qi = gfxQueueASyncPeek(&gw2obj->list_head);
				qix = 0;

				for (i = 0; qi; qix = qi, qi = gfxQueueASyncNext(qi), i++) {
					if ((qi2li->flags & GLIST_FLG_SELECTED))
						if (qix) {
							qi2li->flags &=~ GLIST_FLG_SELECTED;
							qix2li->flags |= GLIST_FLG_SELECTED;
							_gwidgetRedraw(&gw->g);
						}
						break;
					}
				}
				break;
		}
	}

	static void ToggleAssign(GWidgetObject *gw, uint16_t role, uint16_t instance) {
		if (role)
			gw2obj->t_up = instance;
		else
			gw2obj->t_dn = instance;
	}

	static uint16_t ToggleGet(GWidgetObject *gw, uint16_t role) {
		return role ? gw2obj->t_up : gw2obj->t_dn;
	}
#endif

static void _destroy(GHandle gh) {
	const gfxQueueASyncItem* qi;

	while((qi = gfxQueueASyncGet(&gh2obj->list_head)))
		gfxFree((void *)qi);

	_gwidgetDestroy(gh);
}

static const gwidgetVMT listVMT = {
	{
		"List",					// The class name
		sizeof(GListObject),	// The object size
		_destroy,				// The destroy routine
		_gwidgetRedraw,			// The redraw routine
		0,						// The after-clear routine
	},
	gwinListDefaultDraw,		// default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			MouseDown,
			0,
			0,
		},
	#endif
	#if GINPUT_NEED_TOGGLE
		{
			2,					// two toggle roles
			ToggleAssign,		// Assign toggles
			ToggleGet,			// get toggles
			0,
			ToggleOn,			// process toggle on event
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,
			0,
			0,
			0,
		},
	#endif
};

GHandle gwinListCreate(GListObject* gobj, GWidgetInit* pInit, bool_t multiselect) {
	if (!(gobj = (GListObject *)_gwidgetCreate(&gobj->w, pInit, &listVMT)))
		return 0;

	// initialize the item queue
	gfxQueueASyncInit(&gobj->list_head);
	gobj->cnt = 0;
	gobj->top = 0;
	if (multiselect)
		gobj->w.g.flags |= GLIST_FLG_MULTISELECT;

	gwinSetVisible(&gobj->w.g, pInit->g.show);

	return (GHandle)gobj;
}

int gwinListAddItem(GHandle gh, const char* item_name, bool_t useAlloc) {
	ListItem	*newItem;

	if (useAlloc) {
		if (!(newItem = (ListItem *)gfxAlloc(sizeof(ListItem)+strlen(item_name)+1)))
			return -1;

		strcpy((char *)(newItem+1), item_name);
		item_name = (const char *)(newItem+1);
	} else {
		if (!(newItem = (ListItem *)gfxAlloc(sizeof(ListItem))))
			return -1;
	}

	// the item is not selected when added
	newItem->flags = 0;
	newItem->param = 0;
	newItem->text = item_name;
	#if GWIN_NEED_LIST_IMAGES
		newItem->pimg = 0;
	#endif

	// select the item if it's the first in the list
	if (gh2obj->cnt == 0 && !(gh->flags & GLIST_FLG_MULTISELECT))
		newItem->flags |= GLIST_FLG_SELECTED;

	// add the new item to the list
	gfxQueueASyncPut(&gh2obj->list_head, &newItem->q_item);

	// increment the total amount of entries in the list widget
	gh2obj->cnt++;

	_gwidgetRedraw(gh);

	// return the position in the list (-1 because we start with index 0)
	return gh2obj->cnt-1;
}

const char* gwinListItemGetText(GHandle gh, int item) {
	const gfxQueueASyncItem*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item >= gh2obj->cnt)
		return 0;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return qi2li->text;
	}
	return 0;
}

int gwinListFindText(GHandle gh, const char* text) {
	const gfxQueueASyncItem*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	// watch out for NULL pointers
	if (!text)
		return -1;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (strcmp(((ListItem *)qi)->text, text) == 0)
			return i;	
	}

	return -1;
}

int gwinListGetSelected(GHandle gh) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	// Multi-select always returns -1. Use gwinListItemIsSelected() instead
	if ((gh->flags & GLIST_FLG_MULTISELECT))
		return -1;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (qi2li->flags & GLIST_FLG_SELECTED)
			return i;
	}

	return -1;
}

void gwinListItemSetParam(GHandle gh, int item, uint16_t param) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item) {
			qi2li->param = param;
			break;
		}
	}
}

void gwinListDeleteAll(GHandle gh) {
	gfxQueueASyncItem* qi;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	while((qi = gfxQueueASyncGet(&gh2obj->list_head)))
		gfxFree(qi);

	gh->flags &= ~GLIST_FLG_HASIMAGES;
	gh2obj->cnt = 0;
	gh2obj->top = 0;
	_gwidgetRedraw(gh);
}

void gwinListItemDelete(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item >= gh2obj->cnt)
		return;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item) {
			gfxQueueASyncRemove(&gh2obj->list_head, (gfxQueueASyncItem*)qi);
			gfxFree((void *)qi);
			if (gh2obj->top >= item && gh2obj->top)
				gh2obj->top--;
			_gwidgetRedraw(gh);
			break;
		}
	}
}

uint16_t gwinListItemGetParam(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return 0;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return qi2li->param;
	}
	return 0;
}

bool_t gwinListItemIsSelected(GHandle gh, int item) {
	const gfxQueueASyncItem	*	qi;
	int							i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return FALSE;

	// watch out for an invalid item
	if (item < 0 || item > (gh2obj->cnt) - 1)
		return FALSE;

	for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (i == item)
			return (qi2li->flags &  GLIST_FLG_SELECTED) ? TRUE : FALSE;
	}
	return FALSE;
}

int gwinListItemCount(GHandle gh) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	return gh2obj->cnt;
}

#if GWIN_NEED_LIST_IMAGES
	void gwinListItemSetImage(GHandle gh, int item, gdispImage *pimg) {
		const gfxQueueASyncItem	*	qi;
		int							i;

		// is it a valid handle?
		if (gh->vmt != (gwinVMT *)&listVMT)
			return;

		// watch out for an invalid item
		if (item < 0 || item > (gh2obj->cnt) - 1)
			return;

		for(qi = gfxQueueASyncPeek(&gh2obj->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
			if (i == item) {
				qi2li->pimg = pimg;
				if (pimg)
					gh->flags |= GLIST_FLG_HASIMAGES;
				break;
			}
		}
	}
#endif

#endif // GFX_USE_GWIN && GWIN_NEED_LIST
/** @} */

