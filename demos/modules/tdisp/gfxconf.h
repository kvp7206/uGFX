/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* GFX sub-systems to turn on */
#define GFX_USE_TDISP					TRUE
#define GFX_USE_GDISP                   FALSE
#define GFX_USE_GWIN                    FALSE
#define GFX_USE_GEVENT                  FALSE
#define GFX_USE_GTIMER                  FALSE
#define GFX_USE_GINPUT                  FALSE

/* Features for the GDISP subsystem */
#define GDISP_NEED_VALIDATION           FALSE
#define GDISP_NEED_CLIP                 FALSE
#define GDISP_NEED_TEXT                 FALSE
#define GDISP_NEED_CIRCLE               FALSE
#define GDISP_NEED_ELLIPSE              FALSE
#define GDISP_NEED_ARC                  FALSE
#define GDISP_NEED_SCROLL               FALSE
#define GDISP_NEED_PIXELREAD            FALSE
#define GDISP_NEED_CONTROL              FALSE
#define GDISP_NEED_MULTITHREAD          FALSE
#define GDISP_NEED_ASYNC                FALSE
#define GDISP_NEED_MSGAPI               FALSE

/* Features for the TDISP subsystem */
#define TDISP_NEED_4BIT_MODE			TRUE
#define TDISP_NEED_8BIT_MODE			FALSE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_SMALL        FALSE
#define GDISP_INCLUDE_FONT_LARGER       FALSE
#define GDISP_INCLUDE_FONT_UI1          FALSE
#define GDISP_INCLUDE_FONT_UI2          FALSE
#define GDISP_INCLUDE_FONT_LARGENUMBERS FALSE

/* GWIN */
#define GWIN_NEED_CONSOLE				FALSE
#define GWIN_NEED_GRAPH					FALSE
#define GWIN_NEED_BUTTON				FALSE
#define GWIN_NEED_DIAL					FALSE

/* GINPUT */
#define GINPUT_NEED_MOUSE				FALSE

#endif /* _GFXCONF_H */

