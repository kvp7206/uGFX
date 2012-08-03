/*
    ChibiOS/RT - Copyright (C) 2012
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

//achtung wird im RAM abgelegt!
float sintable[91]  = {
		0
		,0.017452406
		,0.034899497
		,0.052335956
		,0.069756474
		,0.087155743
		,0.104528463
		,0.121869343
		,0.139173101
		,0.156434465
		,0.173648178
		,0.190808995
		,0.207911691
		,0.224951054
		,0.241921896
		,0.258819045
		,0.275637356
		,0.292371705
		,0.309016994
		,0.325568154
		,0.342020143
		,0.35836795
		,0.374606593
		,0.390731128
		,0.406736643
		,0.422618262
		,0.438371147
		,0.4539905
		,0.469471563
		,0.48480962
		,0.5
		,0.515038075
		,0.529919264
		,0.544639035
		,0.559192903
		,0.573576436
		,0.587785252
		,0.601815023
		,0.615661475
		,0.629320391
		,0.64278761
		,0.656059029
		,0.669130606
		,0.68199836
		,0.69465837
		,0.707106781
		,0.7193398
		,0.731353702
		,0.743144825
		,0.75470958
		,0.766044443
		,0.777145961
		,0.788010754
		,0.79863551
		,0.809016994
		,0.819152044
		,0.829037573
		,0.838670568
		,0.848048096
		,0.857167301
		,0.866025404
		,0.874619707
		,0.882947593
		,0.891006524
		,0.898794046
		,0.906307787
		,0.913545458
		,0.920504853
		,0.927183855
		,0.933580426
		,0.939692621
		,0.945518576
		,0.951056516
		,0.956304756
		,0.961261696
		,0.965925826
		,0.970295726
		,0.974370065
		,0.978147601
		,0.981627183
		,0.984807753
		,0.987688341
		,0.990268069
		,0.992546152
		,0.994521895
		,0.996194698
		,0.99756405
		,0.998629535
		,0.999390827
		,0.999847695
		,1

};

float getSin(unsigned int degree) {
	degree = degree % 360;

	if(degree <= 90) {
		return sintable[degree];
	}
	else if(degree <= 180) {
		return sintable[180-degree];
	}
	else if(degree <= 270) {
		return sintable[degree-180]*(-1.0);
	}
	else {
		return sintable[360-degree]*(-1.0);
	}
}

double getCos(unsigned int degree) {
	degree = degree % 360;

	return getSin(degree+90);
}

/* signum function */
char sgn(char x) {
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

unsigned char max(unsigned char a, unsigned char b) {
	return (a<b) ? b : a;
}

unsigned char min (unsigned char a, unsigned char b) {
	return (a<b) ? a : b;
}

