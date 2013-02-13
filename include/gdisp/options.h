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
 * @file    include/gdisp/options.h
 * @brief   GDISP sub-system options header file.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef _GDISP_OPTIONS_H
#define _GDISP_OPTIONS_H

/**
 * @name    GDISP Functionality to be included
 * @{
 */
	/**
	 * @brief   Should all operations be clipped to the screen and colors validated.
	 * @details	Defaults to TRUE.
	 * @note    If this is FALSE, any operations that extend beyond the
	 *          edge of the screen will have undefined results. Any
	 *			out-of-range colors will produce undefined results.
	 * @note	If defined then all low level and high level GDISP driver routines
	 *			must check the validity of inputs and do something sensible
	 *			if they are out of range. It doesn't have to be efficient,
	 *			just valid.
	 */
	#ifndef GDISP_NEED_VALIDATION
		#define GDISP_NEED_VALIDATION	TRUE
	#endif
	/**
	 * @brief   Are clipping functions needed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_CLIP
		#define GDISP_NEED_CLIP			TRUE
	#endif
	/**
	 * @brief   Are text functions needed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_TEXT
		#define GDISP_NEED_TEXT			TRUE
	#endif
	/**
	 * @brief   Are circle functions needed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_CIRCLE
		#define GDISP_NEED_CIRCLE		TRUE
	#endif
	/**
	 * @brief   Are ellipse functions needed.
	 * @details	Defaults to TRUE
	 */
	#ifndef GDISP_NEED_ELLIPSE
		#define GDISP_NEED_ELLIPSE		TRUE
	#endif
	/**
	 * @brief   Are arc functions needed.
	 * @details	Defaults to FALSE
	 * @note	Requires the maths library to be included in the link. ie  -lm
	 */
	#ifndef GDISP_NEED_ARC
		#define GDISP_NEED_ARC			FALSE
	#endif
	/**
	 * @brief   Are scrolling functions needed.
	 * @details	Defaults to FALSE
	 * @note	This function must be supported by the low level GDISP driver
	 * 			you have included in your project. If it isn't, defining this
	 * 			option will cause a compile error.
	 */
	#ifndef GDISP_NEED_SCROLL
		#define GDISP_NEED_SCROLL		FALSE
	#endif
	/**
	 * @brief   Is the capability to read pixels back needed.
	 * @details	Defaults to FALSE
	 * @note	This function must be supported by the low level GDISP driver
	 * 			you have included in your project. If it isn't, defining this
	 * 			option will cause a compile error.
	 */
	#ifndef GDISP_NEED_PIXELREAD
		#define GDISP_NEED_PIXELREAD	FALSE
	#endif
	/**
	 * @brief   Control some aspect of the hardware operation.
	 * @details	Defaults to FALSE
	 * @note	This allows control of hardware specific features such as
	 * 			screen rotation, backlight levels, contrast etc
	 */
	#ifndef GDISP_NEED_CONTROL
		#define GDISP_NEED_CONTROL		FALSE
	#endif
	/**
	 * @brief   Is the messaging api interface required.
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_NEED_MSGAPI
		#define GDISP_NEED_MSGAPI		FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP Multi-Threading Options
 * @{
 */
	/**
	 * @brief   Do the drawing functions need to be thread-safe.
	 * @details	Defaults to FALSE
	 * @note	Both GDISP_NEED_MULTITHREAD and GDISP_NEED_ASYNC make
	 * 			the gdisp API thread-safe.
	 * @note	This is more efficient than GDISP_NEED_ASYNC as it only
	 * 			requires a context switch if something else is already
	 * 			drawing.
	 */
	#ifndef GDISP_NEED_MULTITHREAD
		#define GDISP_NEED_MULTITHREAD	FALSE
	#endif
	/**
	 * @brief   Use asynchronous calls (multi-thread safe).
	 * @details	Defaults to FALSE
	 * @note	Both GDISP_NEED_MULTITHREAD and GDISP_NEED_ASYNC make
	 * 			the gdisp API thread-safe.
	 * @note	Turning this on adds two context switches per transaction
	 *			so it can significantly slow graphics drawing but it allows
	 *			drawing operations to continue in the background.
	 */
	#ifndef GDISP_NEED_ASYNC
		#define GDISP_NEED_ASYNC		FALSE
	#endif
/**
 * @}
 *
 * @name    GDISP Fonts
 * @{
 */
	/**
	 * @brief   Include the old global font variable definitions
	 * @details	Defaults to FALSE
	 */
	#ifndef GDISP_OLD_FONT_DEFINITIONS
		#define GDISP_OLD_FONT_DEFINITIONS		FALSE
	#endif
	/**
	 * @brief   Predefined built in fonts
	 * @note	Turning off the ones you are not using can save program size.
	 */
	#ifndef GDISP_INCLUDE_FONT_SMALL
		#define GDISP_INCLUDE_FONT_SMALL		TRUE
	#endif
	#ifndef GDISP_INCLUDE_FONT_LARGER
		#define GDISP_INCLUDE_FONT_LARGER		TRUE
	#endif
	#ifndef GDISP_INCLUDE_FONT_UI1
		#define GDISP_INCLUDE_FONT_UI1			TRUE
	#endif
	#ifndef GDISP_INCLUDE_FONT_UI2
		#define GDISP_INCLUDE_FONT_UI2			TRUE
	#endif
	#ifndef GDISP_INCLUDE_FONT_LARGENUMBERS
		#define GDISP_INCLUDE_FONT_LARGENUMBERS	TRUE
	#endif

/**
 * @}
 *
 * @name    GDISP Optional Sizing Parameters
 * @{
 */
	/**
	 * @brief   The maximum height of a font.
	 * @details	Either 16 or 32. Defaults to 16
	 * @note	Setting this to 32 causes the font tables to take
	 *			twice the internal program memory. Don't do it unless
	 *			you realy must define an unscaled font larger than 16 bits high.
	 */
	#ifndef GDISP_MAX_FONT_HEIGHT
		#define GDISP_MAX_FONT_HEIGHT	16
	#endif
/**
 * @}
 *
 * @name    GDISP Optional Low Level Driver Defines
 * @{
 */
	/**
	 * @brief   Use a custom board definition even if a board definition exists.
	 * @details	Defaults to FALSE
	 * @details	If TRUE, add gdisp_lld_board.h to your project directory and customise it.
	 * @note	Not all GDISP low level drivers currently use board definition files.
	 */
	#ifndef GDISP_USE_CUSTOM_BOARD
		#define GDISP_USE_CUSTOM_BOARD		FALSE
	#endif
	/**
	 * @brief   Set the screen height and width.
	 * @note	Ignored by some low level GDISP drivers, optional for others.
	 * @note	Where these values are allowed, a default is always provided be the low level driver.
	 * @note	The list of GDISP low level drivers that allow these to be set are...
	 *				WIN32, SSD1289, SSD1963, TestStub
	 */
	/* #define GDISP_SCREEN_WIDTH		nnnn */
	/* #define GDISP_SCREEN_HEIGHT		nnnn */
	/**
	 * @brief   Define which bus interface to use.
	 * @details	Only required by the SSD1963 driver.
	 * @note	This will be replaced eventually by board definition files
	 */
	/* #define GDISP_USE_FSMC */
	/* #define GDISP_USE_GPIO */
/** @} */

#endif /* _GDISP_OPTIONS_H */
/** @} */

