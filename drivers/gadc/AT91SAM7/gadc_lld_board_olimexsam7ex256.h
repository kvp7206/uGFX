/* 
 * This source code form is a part of the ChibiOS/GFX project and stands
 * under the terms of the GFX License v1.0. If a copy of the license
 * was not distributed with this file, You can obtain one at: 
 * 
 * http://chibios-gfx.com/license.html
 *
 */

/**
 * @file    drivers/gadc/AT91SAM7/gadc_lld_board_olimexsam7ex256.h
 * @brief   GADC Driver config file.
 *
 * @addtogroup GADC
 * @{
 */

#ifndef _GADC_LLD_BOARD_OLIMEXSAM7EX256_H
#define _GADC_LLD_BOARD_OLIMEXSAM7EX256_H

#if GFX_USE_GADC

/*===========================================================================*/
/* Analogue devices on this board                                            */
/*===========================================================================*/

#define GADC_PHYSDEV_MICROPHONE			0x00000080
#define GADC_PHYSDEV_DIAL				0x00000040
#define GADC_PHYSDEV_TEMPERATURE		0x00000020

#endif	/* GFX_USE_GADC */

#endif	/* _GADC_LLD_BOARD_OLIMEXSAM7EX256_H */
/** @} */

