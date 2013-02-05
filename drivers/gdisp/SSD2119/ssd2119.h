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

#ifndef _SSD2119_H
#define _SSD2119_H

/* SSD2119 registers */

#define SSD2119_REG_DEVICE_CODE_READ  0x00
#define SSD2119_REG_OSC_START         0x00
#define SSD2119_REG_OUTPUT_CTRL       0x01
#define SSD2119_REG_LCD_DRIVE_AC_CTRL 0x02
#define SSD2119_REG_PWR_CTRL_1        0x03
#define SSD2119_REG_DISPLAY_CTRL      0x07
#define SSD2119_REG_FRAME_CYCLE_CTRL  0x0B
#define SSD2119_REG_PWR_CTRL_2        0x0C
#define SSD2119_REG_PWR_CTRL_3        0x0D
#define SSD2119_REG_PWR_CTRL_4        0x0E
#define SSD2119_REG_GATE_SCAN_START   0x0F
#define SSD2119_REG_SLEEP_MODE_1      0x10
#define SSD2119_REG_ENTRY_MODE        0x11
#define SSD2119_REG_SLEEP_MODE_2      0x12
#define SSD2119_REG_GEN_IF_CTRL       0x15
#define SSD2119_REG_H_PORCH           0x16
#define SSD2119_REG_V_PORCH           0x17
#define SSD2119_REG_PWR_CTRL_5        0x1E
#define SSD2119_REG_UNIFORMITY        0x20
#define SSD2119_REG_RAM_DATA          0x22
#define SSD2119_REG_FRAME_FREQ        0x25
#define SSD2119_REG_ANALOG_SET        0x26
#define SSD2119_REG_VCOM_OTP_1        0x28
#define SSD2119_REG_VCOM_OTP_2        0x29
#define SSD2119_REG_GAMMA_CTRL_1      0x30
#define SSD2119_REG_GAMMA_CTRL_2      0x31
#define SSD2119_REG_GAMMA_CTRL_3      0x32
#define SSD2119_REG_GAMMA_CTRL_4      0x33
#define SSD2119_REG_GAMMA_CTRL_5      0x34
#define SSD2119_REG_GAMMA_CTRL_6      0x35
#define SSD2119_REG_GAMMA_CTRL_7      0x36
#define SSD2119_REG_GAMMA_CTRL_8      0x37
#define SSD2119_REG_GAMMA_CTRL_9      0x3A
#define SSD2119_REG_GAMMA_CTRL_10     0x3B
#define SSD2119_REG_V_SCROLL_1        0x41
#define SSD2119_REG_V_SCROLL_2        0x42
#define SSD2119_REG_V_RAM_POS         0x44
#define SSD2119_REG_H_RAM_START       0x45
#define SSD2119_REG_H_RAM_END         0x46
#define SSD2119_REG_1_DRV_POS_1       0x48
#define SSD2119_REG_1_DRV_POS_2       0x49
#define SSD2119_REG_2_DRV_POS_1       0x4A
#define SSD2119_REG_2_DRV_POS_2       0x4B
#define SSD2119_REG_X_RAM_ADDR        0x4E
#define SSD2119_REG_Y_RAM_ADDR        0x4F

/* SSD2119 commands */

#endif // _SSD2119_H
