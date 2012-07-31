/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

                                      ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/
/*
   Concepts and parts of this file have been contributed by:
		Joel Bodenmann aka Tectu	-> Maintainer
		Andrew Hannam aka inmarket	-> Framework
		Badger						-> console implementation and FSMC
		Abhishek 					-> font rendering
		Ben William					-> fastMath and lcdDrawEllipse()
		Dongxu Li aka dxli			-> lcdDrawEllipse() filled option
*/

/*
	This file is an emulation of the GLCD interface using the
	new GDISP interface. It is probably not a perfect replica,
	some code changes may be necessary.
	Note it does not replicate the GLCD low level driver, just
	the high level interface.
*/

#ifndef GLCD_H
#define GLCD_H

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#define PORTRAIT	(lcdGetOrientation() == portrait || lcdGetOrientation() == portraitInv)
#define LANDSCAPE	(lcdGetOrientation() == landscape || lcdGetOrientation() == landscapeInv)

#define RGB565CONVERT(r, g, b)	RGB2COLOR(r,g,b)

enum orientation {portrait, landscape, portraitInv, landscapeInv};
enum filled {frame, filled};
enum transparency {solid, transparent};
enum powermode {powerOff, powerOn, sleepOn, sleepOff};
#define sleepOn		powerSleep
#define sleepOff	powerOn

#define font_Small					(&fontSmall)
#define font_Larger					(&fontLarger)
#define font_MediumBold				(&fontUI1)
#define font_LargeNumbers			(&fontLargeNumbers)

#define GLCDDriver	GDISPDriver
#define GLCDD1		GDISP1

enum glcd_result {	GLCD_DONE,
					GLCD_FAILED,
					GLCD_PROGRESS,
				};

typedef enum glcd_result glcd_result_t;

/* Core functions */
#define lcdInit(dvr)							gdispInit(dvr)
#define lcdClear(color)							(gdispClear(color), GLCD_DONE)
#define lcdSetOrientation(newO)					(gdispSetOrientation(newO), (GDISP1.Orientation == (newO) ? GLCD_DONE : GLCD_FAILED))
#define lcdFillArea(x0,y0,x1,y1,c)				(gdispFillArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c)), GLCD_DONE)
#define lcdWriteArea(x0,y0,x1,y1,b,n)			(gdispBlitArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(b)), GLCD_DONE)
#define lcdSetPowerMode(pm)						(gdispSetPowerMode(pm), (GDISP1.Powermode == (pm) ? GLCD_DONE : GLCD_FAILED))

/* Drawing functions */
#define lcdDrawPixel(x,y,c)						(gdispDrawPixel((x),(y),(c)), GLCD_DONE)
#define lcdDrawLine(x0,y0,x1,y1,c)				gdispDrawLine((x0),(y0),(x1),(y1),(c))
#define lcdDrawRect(x0,y0,x1,y1,f,c)			{if(f) gdispFillArea((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c)); else gdispDrawBox((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(c));}
#define lcdDrawRectString(x0,y0,x1,y1,s,f,c,b)	gdispFillStringBox((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,(s),(f),(c),(b),justifyLeft)
#define lcdDrawCircle(x,y,r,f,c)				{if(f) gdispFillCircle((x),(y),(r),(c)); else gdispDrawCircle((x),(y),(r),(c));}
#define lcdDrawEllipse(x,y,a,b,f,c)				{if(f) gdispFillEllipse((x),(y),(a),(b),(c)); else gdispDrawEllipse((x),(y),(a),(b),(c));}

/* Text Rendering Functions */
#define lcdDrawChar(x,y,h,f,c,b,t)				({if(t) gdispDrawChar((x),(y),(h),(f),(c)); else gdispFillChar((x),(y),(h),(f),(c),(b));}, (gdispGetCharWidth((h),(f))+(f)->charPadding))
#define lcdDrawString(x,y,s,f,c,b,t)			({if(t) gdispDrawString((x),(y),(s),(f),(c)); else gdispFillString((x),(y),(s),(f),(c),(b));}, (gdispGetStringWidth((s),(f))+(f)->charPadding))

/* Character measuring functions */
#define lcdMeasureChar(h,f)						(gdispGetCharWidth((h),(f))+(f)->charPadding)
#define lcdMeasureString(s,f)					(gdispGetStringWidth((s),(f))+(f)->charPadding)
#define lcdGetFontHeight(font)					gdispGetFontMetric(font, fontHeight)

/* Size and orientation related */
#define lcdGetHeight()							(GDISP1.Height)
#define lcdGetWidth()							(GDISP1.Width)
#define lcdGetOrientation()						(GDISP1.Orientation)

/* BGR->RGB and pixel readback */
#define lcdBGR2RGB(c)							RGB2COLOR(BLUE_OF(c),GREEN_OF(c),RED_OF(c))
#define lcdGetPixelColor(x,y)					gdispGetPixelColor((x),(y))

/* Scrolling function */
#define lcdVerticalScroll(x0,y0,x1,y1,l)		gdispVerticalScroll((x0),(y0),(x1)-(x0)+1,(y1)-(y0)+1,l,Black)

#endif
