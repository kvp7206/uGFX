/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file	include/gwin/list.h
 * @brief	GWIN list widget header file
 *
 * @defgroup List List
 * @ingroup GWIN
 *
 * @details		GWIN allows it to create a list widget.
 *
 * @pre			GFX_USE_GDISP must be set to TRUE in your gfxconf.h
 * @pre			GFX_USE_GWIN must be set to TRUE in your gfxconf.h
 * @pre			GDISP_NEED_TEXT must be set to TRUE in your gfxconf.h
 * @pre			GWIN_NEED_LIST must be set to TRUE in your gfxconf.h
 * @pre			The font you want to use must be enabled in your gfxconf.h
 *
 * @{
 */

#ifndef _GWIN_LIST_H
#define _GWIN_LIST_H

// This file is included within "gwin/gwin.h"

/**
 * @brief	The event type for a list event
 */
#define GEVENT_GWIN_LIST	(GEVENT_GWIN_FIRST+4)

/**
 * @brief	A list event
 */
typedef struct GEventGWinList {
	GEventType		type;		// The type of this event (GEVENT_GWIN_LIST)
	GHandle			list;		// The list
	int				item;		// The item that has been selected (or unselected in a multi-select listbox)
} GEventGWinList;

// A list window
typedef struct GListObject {
	GWidgetObject	w;
	int				cnt;		// Number of items currently in the list (quicker than counting each time)
	gfxQueueASync	list_head;	// The list of items
} GListObject;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief				Create a list widget
 *
 * @note				The drawing color and the background color get set to the current defaults. If you haven't called
 *						@p gwinSetDefaultColor() or @p gwinSetDefaultBgColor() then these are Black and White.
 * @note				The font gets set to the current default font. If you haven't called @p gwinSetDefaultFont() then
 *						there is no default font and text drawing operations will not display anything.
 * @note				A list remembers its normal drawing state. If there is a window manager then it is automatically
 *						redrawn if the window is moved or its visibility state is changed.
 * @note				The list contains no elements after creation.
 * @note				A slider supports mouse, toggle and dial input.
 * @note				When assigning a toggle, only one toggle is supported per role. If you try to assign more than
 *						one toggle to a role, it will forget the previous toggle. Two roles are supported:
 *						Role 0 = toggle for down, role 1 = toggle for up 
 * @note				When assigning a dial, only one dial is supported. If you try to assign more than one dial, it
 *						will forget the previous dial. Only dial role 0 is supported.
 *
 * @param[in] widget	The GListObject structure to initialize. If this is NULL, the structure is dynamically allocated.
 * @param[in] pInit		The initialization parameters to use
 *
 * @return				NULL if there is no resulting drawing area, otherwise a window handle.
 *
 * @api
 */
GHandle gwinListCreate(GListObject *widget, GWidgetInit *pInit);

/**
 * @brief				Add an item to the list
 *
 * @note				The ID you get returned is not static. If items get removed from the list, the list items get
 * 						reordered.
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The string which shall be displayed in the list afterwards
 * @param[in] useAlloc	If set to TRUE, the string will be dynamically allocated. A static buffer must be passed otherwise
 *
 * @return				The current ID of the item. The ID might change if you remove items from the middle of the list
 *
 * @api
 */
int gwinListAddItem(GHandle gh, const char* item, bool_t useAlloc);

/**
 * @brief				Get the name behind an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				The string of the list item or NULL on error
 *
 * @api
 */
char* gwinListItemGetText(GHandle gh, int item);

/**
 * @brief				Get the ID of an item with a given name
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] text		The item name
 *
 * @return				The id of the list item or -1 on error
 *
 * @api
 */
int gwinListFindText(GHandle gh, const char* text);

/**
 * @brief				Set the custom parameter of an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 * @param[in] param		The parameter to be set
 *
 * @api
 */
void gwinListItemSetParam(GHandle gh, int item, uint16_t param);

/**
 * @brief				Get the custom parameter of an item with a given ID
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				The parameter
 *
 * @api
 */
uint16_t gwinListItemGetParam(GHandle gh, int item);

/**
 * @brief				Delete all the items of the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @api
 */
void ListDeleteAll(GHandle gh);

/**
 * @brief				Delete an item from the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @api
 */
void gwinListItemDelete(GHandle gh, int item);

/**
 * @brief				Get the amount of items within the list
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @return				The amount of items in the list
 *
 * @api
 */
int gwinListItemCount(GHandle gh);

/**
 * @brief				Check if an item with a given ID is selected
 *
 * @param[in] gh		The widget handle (must be a list handle)
 * @param[in] item		The item ID
 *
 * @return				TRUE if the item is selected, FALSE otherwise
 *
 * @api
 */
bool_t gwinListItemIsSelected(GHandle gh, int item);

/**
 * @brief				Get the ID of the selected item
 *
 * @param[in] gh		The widget handle (must be a list handle)
 *
 * @return				The ID of the list item
 *
 * @api
 */
int gwinListGetSelected(GHandle gh);

#ifdef __cplusplus
}
#endif

#endif // _GWIN_LIST_H
/** @} */

