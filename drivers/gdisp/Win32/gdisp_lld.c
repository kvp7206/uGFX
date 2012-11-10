/*
    ChibiOS/RT - Copyright (C) 2012
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
 * @file    drivers/gdisp/Win32/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for Win32.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"
#include "touchscreen.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

/* Include the emulation code for things we don't support */
#include "gdisp_emulation.c"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <wingdi.h>
#include <assert.h>

/*===========================================================================*/
/* Driver local routines    .                                                */
/*===========================================================================*/

#define APP_NAME "GDISP"

#define COLOR2BGR(c)	((((c) & 0xFF)<<16)|((c) & 0xFF00)|(((c)>>16) & 0xFF))
#define BGR2COLOR(c)	COLOR2BGR(c)

static HWND winRootWindow = NULL;
static HDC dcBuffer = NULL;
static HBITMAP dcBitmap = NULL;
static HBITMAP dcOldBitmap;
static volatile bool_t isReady = FALSE;
static coord_t	mousex, mousey;
static bool_t	mousedn;

static LRESULT myWindowProc(HWND hWnd,	UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC dc;
	PAINTSTRUCT ps;

	switch (Msg) {
	case WM_CREATE:
		break;
	case WM_LBUTTONDOWN:
		mousedn = TRUE;
		mousex = (coord_t)LOWORD(lParam); 
		mousey = (coord_t)HIWORD(lParam); 
		break;
	case WM_LBUTTONUP:
		mousedn = FALSE;
		mousex = (coord_t)LOWORD(lParam); 
		mousey = (coord_t)HIWORD(lParam); 
		break;
	case WM_MOUSEMOVE:
		mousex = (coord_t)LOWORD(lParam); 
		mousey = (coord_t)HIWORD(lParam); 
		break;
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
		break;
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		break;
	case WM_PAINT:
		dc = BeginPaint(hWnd, &ps);
		BitBlt(dc, ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			dcBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		SelectObject(dcBuffer, dcOldBitmap);
		DeleteDC(dcBuffer);
		DeleteObject(dcBitmap);
		winRootWindow = NULL;
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

static DWORD WINAPI WindowThread(LPVOID lpParameter) {
	(void)lpParameter;
	
	MSG msg;
	HANDLE hInstance;
	WNDCLASS wc;
	RECT	rect;

	hInstance = GetModuleHandle(NULL);

	wc.style           = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC;
	wc.lpfnWndProc     = (WNDPROC)myWindowProc;
	wc.cbClsExtra      = 0;
	wc.cbWndExtra      = 0;
	wc.hInstance       = hInstance;
	wc.hIcon           = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor         = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground   = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName    = NULL;
	wc.lpszClassName   = APP_NAME;
	RegisterClass(&wc);

	rect.top = 0; rect.bottom = GDISP.Height;
	rect.left = 0; rect.right = GDISP.Width;
	AdjustWindowRect(&rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 0);
	winRootWindow = CreateWindow(APP_NAME, "", WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 0, 0,
			rect.right-rect.left, rect.bottom-rect.top, 0, 0, hInstance, NULL);
	assert(winRootWindow != NULL);

	HDC dc = GetDC(winRootWindow);

	GetClientRect(winRootWindow, &rect);
	GDISP.Width = rect.right-rect.left;
	GDISP.Height = rect.bottom - rect.top;
	dcBitmap = CreateCompatibleBitmap(dc, GDISP.Width, GDISP.Height);
	dcBuffer = CreateCompatibleDC(dc);
	dcOldBitmap = SelectObject(dcBuffer, dcBitmap);
	ReleaseDC(winRootWindow, dc);

	ShowWindow(winRootWindow, SW_SHOW);
	UpdateWindow(winRootWindow);
	isReady = TRUE;

	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ExitProcess(0);
	return msg.wParam;
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/* ---- Required Routines ---- */
/*
	The following 2 routines are required.
	All other routines are optional.
*/

/**
 * @brief   Low level GDISP driver initialisation.
 * @return	TRUE if successful, FALSE on error.
 *
 * @notapi
 */
bool_t GDISP_LLD(init)(void) {
	RECT rect;

	/* Set the window dimensions */
	GetWindowRect(GetDesktopWindow(), &rect);
	GDISP.Width = rect.right - rect.left;
	GDISP.Height = rect.bottom - rect.top;
	if (GDISP.Width > GDISP_SCREEN_WIDTH)
		GDISP.Width = GDISP_SCREEN_WIDTH;
	if (GDISP.Height > GDISP_SCREEN_HEIGHT)
		GDISP.Height = GDISP_SCREEN_HEIGHT;

	/* Initialise the window */
	CreateThread(0, 0, WindowThread, 0, 0, 0);
	while (!isReady)
		Sleep(1);

	/* Initialise the GDISP structure to match */
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		GDISP.clipx0 = 0;
		GDISP.clipy0 = 0;
		GDISP.clipx1 = GDISP.Width;
		GDISP.clipy1 = GDISP.Height;
	#endif
	return TRUE;
}

/**
 * @brief   Draws a pixel on the display.
 *
 * @param[in] x        X location of the pixel
 * @param[in] y        Y location of the pixel
 * @param[in] color    The color of the pixel
 *
 * @notapi
 */
void GDISP_LLD(drawpixel)(coord_t x, coord_t y, color_t color) {
	HDC dc;

	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif

	// Draw the pixel in the buffer
	color = COLOR2BGR(color);
	SetPixel(dcBuffer, x, y, color);
	
	// Draw the pixel again directly on the screen.
	// This is cheaper than invalidating a single pixel in the window
	dc = GetDC(winRootWindow);
	SetPixel(dc, x, y, color);
	ReleaseDC(winRootWindow, dc);
}

/* ---- Optional Routines ---- */

#if GDISP_HARDWARE_LINES || defined(__DOXYGEN__)
	/**
	 * @brief   Draw a line.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x0, y0   The start of the line
	 * @param[in] x1, y1   The end of the line
	 * @param[in] color    The color of the line
	 *
	 * @notapi
	 */
	void GDISP_LLD(drawline)(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color) {
		POINT p;
		HPEN pen;
		HDC dc;
		HGDIOBJ old;
		HRGN	clip;

		#if GDISP_NEED_CLIP
			clip = NULL;
			if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
				clip = CreateRectRgn(GDISP.clipx0, GDISP.clipy0, GDISP.clipx1, GDISP.clipy1);
		#endif

		color = COLOR2BGR(color);
		pen = CreatePen(PS_SOLID, 1, color);
		if (pen) {
			// Draw the line in the buffer
			#if GDISP_NEED_CLIP
				if (clip) SelectClipRgn(dcBuffer, clip);
			#endif
			old = SelectObject(dcBuffer, pen);
			MoveToEx(dcBuffer, x0, y0, &p);
			LineTo(dcBuffer, x1, y1);
			SelectObject(dcBuffer, old);
			SetPixel(dcBuffer, x1, y1, color);
			#if GDISP_NEED_CLIP
				if (clip) SelectClipRgn(dcBuffer, NULL);
			#endif

			// Redrawing the line on the screen is cheaper than invalidating the whole rectangular area
			dc = GetDC(winRootWindow);
			#if GDISP_NEED_CLIP
				if (clip) SelectClipRgn(dc, clip);
			#endif
			old = SelectObject(dc, pen);
			MoveToEx(dc, x0, y0, &p);
			LineTo(dc, x1, y1);
			SelectObject(dc, old);
			SetPixel(dc, x1, y1, color);
			#if GDISP_NEED_CLIP
				if (clip) SelectClipRgn(dc, NULL);
			#endif
			ReleaseDC(winRootWindow, dc);

			DeleteObject(pen);
		}
	}
#endif

#if GDISP_HARDWARE_FILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a color.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] color    The color of the fill
	 *
	 * @notapi
	 */
	void GDISP_LLD(fillarea)(coord_t x, coord_t y, coord_t cx, coord_t cy, color_t color) {
		HDC dc;
		RECT rect;
		HBRUSH hbr;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		color = COLOR2BGR(color);
		hbr = CreateSolidBrush(color);

		if (hbr) {
			rect.bottom = y+cy;
			rect.top = y;
			rect.left = x;
			rect.right = x+cx;
			
			// Fill the area
			FillRect(dcBuffer, &rect, hbr);

			// Filling the area directly on the screen is likely to be cheaper than invalidating it
			dc = GetDC(winRootWindow);
			FillRect(dc, &rect, hbr);
			ReleaseDC(winRootWindow, dc);

			DeleteObject(hbr);
		}
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	/**
	 * @brief   Fill an area with a bitmap.
	 * @note    Optional - The high level driver can emulate using software.
	 *
	 * @param[in] x, y     The start filled area
	 * @param[in] cx, cy   The width and height to be filled
	 * @param[in] srcx, srcy   The bitmap position to start the fill from
	 * @param[in] srccx    The width of a line in the bitmap.
	 * @param[in] buffer   The pixels to use to fill the area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(blitareaex)(coord_t x, coord_t y, coord_t cx, coord_t cy, coord_t srcx, coord_t srcy, coord_t srccx, const pixel_t *buffer) {
		BITMAPV4HEADER bmpInfo;
		RECT	rect;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bV4Size = sizeof(bmpInfo);
		bmpInfo.bV4Width = srccx;
		bmpInfo.bV4Height = -(srcy+cy); /* top-down image */
		bmpInfo.bV4Planes = 1;
		bmpInfo.bV4BitCount = 32;
		bmpInfo.bV4SizeImage = ((srcy+cy)*srccx * 32)/8;
		bmpInfo.bV4AlphaMask = 0;
		bmpInfo.bV4RedMask		= RGB2COLOR(255,0,0);
		bmpInfo.bV4GreenMask	= RGB2COLOR(0,255,0);
		bmpInfo.bV4BlueMask		= RGB2COLOR(0,0,255);
		bmpInfo.bV4V4Compression = BI_BITFIELDS;
		bmpInfo.bV4XPelsPerMeter = 3078;
		bmpInfo.bV4YPelsPerMeter = 3078;
		bmpInfo.bV4ClrUsed = 0;
		bmpInfo.bV4ClrImportant = 0;
		bmpInfo.bV4CSType = 0; //LCS_sRGB;

		// Draw the bitmap
		SetDIBitsToDevice(dcBuffer, x, y, cx, cy, srcx, srcy, 0, cy+srcy, buffer, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);

		// Invalidate the region to get it on the screen.
		rect.bottom = y+cy;
		rect.top = y;
		rect.left = x;
		rect.right = x+cx;
		InvalidateRect(winRootWindow, &rect, FALSE);
		UpdateWindow(winRootWindow);
	}
#endif

#if (GDISP_NEED_PIXELREAD && GDISP_HARDWARE_PIXELREAD) || defined(__DOXYGEN__)
	/**
	 * @brief   Get the color of a particular pixel.
	 * @note    Optional.
	 * @note    If x,y is off the screen, the result is undefined.
	 * @return	The color of the specified pixel.
	 *
	 * @param[in] x, y     The start of the text
	 *
	 * @notapi
	 */
	color_t GDISP_LLD(getpixelcolor)(coord_t x, coord_t y) {
		color_t color;

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		color = GetPixel(dcBuffer, x, y);
		return BGR2COLOR(color);
	}
#endif

#if (GDISP_NEED_SCROLL && GDISP_HARDWARE_SCROLL) || defined(__DOXYGEN__)
	/**
	 * @brief   Scroll vertically a section of the screen.
	 * @note    Optional.
	 * @note    If x,y + cx,cy is off the screen, the result is undefined.
	 * @note    If lines is >= cy, it is equivelent to a area fill with bgcolor.
	 *
	 * @param[in] x, y     The start of the area to be scrolled
	 * @param[in] cx, cy   The size of the area to be scrolled
	 * @param[in] lines    The number of lines to scroll (Can be positive or negative)
	 * @param[in] bgcolor  The color to fill the newly exposed area.
	 *
	 * @notapi
	 */
	void GDISP_LLD(verticalscroll)(coord_t x, coord_t y, coord_t cx, coord_t cy, int lines, color_t bgcolor) {
		RECT	rect, frect;
		HBRUSH	hbr;
		
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		rect.bottom = y+cy;
		rect.top = y;
		rect.left = x;
		rect.right = x+cx;
		if (cy > lines && cy > -lines)
			ScrollDC(dcBuffer, 0, -lines, &rect, 0, 0, 0);
		bgcolor = COLOR2BGR(bgcolor);
		hbr = CreateSolidBrush(bgcolor);
		if (hbr) {
			if (lines > 0) {
				if (lines > cy) lines = cy;
				frect.bottom = y+cy;
				frect.top = y+cy-lines;
			} else {
				if (-lines > cy) lines = -cy;
				frect.top = y;
				frect.bottom = y-lines;
			}
			frect.left = x;
			frect.right = x+cx;
			FillRect(dcBuffer, &frect, hbr);
		}
		InvalidateRect(winRootWindow, &rect, FALSE);
		UpdateWindow(winRootWindow);
	}
#endif

#if GFX_USE_TOUCHSCREEN /*|| defined(__DOXYGEN__)*/

void ts_store_calibration_lld(struct cal_t *cal) {
	(void) cal;
	// Just ignore the calibration data - we implicitly know the calibration
}

struct cal_t *ts_restore_calibration_lld(void) {
	static struct cal_t cal = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
	// Our x,y is always already calibrated.
	return &cal;
}

/**
 * @brief   Low level touchscreen driver initialization.
 *
 * @param[in] ts	The touchscreen driver
 *
 * @notapi
 */
void ts_lld_init(const TouchscreenDriver *ts) {
	(void) ts;
	// Just ignore everything
}

/**
 * @brief   Reads out the X direction.
 *
 * @notapi
 */
uint16_t ts_lld_read_x(void) {
	return mousex;
}

/**
 * @brief   Reads out the Y direction.
 *
 * @notapi
 */
uint16_t ts_lld_read_y(void) {
	return mousey;
}

/**
 * @brief   Reads out the Z direction.
 *
 * @notapi
 */
uint16_t ts_lld_read_z(void) {
	return 0;
}

/*
 * @brief	for checking if touchpad is pressed or not.
 *
 * @return	1 if pressed / 0 if not pressed
 *
 * @notapi
 */
uint8_t ts_lld_pressed(void) {
	return (uint8_t)mousedn;
}

#endif /* GFX_USE_TOUCHSCREEN */

#endif /* GFX_USE_GDISP */
/** @} */

