/*
    ChibiOS/RT - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS-LCD-Driver.

    ChibiOS-LCD-Driver is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS-LCD-Driver is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
	This file is an emulation of the GLCD interface using the
	new GDISP interface. It is probably not a perfect replica,
	some code changes may be necessary.
	Note it does not replicate the GLCD low level driver, just
	the high level interface.
	You may also need to define the various GDISP_NEED_XXX in your
	halconf.h in order to turn on the functionality you need.
*/

#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#define PORTRAIT	(lcdGetOrientation() == portrait || lcdGetOrientation() == portraitInv)
#define LANDSCAPE	(lcdGetOrientation() == landscape || lcdGetOrientation() == landscapeInv)

#define RGB565CONVERT(r, g, b)	RGB2COLOR(r,g,b)

enum filled {frame, filled};
enum transparency {solid, transparent};
#define sleepOn		powerSleep
#define sleepOff	powerOn

#define font_Small						(&fontSmall)
#define font_SmallDouble				(&fontSmallDouble)
#define font_SmallNarrow				(&fontSmall)
#define font_Larger						(&fontLarger)
#define font_LargerDouble				(&fontLargerDouble)
#define font_LargerNarrow				(&fontLargerNarrow)
#define font_MediumBold					(&fontUI1)
#define font_MediumBoldDouble			(&fontUI1Double)
#define font_MediumBoldNarrow			(&fontUI1Narrow)
#define font_LargeNumbers				(&fontLargeNumbers)
#define font_LargeNumbersDouble			(&fontLargeNumbersDouble)
#define font_LargeNumbersNarrow			(&fontLargeNumbersNarrow)

enum glcd_result {	GLCD_DONE,
					GLCD_FAILED,
					GLCD_PROGRESS,
				};

typedef enum glcd_result glcd_result_t;

/* Core functions */
#define lcdInit(dvr)							gdispInit(dvr)
static __inline glcd_result_t lcdClear(color_t color) {
	gdispClear(color);
	return GLCD_DONE;
}
static __inline glcd_result_t lcdSetOrientation(enum orientation newO) {
	gdispControl(GDISP_CONTROL_ORIENTATION, (void *)(int)newO);
	return ((enum orientation)(unsigned)gdispQuery(GDISP_QUERY_ORIENTATION)) == (newO) ? GLCD_DONE : GLCD_FAILED;
}
static __inline glcd_result_t lcdFillArea(coord_t x0, coord_t y0, coord_t x1, coord_t y1,color_t c) {
	gdispFillArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c));
	return GLCD_DONE;
}
static __inline glcd_result_t lcdWriteArea(coord_t x0, coord_t y0, coord_t x1, coord_t y1, const pixel_t *b, coord_t n) {
		(void)n;
		gdispBlitArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(b));
		return GLCD_DONE;
}
static __inline glcd_result_t lcdSetPowerMode(enum powermode pm) {
	gdispControl(GDISP_CONTROL_POWER, (void *)(int)pm);
	return ((enum powermode)(unsigned)gdispQuery(GDISP_QUERY_POWER)) == (pm) ? GLCD_DONE : GLCD_FAILED;
}

/* Drawing functions */
static __inline glcd_result_t lcdDrawPixel(coord_t x, coord_t y, color_t c) {
	gdispDrawPixel((x),(y),(c));
	return GLCD_DONE;
}
#define lcdDrawLine(x0,y0,x1,y1,c)				gdispDrawLine((x0),(y0),(x1),(y1),(c))
#define lcdDrawRect(x0,y0,x1,y1,f,c)			{if(f) gdispFillArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c)); else gdispDrawBox((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c));}
#define lcdDrawRectString(x0,y0,x1,y1,s,f,c,b)	gdispFillStringBox((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(s),(f),(c),(b),justifyLeft)
#define lcdDrawCircle(x,y,r,f,c)				{if(f) gdispFillCircle((x),(y),(r),(c)); else gdispDrawCircle((x),(y),(r),(c));}
#define lcdDrawEllipse(x,y,a,b,f,c)				{if(f) gdispFillEllipse((x),(y),(a),(b),(c)); else gdispDrawEllipse((x),(y),(a),(b),(c));}

/* Text Rendering Functions */
static __inline coord_t lcdDrawChar(coord_t x, coord_t y, char h, font_t f, color_t c, color_t b, bool_t t) {
	if (t)
		gdispDrawChar((x),(y),(h),(f),(c));
	else
		gdispFillChar((x),(y),(h),(f),(c),(b));
	return gdispGetCharWidth((h),(f))+gdispGetFontMetric((f), fontCharPadding);
}
static __inline coord_t lcdDrawString(coord_t x, coord_t y, const char *s, font_t f, color_t c, color_t b, bool_t t) {
	if (t)
		gdispDrawString((x),(y),(s),(f),(c));
	else
		gdispFillString((x),(y),(s),(f),(c),(b));
	return gdispGetStringWidth((s),(f))+gdispGetFontMetric((f), fontCharPadding);
}

/* Character measuring functions */
#define lcdMeasureChar(h,f)						(gdispGetCharWidth((h),(f))+gdispGetFontMetric((f), fontCharPadding))
#define lcdMeasureString(s,f)					(gdispGetStringWidth((s),(f))+gdispGetFontMetric((f), fontCharPadding))
#define lcdGetFontHeight(f)						gdispGetFontMetric((f), fontHeight)

/* Size and orientation related */
#define lcdGetHeight()							((coord_t)(unsigned)gdispQuery(GDISP_QUERY_HEIGHT))
#define lcdGetWidth()							((coord_t)(unsigned)gdispQuery(GDISP_QUERY_WIDTH))
#define lcdGetOrientation()						((enum orientation)(unsigned)gdispQuery(GDISP_QUERY_ORIENTATION))

/* BGR->RGB and pixel readback */
#define lcdBGR2RGB(c)							RGB2COLOR(BLUE_OF(c),GREEN_OF(c),RED_OF(c))
#define lcdGetPixelColor(x,y)					gdispGetPixelColor((x),(y))

/* Scrolling function */
#define lcdVerticalScroll(x0,y0,x1,y1,l)		gdispVerticalScroll((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,l,Black)

#endif
