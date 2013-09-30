/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    drivers/gdisp/T6963C/gdisp_lld_board_ThunderCryer.h
 * @brief   GDISP Graphic Driver subsystem board interface for the T6963C display.
 *
 * @addtogroup GDISP
 * @{
 */

#ifndef GDISP_LLD_BOARD_H
#define GDISP_LLD_BOARD_H

#include "gfx.h"
#include "ch.h"
#include "hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

// data port
#define GLCD_DATA_PORT          GPIOE
#define GLCD_DATA_PORT_MASK     0xff00
#define GLCD_DATA_OFFSET        8

#define GLCD_DATA_MODE_MASK_IN  0xffff0000
#define GLCD_DATA_MODE_MASK_OUT 0x55550000
#define GLCD_DATA_INPUT         GLCD_DATA_PORT->MODER &= ~GLCD_DATA_MODE_MASK_IN;
#define GLCD_DATA_OUTPUT        GLCD_DATA_PORT->MODER |= GLCD_DATA_MODE_MASK_OUT;

// control port
#define GLCD_CTRL_PORT          GPIOB

// control signals
#define GLCD_RD         (1<<GPIOB_RD)
#define GLCD_WR         (1<<GPIOB_WR)
#define GLCD_RESET      (1<<GPIOB_RST)
#define GLCD_CE         (1<<GPIOB_CE)
#define GLCD_CD         (1<<GPIOB_CD)


// aux pins
#define GLCD_CONTRAST_PORT  GPIOB
#define GLCD_CONTRAST_MASK  ((1<<GPIOB_CONTRAST_A0) | (1<<GPIOB_CONTRAST_A1) | \
                             (1<<GPIOB_CONTRAST_A2))
#define GLCD_LED            GPIOB_LED


/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static GPTConfig gptConf14 = {
    4e6,  //timer clock
    NULL   //timer callback
};

static PWMConfig pwmconf9 = {
   1e6,
   100,
   NULL,
   {
    {PWM_OUTPUT_ACTIVE_HIGH , NULL},
    {PWM_OUTPUT_DISABLED , NULL},
    {PWM_OUTPUT_DISABLED , NULL},
    {PWM_OUTPUT_DISABLED , NULL}
   },
   0
};

const unsigned long c_iDelayFore = 2;
const unsigned long c_iDelayAfter = 2;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



#endif /* GDISP_LLD_BOARD_H */
/** @} */

