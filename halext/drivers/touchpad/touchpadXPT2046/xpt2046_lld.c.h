/*
    ChibiOS-LCD-Driver - Copyright (C) 2012
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

#ifndef _XPT2046_H
#define _XPT2046_H

void tp_lld_init(TOUCHPADDriver *tp) {

}

uint16_t tp_lld_read_x(void) {
	return 42;
}

uint16_t tp_lld_read_y(void) {
	return 42;
}

uint16_t tp_lld_read_z(void) {
	return 42;
}

#endif /* _XPT2046_H */
