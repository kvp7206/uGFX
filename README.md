# Features

- Uses GPIO - no FSMC interface needed - portable to any controller

- All four modes are Supported:
	- Portrait
	- Landscape
	- Portrait inverted
	- Landscape inverted

- Supported drawing functions:

	- Pixels
	- Lines
	- Rectanges (frames/filled)
	- Circles (frames/filled)
	- character
	- string

 
 
 
 

# Install

## checkout Driver code into ext/
cd chibios/ext
git clone https://github.com/tectu/Chibios-LCD-Driver lcd

## Edit Makefile:
include lcd.mk:
	include $(CHIBIOS)/ext/lcd/lcd.mk

Add $(LCDSRC) to CSRC:
	CSRC = $(PORTSRC) \
	       $(KERNSRC) \
	       $(TESTSRC) \
	       $(HALSRC) \
	       $(PLATFORMSRC) \
	       $(BOARDSRC) \
	       $(FATFSSRC) \
	       $(LCDSRC) \
	       $(CHIBIOS)/os/various/evtimer.c \
	       $(CHIBIOS)/os/various/syscalls.c

Add $(LCDINC) to INCDIR:
	INCDIR = $(PORTINC) $(KERNINC) $(TESTINC) \
	         $(HALINC) $(PLATFORMINC) $(BOARDINC) \
	         $(FATFSINC) \
	         $(LCDINC) \
	         $(CHIBIOS)/os/various ../common

## Use
include glcd.h header file wherever you need it.

