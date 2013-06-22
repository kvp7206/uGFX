/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use - one of these must be defined */
#define GFX_USE_OS_CHIBIOS		TRUE
#define GFX_USE_OS_WIN32		FALSE
#define GFX_USE_OS_POSIX		FALSE

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP                   TRUE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION           FALSE
#define GDISP_NEED_CLIP                 FALSE
#define GDISP_NEED_TEXT                 TRUE
#define GDISP_NEED_CIRCLE               FALSE
#define GDISP_NEED_ELLIPSE              FALSE
#define GDISP_NEED_ARC                  FALSE
#define GDISP_NEED_SCROLL               FALSE
#define GDISP_NEED_PIXELREAD            FALSE
#define GDISP_NEED_CONTROL              TRUE
#define GDISP_NEED_MULTITHREAD          FALSE
#define GDISP_NEED_ASYNC                FALSE
#define GDISP_NEED_MSGAPI               FALSE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_SMALL        FALSE
#define GDISP_INCLUDE_FONT_LARGER       FALSE
#define GDISP_INCLUDE_FONT_UI1          FALSE
#define GDISP_INCLUDE_FONT_UI2          TRUE
#define GDISP_INCLUDE_FONT_LARGENUMBERS FALSE

#endif /* _GFXCONF_H */

