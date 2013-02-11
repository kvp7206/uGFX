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
 * @file    include/gmisc/gmisc.h
 * @brief   GMISC - Miscellaneous Routines header file.
 *
 * @addtogroup GAUDIN
 *
 * @{
 */

#ifndef _GMISC_H
#define _GMISC_H

#include "gfx.h"

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief Sample data formats
 */
typedef enum ArrayDataFormat_e {
	ARRAY_DATA_4BITUNSIGNED = 4,	ARRAY_DATA_4BITSIGNED = 5,
	ARRAY_DATA_8BITUNSIGNED = 8,	ARRAY_DATA_8BITSIGNED = 9,
	ARRAY_DATA_10BITUNSIGNED = 10,	ARRAY_DATA_10BITSIGNED = 11,
	ARRAY_DATA_12BITUNSIGNED = 12,	ARRAY_DATA_12BITSIGNED = 13,
	ARRAY_DATA_14BITUNSIGNED = 14,	ARRAY_DATA_14BITSIGNED = 15,
	ARRAY_DATA_16BITUNSIGNED = 16,	ARRAY_DATA_16BITSIGNED = 17,
	} ArrayDataFormat;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#if GFX_USE_GMISC || defined(__DOXYGEN__)

#ifdef __cplusplus
extern "C" {
#endif

#if GMISC_NEED_ARRAYOPS || defined(__DOXYGEN__)
	/**
	 * @brief				Convert from one array format to another array format.
	 *
	 * @param[in] srcfmt		The format of the source array
	 * @param[in] src			The source array
	 * @param[in] dstfmt		The format of the destination array
	 * @param[in] dst			The dstination array
	 * @param[in] cnt			The number of array elements to convert
	 *
	 * @note				Assumes the destination buffer is large enough for the resultant data.
	 * @note				This routine is optimised to perform as fast as possible.
	 * @note				No type checking is performed on the source format. It is assumed to
	 * 						have only valid values eg. ARRAY_DATA_4BITSIGNED will have values
	 * 							0000 -> 0111 for positive numbers and 1111 -> 1000 for negative numbers
	 * 							Bits 5 -> 8 in the storage byte are treated in an undefined manner.
	 * @note				If srcfmt or dstfmt is an unknown format, this routine does nothing
	 * 						with no warning that something is wrong
	 *
	 * @api
	 */
	void gmiscArrayConvert(ArrayDataFormat srcfmt, void *src, ArrayDataFormat dstfmt, void *dst, size_t cnt);

	#if 0
		void gmiscArrayTranslate(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int trans);

		void gmiscArrayMultiply(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mult);

		void gmiscArrayDivide(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mdiv);

		void gmiscArrayMultDiv(ArrayDataFormat fmt, void *src, void *dst, size_t cnt, int mult, int div);

		void gmiscArrayAdd(ArrayDataFormat fmt, void *src1, void *src2, void *dst, size_t cnt);

		void gmiscArrayAddNoOverflow(ArrayDataFormat fmt, void *src1, void *src2, void *dst, size_t cnt);
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_MISC */

#endif /* _GMISC_H */
/** @} */

