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
 * @file    drivers/multiple/Win32/gdisp_lld.c
 * @brief   GDISP Graphics Driver subsystem low level driver source for Win32.
 *
 * @addtogroup GDISP
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "gdisp.h"

#if GFX_USE_GDISP /*|| defined(__DOXYGEN__)*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <wingdi.h>
#include <assert.h>

#ifndef GINPUT_NEED_TOGGLE
	#define GINPUT_NEED_TOGGLE	FALSE
#endif
#ifndef GINPUT_NEED_MOUSE
	#define GINPUT_NEED_MOUSE	FALSE
#endif

#if GINPUT_NEED_TOGGLE
	/* Include toggle support code */
	#include "lld/ginput/toggle.h"

	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = {
		{0,	0xFF, 0x00, PAL_MODE_INPUT},
	};
#endif

#if GINPUT_NEED_MOUSE
	/* Include mouse support code */
	#include "ginput.h"
	#include "lld/ginput/mouse.h"
#endif

/* Include the emulation code for things we don't support */
#include "lld/gdisp/emulation.c"

/*===========================================================================*/
/* Driver local routines    .                                                */
/*===========================================================================*/

#define WIN32_USE_MSG_REDRAW	FALSE
#if GINPUT_NEED_TOGGLE
	#define WIN32_BUTTON_AREA		16
#else
	#define WIN32_BUTTON_AREA		0
#endif

#define APP_NAME "GDISP"

#define COLOR2BGR(c)	((((c) & 0xFF)<<16)|((c) & 0xFF00)|(((c)>>16) & 0xFF))
#define BGR2COLOR(c)	COLOR2BGR(c)

static HWND winRootWindow = NULL;
static HDC dcBuffer = NULL;
static HBITMAP dcBitmap = NULL;
static HBITMAP dcOldBitmap;
static volatile bool_t isReady = FALSE;
static coord_t	wWidth, wHeight;

#if GINPUT_NEED_MOUSE
	static coord_t	mousex, mousey;
	static uint16_t	mousebuttons;
#endif
#if GINPUT_NEED_TOGGLE
	static uint8_t	toggles = 0;
#endif

static LRESULT myWindowProc(HWND hWnd,	UINT Msg, WPARAM wParam, LPARAM lParam)
{
	HDC			dc;
	PAINTSTRUCT	ps;
	#if GINPUT_NEED_TOGGLE
		HBRUSH		hbrOn, hbrOff;
		HPEN		pen;
		RECT		rect;
		HGDIOBJ		old;
		POINT 		p;
		coord_t		pos;
		uint8_t		bit;
	#endif

	switch (Msg) {
	case WM_CREATE:
		break;
	case WM_LBUTTONDOWN:
		#if GINPUT_NEED_MOUSE
			if ((coord_t)HIWORD(lParam) < wHeight) {
				mousebuttons |= GINPUT_MOUSE_BTN_LEFT;
				goto mousemove;
			}
		#endif
		#if GINPUT_NEED_TOGGLE
			bit = 1 << ((coord_t)LOWORD(lParam)*8/wWidth);
			toggles ^= bit;
			rect.left = 0;
			rect.right = wWidth;
			rect.top = wHeight;
			rect.bottom = wHeight + WIN32_BUTTON_AREA;
			InvalidateRect(hWnd, &rect, FALSE);
			UpdateWindow(hWnd);
			#if GINPUT_TOGGLE_POLL_PERIOD == TIME_INFINITE
				ginputToggleWakeup();
			#endif
		#endif
		break;
	case WM_LBUTTONUP:
		#if GINPUT_NEED_TOGGLE
			if ((toggles & 0xF0)) {
				toggles &= 0x0F;
				rect.left = 0;
				rect.right = wWidth;
				rect.top = wHeight;
				rect.bottom = wHeight + WIN32_BUTTON_AREA;
				InvalidateRect(hWnd, &rect, FALSE);
				UpdateWindow(hWnd);
				#if GINPUT_TOGGLE_POLL_PERIOD == TIME_INFINITE
					ginputToggleWakeup();
				#endif
			}
		#endif
		#if GINPUT_NEED_MOUSE
			if ((coord_t)HIWORD(lParam) < wHeight) {
				mousebuttons &= ~GINPUT_MOUSE_BTN_LEFT;
				goto mousemove;
			}
		#endif
		break;
#if GINPUT_NEED_MOUSE
	case WM_MBUTTONDOWN:
		if ((coord_t)HIWORD(lParam) < wHeight) {
			mousebuttons |= GINPUT_MOUSE_BTN_MIDDLE;
			goto mousemove;
		}
		break;
	case WM_MBUTTONUP:
		if ((coord_t)HIWORD(lParam) < wHeight) {
			mousebuttons &= ~GINPUT_MOUSE_BTN_MIDDLE;
			goto mousemove;
		}
		break;
	case WM_RBUTTONDOWN:
		if ((coord_t)HIWORD(lParam) < wHeight) {
			mousebuttons |= GINPUT_MOUSE_BTN_RIGHT;
			goto mousemove;
		}
		break;
	case WM_RBUTTONUP:
		if ((coord_t)HIWORD(lParam) < wHeight) {
			mousebuttons &= ~GINPUT_MOUSE_BTN_RIGHT;
			goto mousemove;
		}
		break;
	case WM_MOUSEMOVE:
		if ((coord_t)HIWORD(lParam) >= wHeight)
			break;
	mousemove:
		mousex = (coord_t)LOWORD(lParam); 
		mousey = (coord_t)HIWORD(lParam); 
		#if GINPUT_MOUSE_POLL_PERIOD == TIME_INFINITE
			ginputMouseWakeup();
		#endif
		break;
#endif
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
			(ps.rcPaint.bottom > wHeight ? wHeight : ps.rcPaint.bottom) - ps.rcPaint.top,
			dcBuffer, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		#if GINPUT_NEED_TOGGLE
			if (ps.rcPaint.bottom >= wHeight) {
				pen = CreatePen(PS_SOLID, 1, COLOR2BGR(Black));
				hbrOn = CreateSolidBrush(COLOR2BGR(Blue));
				hbrOff = CreateSolidBrush(COLOR2BGR(Gray));
				old = SelectObject(dc, pen);
				MoveToEx(dc, 0, wHeight, &p);
				LineTo(dc, wWidth, wHeight);
				for(pos = 0, bit=1; pos < wWidth; pos=rect.right, bit <<= 1) {
					rect.left = pos;
					rect.right = pos + wWidth/8;
					rect.top = wHeight;
					rect.bottom = wHeight + WIN32_BUTTON_AREA;
					FillRect(dc, &rect, (toggles & bit) ? hbrOn : hbrOff);
					if (pos > 0) {
						MoveToEx(dc, rect.left, rect.top, &p);
						LineTo(dc, rect.left, rect.bottom);
					}
				}
				DeleteObject(hbrOn);
				DeleteObject(hbrOff);
				SelectObject(dc, old);
			}
		#endif
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
	HDC		dc;

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

	rect.top = 0; rect.bottom = wHeight+WIN32_BUTTON_AREA;
	rect.left = 0; rect.right = wWidth;
	AdjustWindowRect(&rect, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 0);
	winRootWindow = CreateWindow(APP_NAME, "", WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU, 0, 0,
			rect.right-rect.left, rect.bottom-rect.top, 0, 0, hInstance, NULL);
	assert(winRootWindow != NULL);


	GetClientRect(winRootWindow, &rect);
	wWidth = rect.right-rect.left;
	wHeight = rect.bottom - rect.top - WIN32_BUTTON_AREA;

	dc = GetDC(winRootWindow);
	dcBitmap = CreateCompatibleBitmap(dc, wWidth, wHeight);
	dcBuffer = CreateCompatibleDC(dc);
	ReleaseDC(winRootWindow, dc);
	dcOldBitmap = SelectObject(dcBuffer, dcBitmap);

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
	wWidth = rect.right - rect.left;
	wHeight = rect.bottom - rect.top - WIN32_BUTTON_AREA;
	if (wWidth > GDISP_SCREEN_WIDTH)
		wWidth = GDISP_SCREEN_WIDTH;
	if (wHeight > GDISP_SCREEN_HEIGHT)
		wHeight = GDISP_SCREEN_HEIGHT;

	/* Initialise the window */
	CreateThread(0, 0, WindowThread, 0, 0, 0);
	while (!isReady)
		Sleep(1);

	/* Initialise the GDISP structure to match */
	GDISP.Orientation = GDISP_ROTATE_0;
	GDISP.Powermode = powerOn;
	GDISP.Backlight = 100;
	GDISP.Contrast = 50;
	GDISP.Width = wWidth;
	GDISP.Height = wHeight;
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
	#if WIN32_USE_MSG_REDRAW
		RECT	rect;
	#endif
	#if GDISP_NEED_CONTROL
		coord_t	t;
	#endif

	#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
		// Clip pre orientation change
		if (x < GDISP.clipx0 || y < GDISP.clipy0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
	#endif

	#if GDISP_NEED_CONTROL
		switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			break;
		case GDISP_ROTATE_90:
			t = GDISP.Height - 1 - y;
			y = x;
			x = t;
			break;
		case GDISP_ROTATE_180:
			x = GDISP.Width - 1 - x;
			y = GDISP.Height - 1 - y;
			break;
		case GDISP_ROTATE_270:
			t = GDISP.Width - 1 - x;
			x = y;
			y = t;
			break;
		}
	#endif
	
	// Draw the pixel in the buffer
	color = COLOR2BGR(color);
	SetPixel(dcBuffer, x, y, color);
	
	#if WIN32_USE_MSG_REDRAW
		rect.left = x; rect.right = x+1;
		rect.top = y; rect.bottom = y+1;
		InvalidateRect(winRootWindow, &rect, FALSE);
		UpdateWindow(winRootWindow);
	#else
		// Draw the pixel again directly on the screen.
		// This is cheaper than invalidating a single pixel in the window
		dc = GetDC(winRootWindow);
		SetPixel(dc, x, y, color);
		ReleaseDC(winRootWindow, dc);
	#endif
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
		#if GDISP_NEED_CLIP
			HRGN	clip;
		#endif
		#if WIN32_USE_MSG_REDRAW
			RECT	rect;
		#endif
		#if GDISP_NEED_CONTROL
			coord_t	t;
		#endif

		#if GDISP_NEED_CLIP
			clip = NULL;
		#endif

		#if GDISP_NEED_CONTROL
			switch(GDISP.Orientation) {
			case GDISP_ROTATE_0:
				#if GDISP_NEED_CLIP
					// Clip post orientation change
					if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
						clip = CreateRectRgn(GDISP.clipx0, GDISP.clipy0, GDISP.clipx1, GDISP.clipy1);
				#endif
				break;
			case GDISP_ROTATE_90:
				t = GDISP.Height - 1 - y0;
				y0 = x0;
				x0 = t;
				t = GDISP.Height - 1 - y1;
				y1 = x1;
				x1 = t;
				#if GDISP_NEED_CLIP
					// Clip post orientation change
					if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
						clip = CreateRectRgn(GDISP.Height-1-GDISP.clipy1, GDISP.clipx0, GDISP.Height-1-GDISP.clipy0, GDISP.clipx1);
				#endif
				break;
			case GDISP_ROTATE_180:
				x0 = GDISP.Width - 1 - x0;
				y0 = GDISP.Height - 1 - y0;
				x1 = GDISP.Width - 1 - x1;
				y1 = GDISP.Height - 1 - y1;
				#if GDISP_NEED_CLIP
					// Clip post orientation change
					if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
						clip = CreateRectRgn(GDISP.Width-1-GDISP.clipx1, GDISP.Height-1-GDISP.clipy1, GDISP.Width-1-GDISP.clipx0, GDISP.Height-1-GDISP.clipy0);
				#endif
				break;
			case GDISP_ROTATE_270:
				t = GDISP.Width - 1 - x0;
				x0 = y0;
				y0 = t;
				t = GDISP.Width - 1 - x1;
				x1 = y1;
				y1 = t;
				#if GDISP_NEED_CLIP
					// Clip post orientation change
					if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
						clip = CreateRectRgn(GDISP.clipy0, GDISP.Width-1-GDISP.clipx1, GDISP.clipy1, GDISP.Width-1-GDISP.clipx0);
				#endif
				break;
			}
		#else
			#if GDISP_NEED_CLIP
				clip = NULL;
				if (GDISP.clipx0 != 0 || GDISP.clipy0 != 0 || GDISP.clipx1 != GDISP.Width || GDISP.clipy1 != GDISP.Height)
					clip = CreateRectRgn(GDISP.clipx0, GDISP.clipy0, GDISP.clipx1, GDISP.clipy1);
			#endif
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

			#if WIN32_USE_MSG_REDRAW
				rect.left = x0; rect.right = x1+1;
				rect.top = y0; rect.bottom = y1+1;
				InvalidateRect(winRootWindow, &rect, FALSE);
				UpdateWindow(winRootWindow);
			#else
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
			#endif

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
			// Clip pre orientation change
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		#if GDISP_NEED_CONTROL
			switch(GDISP.Orientation) {
			case GDISP_ROTATE_0:
				rect.top = y;
				rect.bottom = rect.top+cy;
				rect.left = x;
				rect.right = rect.left+cx;
				break;
			case GDISP_ROTATE_90:
				rect.top = x;
				rect.bottom = rect.top+cx;
				rect.right = GDISP.Height - y;
				rect.left = rect.right-cy;
				break;
			case GDISP_ROTATE_180:
				rect.bottom = GDISP.Height - y;
				rect.top = rect.bottom-cy;
				rect.right = GDISP.Width - x;
				rect.left = rect.right-cx;
				break;
			case GDISP_ROTATE_270:
				rect.bottom = GDISP.Width - x;
				rect.top = rect.bottom-cx;
				rect.left = y;
				rect.right = rect.left+cy;
				break;
			}
		#else
			rect.top = y;
			rect.bottom = rect.top+cy;
			rect.left = x;
			rect.right = rect.left+cx;
		#endif

		color = COLOR2BGR(color);
		hbr = CreateSolidBrush(color);

		if (hbr) {
			// Fill the area
			FillRect(dcBuffer, &rect, hbr);

			#if WIN32_USE_MSG_REDRAW
				InvalidateRect(winRootWindow, &rect, FALSE);
				UpdateWindow(winRootWindow);
			#else
				// Filling the area directly on the screen is likely to be cheaper than invalidating it
				dc = GetDC(winRootWindow);
				FillRect(dc, &rect, hbr);
				ReleaseDC(winRootWindow, dc);
			#endif

			DeleteObject(hbr);
		}
	}
#endif

#if GDISP_HARDWARE_BITFILLS || defined(__DOXYGEN__)
	static pixel_t *rotateimg(coord_t cx, coord_t cy, coord_t srcx, coord_t srccx, const pixel_t *buffer) {
		pixel_t	*dstbuf;
		pixel_t	*dst;
		const pixel_t	*src;
		size_t	sz;
		coord_t	i, j;

		// Shortcut.
		if (GDISP.Orientation == GDISP_ROTATE_0 && srcx == 0 && cx == srccx)
			return (pixel_t *)buffer;
		
		// Allocate the destination buffer
		sz = (size_t)cx * (size_t)cy;
		if (!(dstbuf = (pixel_t *)malloc(sz * sizeof(pixel_t))))
			return 0;
		
		// Copy the bits we need
		switch(GDISP.Orientation) {
		case GDISP_ROTATE_0:
			for(dst = dstbuf, src = buffer+srcx, j = 0; j < cy; j++)
				for(i = 0; i < cx; i++, src += srccx - cx)
					*dst++ = *src++;
			break;
		case GDISP_ROTATE_90:
			for(src = buffer+srcx, j = 0; j < cy; j++) {
				dst = dstbuf+cy-j-1;
				for(i = 0; i < cx; i++, src += srccx - cx, dst += cy)
					*dst = *src++;
			}
			break;
		case GDISP_ROTATE_180:
			for(dst = dstbuf+sz, src = buffer+srcx, j = 0; j < cy; j++)
				for(i = 0; i < cx; i++, src += srccx - cx)
					*--dst = *src++;
			break;
		case GDISP_ROTATE_270:
			for(src = buffer+srcx, j = 0; j < cy; j++) {
				dst = dstbuf+sz-cy+j;
				for(i = 0; i < cx; i++, src += srccx - cx, dst -= cy)
					*dst = *src++;
			}
			break;
		}
		return dstbuf;
	}
	
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
		#if GDISP_NEED_CONTROL
			pixel_t	*srcimg;
		#endif

		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			// Clip pre orientation change
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; srcx += GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; srcy += GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (srcx+cx > srccx)		cx = srccx - srcx;
			if (cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif

		// Make everything relative to the start of the line
		buffer += srccx*srcy;
		srcy = 0;
		
		memset(&bmpInfo, 0, sizeof(bmpInfo));
		bmpInfo.bV4Size = sizeof(bmpInfo);
		bmpInfo.bV4Planes = 1;
		bmpInfo.bV4BitCount = 32;
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

		#if GDISP_NEED_CONTROL
			bmpInfo.bV4SizeImage = (cy*cx) * sizeof(pixel_t);
			srcimg = rotateimg(cx, cy, srcx, srccx, buffer);
			if (!srcimg) return;
			
			switch(GDISP.Orientation) {
			case GDISP_ROTATE_0:
				bmpInfo.bV4Width = cx;
				bmpInfo.bV4Height = -cy; /* top-down image */
				rect.top = y;
				rect.bottom = rect.top+cy;
				rect.left = x;
				rect.right = rect.left+cx;
				break;
			case GDISP_ROTATE_90:
				bmpInfo.bV4Width = cy;
				bmpInfo.bV4Height = -cx; /* top-down image */
				rect.top = x;
				rect.bottom = rect.top+cx;
				rect.right = GDISP.Height - y;
				rect.left = rect.right-cy;
				break;
			case GDISP_ROTATE_180:
				bmpInfo.bV4Width = cx;
				bmpInfo.bV4Height = -cy; /* top-down image */
				rect.bottom = GDISP.Height - y;
				rect.top = rect.bottom-cy;
				rect.right = GDISP.Width - x;
				rect.left = rect.right-cx;
				break;
			case GDISP_ROTATE_270:
				bmpInfo.bV4Width = cy;
				bmpInfo.bV4Height = -cx; /* top-down image */
				rect.bottom = GDISP.Width - x;
				rect.top = rect.bottom-cx;
				rect.left = y;
				rect.right = rect.left+cy;
				break;
			}
			SetDIBitsToDevice(dcBuffer, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, 0, 0, 0, rect.bottom-rect.top, srcimg, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);
			if (srcimg != (pixel_t *)buffer)
				free(srcimg);
			
		#else
			bmpInfo.bV4Width = srccx;
			bmpInfo.bV4Height = -cy; /* top-down image */
			bmpInfo.bV4SizeImage = (cy*srccx) * sizeof(pixel_t);
			rect.top = y;
			rect.bottom = rect.top+cy;
			rect.left = x;
			rect.right = rect.left+cx;
			SetDIBitsToDevice(dcBuffer, x, y, cx, cy, srcx, 0, 0, cy, buffer, (BITMAPINFO*)&bmpInfo, DIB_RGB_COLORS);
		#endif

		// Invalidate the region to get it on the screen.
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
			// Clip pre orientation change
			if (x < 0 || x >= GDISP.Width || y < 0 || y >= GDISP.Height) return 0;
		#endif

		#if GDISP_NEED_CONTROL
			switch(GDISP.Orientation) {
			case GDISP_ROTATE_90:
				t = GDISP.Height - 1 - y;
				y = x;
				x = t;
				break;
			case GDISP_ROTATE_180:
				x = GDISP.Width - 1 - x;
				y = GDISP.Height - 1 - y;
				break;
			case GDISP_ROTATE_270:
				t = GDISP.Width - 1 - x;
				x = y;
				y = t;
				break;
			}
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
		RECT	rect, frect, srect;
		HBRUSH	hbr;
		
		#if GDISP_NEED_VALIDATION || GDISP_NEED_CLIP
			// Clip pre orientation change
			if (x < GDISP.clipx0) { cx -= GDISP.clipx0 - x; x = GDISP.clipx0; }
			if (y < GDISP.clipy0) { cy -= GDISP.clipy0 - y; y = GDISP.clipy0; }
			if (!lines || cx <= 0 || cy <= 0 || x >= GDISP.clipx1 || y >= GDISP.clipy1) return;
			if (x+cx > GDISP.clipx1)	cx = GDISP.clipx1 - x;
			if (y+cy > GDISP.clipy1)	cy = GDISP.clipy1 - y;
		#endif
		
		if (lines > cy) lines = cy;
		else if (-lines > cy) lines = -cy;

		bgcolor = COLOR2BGR(bgcolor);
		hbr = CreateSolidBrush(bgcolor);

		#if GDISP_NEED_CONTROL
			switch(GDISP.Orientation) {
			case GDISP_ROTATE_0:
				rect.top = y;
				rect.bottom = rect.top+cy;
				rect.left = x;
				rect.right = rect.left+cx;
				lines = -lines;
				goto vertical_scroll;
			case GDISP_ROTATE_90:
				rect.top = x;
				rect.bottom = rect.top+cx;
				rect.right = GDISP.Height - y;
				rect.left = rect.right-cy;
				goto horizontal_scroll;
			case GDISP_ROTATE_180:
				rect.bottom = GDISP.Height - y;
				rect.top = rect.bottom-cy;
				rect.right = GDISP.Width - x;
				rect.left = rect.right-cx;
			vertical_scroll:
				srect.left = frect.left = rect.left;
				srect.right = frect.right = rect.right;
				if (lines > 0) {
					srect.top = frect.top = rect.top;
					frect.bottom = rect.top+lines;
					srect.bottom = rect.bottom-lines;
				} else {
					srect.bottom = frect.bottom = rect.bottom;
					frect.top = rect.bottom+lines;
					srect.top = rect.top-lines;
				}
				if (cy >= lines && cy >= -lines)
					ScrollDC(dcBuffer, 0, lines, &srect, 0, 0, 0);
				break;
			case GDISP_ROTATE_270:
				rect.bottom = GDISP.Width - x;
				rect.top = rect.bottom-cx;
				rect.left = y;
				rect.right = rect.left+cy;
				lines = -lines;
			horizontal_scroll:
				srect.top = frect.top = rect.top;
				srect.bottom = frect.bottom = rect.bottom;
				if (lines > 0) {
					srect.left = frect.left = rect.left;
					frect.right = rect.left+lines;
					srect.right = rect.right-lines;
				} else {
					srect.right = frect.right = rect.right;
					frect.left = rect.right+lines;
					srect.left = rect.left-lines;
				}
				if (cy >= lines && cy >= -lines)
					ScrollDC(dcBuffer, lines, 0, &srect, 0, 0, 0);
				break;
			}
		#else
			rect.top = y;
			rect.bottom = rect.top+cy;
			rect.left = x;
			rect.right = rect.left+cx;
			lines = -lines;
			srect.left = frect.left = rect.left;
			srect.right = frect.right = rect.right;
			if (lines > 0) {
				srect.top = frect.top = rect.top;
				frect.bottom = rect.top+lines;
				srect.bottom = rect.bottom-lines;
			} else {
				srect.bottom = frect.bottom = rect.bottom;
				frect.top = rect.bottom+lines;
				srect.top = rect.top-lines;
			}
			if (cy >= lines && cy >= -lines)
				ScrollDC(dcBuffer, 0, lines, &srect, 0, 0, 0);
		#endif
		
		if (hbr)
			FillRect(dcBuffer, &frect, hbr);
		InvalidateRect(winRootWindow, &rect, FALSE);
		UpdateWindow(winRootWindow);
	}
#endif

#if (GDISP_NEED_CONTROL && GDISP_HARDWARE_CONTROL) || defined(__DOXYGEN__)
	/**
	 * @brief   Driver Control
	 * @detail	Unsupported control codes are ignored.
	 * @note	The value parameter should always be typecast to (void *).
	 * @note	There are some predefined and some specific to the low level driver.
	 * @note	GDISP_CONTROL_POWER			- Takes a gdisp_powermode_t
	 * 			GDISP_CONTROL_ORIENTATION	- Takes a gdisp_orientation_t
	 * 			GDISP_CONTROL_BACKLIGHT -	 Takes an int from 0 to 100. For a driver
	 * 											that only supports off/on anything other
	 * 											than zero is on.
	 * 			GDISP_CONTROL_CONTRAST		- Takes an int from 0 to 100.
	 * 			GDISP_CONTROL_LLD			- Low level driver control constants start at
	 * 											this value.
	 *
	 * @param[in] what		What to do.
	 * @param[in] value		The value to use (always cast to a void *).
	 *
	 * @notapi
	 */
	void GDISP_LLD(control)(unsigned what, void *value) {
		switch(what) {
		case GDISP_CONTROL_ORIENTATION:
			if (GDISP.Orientation == (gdisp_orientation_t)value)
				return;
			switch((gdisp_orientation_t)value) {
				case GDISP_ROTATE_0:
					GDISP.Width = wWidth;
					GDISP.Height = wHeight;
					break;
				case GDISP_ROTATE_90:
					GDISP.Height = wWidth;
					GDISP.Width = wHeight;
					break;
				case GDISP_ROTATE_180:
					GDISP.Width = wWidth;
					GDISP.Height = wHeight;
					break;
				case GDISP_ROTATE_270:
					GDISP.Height = wWidth;
					GDISP.Width = wHeight;
					break;
				default:
					return;
			}

			#if GDISP_NEED_CLIP || GDISP_NEED_VALIDATION
				GDISP.clipx0 = 0;
				GDISP.clipy0 = 0;
				GDISP.clipx1 = GDISP.Width;
				GDISP.clipy1 = GDISP.Height;
			#endif
			GDISP.Orientation = (gdisp_orientation_t)value;
			return;
/*
		case GDISP_CONTROL_POWER:
		case GDISP_CONTROL_BACKLIGHT:
		case GDISP_CONTROL_CONTRAST:
*/
		}
	}
#endif

#if GINPUT_NEED_MOUSE

	void ginput_lld_mouse_init(void) {}

	void ginput_lld_mouse_get_reading(MouseReading *pt) {
		pt->x = mousex;
		pt->y = mousey > wHeight ? wHeight : mousey;
		pt->z = (mousebuttons & GINPUT_MOUSE_BTN_LEFT) ? 100 : 0;
		pt->buttons = mousebuttons;
	}

#endif /* GINPUT_NEED_MOUSE */

#if GINPUT_NEED_TOGGLE

	void ginput_lld_toggle_init(const GToggleConfig *ptc) { (void) ptc; }
	unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc) { (void) ptc; return toggles; }

#endif /* GINPUT_NEED_MOUSE */

#endif /* GFX_USE_GDISP */
/** @} */

