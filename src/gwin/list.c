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

#define widget(gh)	((GListObject *)gh)

#define GLIST_FLG_MULTISELECT		(GWIN_FIRST_CONTROL_FLAG << 0)
#define GLIST_FLG_HASIMAGES			(GWIN_FIRST_CONTROL_FLAG << 1)
#define GLIST_FLG_SELECTED			(GWIN_FIRST_CONTROL_FLAG << 2)

/*
Use gw->pstyle->background for the unselected fill.
Use gw->pstyle->enabled/disabled->text for unselected text color
Use gw->pstyle->enabled/disabled->edge for the surounding box
Use gw->pstyle->enabled/disabled->fill for the selected text fill
or gw->pstyle->pressed->fill for the selected text fill (which ever looks best)
and use gw->pstyle->pressed->text for the selected text color
*/

typedef struct ListItem {
	gfxQueueASyncItem	q_item;		// This must be the first member in the struct

	uint16_t 			id;
	uint16_t			flags;
	    #define LISTITEM_ALLOCEDTEXT      0x0001
	    #define LISTITEM_SELECTED         0x0002
	uint16_t			uparam;
	const char*			text;
	#if GWIN_LIST_IMAGES
	gdispImage*			pimg;
	#endif
} ListItem;

static void sendListEvent(GWidgetObject *gw) {
	GSourceListener*	psl;
	GEvent*				pe;
	#define pse			((GEventGWinList *)pe)

	// Trigger a GWIN list event
	psl = 0;
	while ((psl = geventGetSourceListener(GWIDGET_SOURCE, psl))) {
		if (!(pe = geventGetEventBuffer(psl)))
			continue

		pse->type = GEVENT_GWIN_SLIDER;
		pse->list = (GHandle)gw;
		pse->item = 42;

		geventSendEvent(psl);
	}

	#undef pse	
}

static void gwinListDefaultDraw(GWidgetObject* gw, void* param) {
	#define gcw			((GListObject *)gw)
	(void)param;

	uint16_t i, fheight;
	gfxQueueASyncItem* qi;

	fheight = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight);	

	gdispDrawBox(gw->g.x, gw->g.y, gw->g.width, gw->g.height, Black);
	
	for (qi = gfxQueueASyncPeek(&gcw->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i += fheight + 4) {
		if (((ListItem*)qi)->flags & GLIST_FLG_SELECTED) 
			gdispFillStringBox(gw->g.x + 3, gw->g.y + 3 + i, gw->g.width - 6, fheight, ((ListItem*)qi)->text, gwinGetDefaultFont(), White, Black, justifyLeft);
		else
			gdispFillStringBox(gw->g.x + 3, gw->g.y + 3 + i, gw->g.width - 6, fheight, ((ListItem*)qi)->text, gwinGetDefaultFont(), Black, White, justifyLeft);
	}

	#undef gcw
}

#if GINPUT_NEED_MOUSE
	// A mouse down has occured over the list area
	static void MouseDown(GWidgetObject* gw, coord_t x, coord_t y) {
		#define gcw			((GListObject *)gw)

		uint16_t i, item_id, item_height;
		gfxQueueASyncItem* qi;

		item_height = gdispGetFontMetric(gwinGetDefaultFont(), fontHeight) + 2;

		item_id = (y - gw->g.y) / item_height;
		printf("item_id = %d\r\n", item_id);

		for(qi = gfxQueueASyncPeek(&gcw->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {
			if (item_id == i)
				((ListItem*)qi)->flags |= GLIST_FLG_SELECTED;
			else
				((ListItem*)qi)->flags &=~ GLIST_FLG_SELECTED;
		}

		_gwidgetRedraw((GHandle)gw);
		sendListEvent(gw);

		#undef gcw
	}
#endif

static const gwidgetVMT listVMT = {
	{
		"List",					// The class name
		sizeof(GListObject),	// The object size
		_gwidgetDestroy,		// The destroy routine
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
			0,
			0,
			0,
			0,
			0,
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
	ListItem* newItem;

	if (useAlloc) {
		newItem = gfxAlloc(sizeof(newItem) + strlen(item_name) + 1);

		// allocate string
		newItem->text = gfxAlloc(strlen(item_name) + 1);
		if (newItem->text == NULL)
			return -1;

		// assign text
		newItem->text = item_name;		

	} else {
		newItem = gfxAlloc(sizeof(newItem));

		if (newItem == NULL)
			return -1;
	}

	// the item is not selected when added
	newItem->flags &=~ GLIST_FLG_SELECTED;

	// add the new item to the list
	gfxQueueASyncPut(&(widget(gh)->list_head), &newItem->q_item);

	// increment the total amount of entries in the list widget
	widget(gh)->cnt++;
}

int gwinListGetSelected(GHandle gh) {
	gfxQueueASyncItem* qi;
	uint16_t i;

	//for(qi = gfxQueueASyncPeek(&gcw->list_head), i = 0; qi; qi = gfxQueueASyncNext(qi), i++) {

	
}

#endif // GFX_USE_GWIN && GWIN_NEED_LIST
/** @} */

