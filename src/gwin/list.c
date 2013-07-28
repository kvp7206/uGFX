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
#define BORDER			3	// the border from the the text to the frame
#define BORDER_SCROLL	20	// the border from the scroll buttons to the frame
#define ARROW			10	// arrow side length

#define widget(gh)	((GListObject *)gh)

#define GLIST_FLG_MULTISELECT		(GWIN_FIRST_CONTROL_FLAG << 0)
#define GLIST_FLG_HASIMAGES			(GWIN_FIRST_CONTROL_FLAG << 1)
#define GLIST_FLG_SELECTED			(GWIN_FIRST_CONTROL_FLAG << 2)

typedef struct ListItem {
	gfxQueueASyncItem	q_item;		// This must be the first member in the struct

	uint16_t			flags;
	uint16_t			param;		// A parameter the user can specify himself
	const char*			text;
	#if GWIN_LIST_IMAGES
	gdispImage*			pimg;
	#endif
} ListItem;

void _selectUp(GWidgetObject *gw) {
	#define gcw			((GListObject *)gw)

	gfxQueueASyncItem	*qi;
	uint16_t i;

	for (i = 0, qi = gfxQueueASyncPeek(&gcw->list_head); qi; qi = gfxQueueASyncNext(qi), i++) {
		if (((ListItem*)qi)->flags & GLIST_FLG_SELECTED && i < gcw->cnt - 1) {
			((ListItem*)qi)->flags &=~ GLIST_FLG_SELECTED;
			qi = gfxQueueASyncNext(qi);
			((ListItem*)qi)->flags |= GLIST_FLG_SELECTED;	
		}
	}

	_gwidgetRedraw((GHandle)gw);

	#undef gcw
}

static void _selectDown(GWidgetObject *gw) {
	#define gcw			((GListObject *)gw)

	_gwidgetRedraw((GHandle)gw);

	#undef gcw
}

static void sendListEvent(GWidgetObject *gw, int item) {
	GSourceListener*	psl;
	GEvent*				pe;
	#define pse			((GEventGWinList *)pe)

	// Trigger a GWIN list event
	psl = 0;

	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue;

		pse->type = GEVENT_GWIN_LIST;
		pse->list = (GHandle)gw;
		pse->item = item;

		geventSendEvent(psl);
	}
	
	#undef pse	
}

static void gwinListDefaultDraw(GWidgetObject* gw, void* param) {
	#define gcw			((GListObject *)gw)
	(void)param;

	uint16_t i, fheight;
	const point upArrow[] = { {0, ARROW}, {ARROW, ARROW}, {ARROW/2, 0} };
	const point downArrow[] = { {0, 0}, {ARROW, 0}, {ARROW/2, ARROW} };
	const gfxQueueASyncItem* qi;

	fheight = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight);	

	// the list frame
	gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->pstyle->enabled.edge);
	gdispDrawLine(gw->g.x + gw->g.width - BORDER_SCROLL, gw->g.y, gw->g.x + gw->g.width - BORDER_SCROLL, gw->g.y + gw->g.height, gw->pstyle->enabled.edge);
	gdispDrawLine(gw->g.x + gw->g.width - BORDER_SCROLL, gw->g.y + BORDER_SCROLL, gw->g.x + gw->g.width, gw->g.y + BORDER_SCROLL, gw->pstyle->enabled.edge);
	gdispDrawLine(gw->g.x + gw->g.width - BORDER_SCROLL, gw->g.y + gw->g.height - BORDER_SCROLL, gw->g.x + gw->g.width, gw->g.y + gw->g.height - BORDER_SCROLL, gw->pstyle->enabled.edge);

	// the up-button
	gdispFillConvexPoly(gw->g.x + gw->g.width - BORDER_SCROLL + ARROW/2, gw->g.y + BORDER_SCROLL - ARROW - ARROW/2, upArrow, 3, gw->pstyle->enabled.edge);
	
	// the down-button
	gdispFillConvexPoly(gw->g.x + gw->g.width - BORDER_SCROLL + ARROW/2, gw->g.y + gw->g.height - BORDER_SCROLL + ARROW/2, downArrow, 3, gw->pstyle->enabled.edge);

	for (qi = gfxQueueASyncPeek(&gcw->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i += fheight + 2*BORDER) {
		if (((ListItem*)qi)->flags & GLIST_FLG_SELECTED) {
			gdispFillStringBox(gw->g.x + BORDER, gw->g.y + BORDER + i, gw->g.width - 2*BORDER - BORDER_SCROLL, fheight, ((ListItem*)qi)->text, gwinGetDefaultFont(), gw->pstyle->background, gw->pstyle->enabled.text, justifyLeft);
		} else {
			gdispFillStringBox(gw->g.x + BORDER, gw->g.y + BORDER + i, gw->g.width - 2*BORDER - BORDER_SCROLL, fheight, ((ListItem*)qi)->text, gwinGetDefaultFont(), gw->pstyle->enabled.text, gw->pstyle->background, justifyLeft);
		}
	}

	#undef gcw
}

#if GINPUT_NEED_MOUSE
	// a mouse down has occured over the list area
	static void MouseDown(GWidgetObject* gw, coord_t x, coord_t y) {
		#define gcw			((GListObject *)gw)
		#define li			((ListItem *)qi)
		(void)				x;

		uint16_t i, item_id, item_height;
		const gfxQueueASyncItem* qi;

		item_height = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight) + 2*BORDER;
		item_id = (y - gw->g.y) / item_height;

		for(qi = gfxQueueASyncPeek(&gcw->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
			if (item_id == i)
				li->flags |= GLIST_FLG_SELECTED;
			else
				li->flags &=~ GLIST_FLG_SELECTED;
		}

		_gwidgetRedraw((GHandle)gw);

		// do not generate an event if an empty section of the list has has been selected
		if (item_id < 0 || item_id > gcw->cnt - 1)
			return;

		sendListEvent(gw, item_id);

		#undef gcw
		#undef li
	}
#endif

#if GINPUT_NEED_TOGGLE
	// a toggle-on has occurred
	static void ToggleOn(GWidgetObject *gw, uint16_t role) {
		#define gcw			((GListObject *)gw)

		switch (role) {
			// select up
			case 0:
				_selectUp(gw);
				break;

			// select down
			case 1:
				_selectDown(gw);
				break;
		}

		#undef gcw
	}
#endif

static void _destroy(GHandle gh) {
	const gfxQueueASyncItem* qi;

	while((qi = gfxQueueASyncGet(&((GListObject *)gh)->list_head)))
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
			ToggleAssin,		// Assign toggles
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

GHandle gwinListCreate(GListObject* widget, GWidgetInit* pInit) {
	if (!(widget = (GListObject *)_gwidgetCreate(&widget->w, pInit, &listVMT)))
		return 0;

	// initialize the item queue
	gfxQueueASyncInit(&(widget->list_head));
	widget->cnt = 0;

	gwinSetVisible(&widget->w.g, pInit->g.show);

	return (GHandle)widget;
}

int gwinListAddItem(GHandle gh, const char* item_name, bool_t useAlloc) {
	size_t		sz;
	ListItem	*newItem;

	sz = useAlloc ? sizeof(ListItem)+strlen(item_name)+1 : sizeof(ListItem);

	if (!(newItem = (ListItem *)gfxAlloc(sz)))
		return -1;

	if (useAlloc) {
		strcpy((char *)(newItem+1), item_name);
		item_name = (const char *)(newItem+1);
	}

	// the item is not selected when added
	newItem->flags = 0;
	newItem->param = 0;
	newItem->text = item_name;

	// select the item if it's the first in the list
	if (widget(gh)->cnt == 0)
		newItem->flags |= GLIST_FLG_SELECTED;

	// add the new item to the list
	gfxQueueASyncPut(&widget(gh)->list_head, &newItem->q_item);

	// increment the total amount of entries in the list widget
	widget(gh)->cnt++;

	// return the position in the list (-1 because we start with index 0)
	return widget(gh)->cnt-1;
}

char* gwinListItemGetText(GHandle gh, int item) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item > (widget(gh)->cnt) - 1)
		return 0;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);
	
	for (i = 0; i < item; i++) {
		qi = gfxQueueASyncNext(qi);
	}

	return ((ListItem*)qi)->text;
}

int gwinListFindText(GHandle gh, const char* text) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	// watch out for NULL pointers
	if (!text)
		return -1;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);
	
	for(qi = gfxQueueASyncPeek(&widget(gh)->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (strcmp(((ListItem *)qi)->text, text) == 0)
			return i;	
	}

	return -1;
}

int gwinListGetSelected(GHandle gh) {
	const gfxQueueASyncItem	*qi;
	int i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return -1;

	for(qi = gfxQueueASyncPeek(&widget(gh)->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
		if (((ListItem*)qi)->flags & GLIST_FLG_SELECTED)
			return i;
	}

	return -1;
}

void gwinListItemSetParam(GHandle gh, int item, uint16_t param) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item > (widget(gh)->cnt) - 1)
		return;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);
	
	for (i = 0; i < item; i++) {
		((ListItem*)qi)->param = param;
	}

	return;	
}

void gwinListDeleteAll(GHandle gh) {
	const gfxQueueASyncItem* qi;

	while((qi = gfxQueueASyncGet(&((GListObject *)gh)->list_head)))
		gfxFree((void *)qi);
}

void gwinListItemDelete(GHandle gh, int item) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return;

	// watch out for an invalid item
	if (item < 0 || item > (widget(gh)->cnt) - 1)
		return;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);

	// get our item pointer	
	for (i = 0; i < item; i++) {
		qi = gfxQueueASyncNext(qi);
	}

	gfxQueueASyncRemove(&widget(gh)->list_head, qi);
	gfxFree((void*)qi);
}

uint16_t gwinListItemGetParam(GHandle gh, int item) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	// watch out for an invalid item
	if (item < 0 || item > (widget(gh)->cnt) - 1)
		return 0;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);
	
	for (i = 0; i < item; i++) {
		qi = gfxQueueASyncNext(qi);
	}

	return ((ListItem*)qi)->param;
}

bool_t gwinListItemIsSelected(GHandle gh, int item) {
	const gfxQueueASyncItem* qi;
	uint16_t i;

	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return FALSE;

	// watch out for an invalid item
	if (item < 0 || item > (widget(gh)->cnt) - 1)
		return FALSE;

	qi = gfxQueueASyncPeek(&widget(gh)->list_head);
	
	for (i = 0; i < item; i++) {
		qi = gfxQueueASyncNext(qi);
	}

	if (((ListItem*)qi)->flags &  GLIST_FLG_SELECTED)
		return TRUE;
	else
		return FALSE;
}

int gwinListItemCount(GHandle gh) {
	// is it a valid handle?
	if (gh->vmt != (gwinVMT *)&listVMT)
		return 0;

	return widget(gh)->cnt;
}

#endif // GFX_USE_GWIN && GWIN_NEED_LIST
/** @} */

