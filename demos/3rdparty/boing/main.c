/* Derived from the 2011 IOCCC submission by peter.eastman@gmail.com
 * http://www.ioccc.org/2011/eastman/eastman.c
 * --
 * Public Domain -- but you're looking at this for ideas of techniques
 * and methods, not trying to cut&paste an entire application, anyway.
 * --
 * When you need to blit an entire screenfull of data to an LCD
 * display, the basic idea is to exploit the auto-increment feature of
 * the display controller when it writes to screen memory. You start
 * by resetting the 'cursor' to the 0,0 position, and then stream
 * width*height pixels out.
 * --
 * Chris Baird,, <cjb@brushtail.apana.org.au> April 2013
 *
 * Modified Andrew Hannam (inmarket) 2013-04-29 New GFX support
 */

#include <math.h>
#include "gfx.h"
#include "ssd2119.h"

#define Lightgrey (HTML2COLOR(0xC0C0C0))
#define Midgrey (HTML2COLOR(0x606060))
#define Darkgrey (HTML2COLOR(0x303030))


/* ---------------------------------------------------------------------- */
/* As of early April 2013, the /gfx extension tries to keep the low-level
 * stuff away from our filthy paws. So Code Duplication.
 * (Possibly to be replaced with gdispStartStream(), gdispWriteStream()
 * and gdispStopStream() in the future.)
 */

#define GDISP_REG (*((volatile uint16_t *) 0x60000000)) /* DC = 0 */
#define GDISP_RAM (*((volatile uint16_t *) 0x60100000)) /* DC = 1 */

inline void write_index (uint16_t index) { GDISP_REG = index; }
inline void write_data (uint16_t data) { GDISP_RAM = data; }

#define write_reg(reg, data) { write_index(reg); write_data(data); }

void reset_cursor (void)
{
  write_reg (SSD2119_REG_X_RAM_ADDR, 0);
  write_reg (SSD2119_REG_Y_RAM_ADDR, 0);
}

#define StartStream() { write_index (SSD2119_REG_RAM_DATA); }
#define WriteStream(x) { write_data (x); }
#define StopStream() /* NOP */


/* ---------------------------------------------------------------------- */

void main (void)
{
  uint16_t xx, yy, colour;

  gfxInit();

  uint16_t width = (uint16_t)gdispGetWidth();
  uint16_t height = (uint16_t)gdispGetHeight();

  float i=height/5+height%2+1, floorstart=height/5-1, spherespin=0.0,
    l=width/2, m=height/4, n=.01*width, o=0.0, rotspeed=0.1, h, f, g;

  while (TRUE)
    {
      reset_cursor ();
      StartStream ();

      for (xx=yy=0;
	   h = (m-yy)/i, f=-.3*(g=(l-xx)/i)+.954*h, yy<height;
	   yy += (xx = ++xx%width)==0 )
	{
	  if (g*g < 1-h*h)	/* if inside the ball */
	    if (((int)(9-spherespin+(.954*g+.3*h)/sqrtf(1-f*f))+(int)(2+f*2))%2==0)
	      colour = Red;
	    else
	      colour = White;
	  else
	    {
	      if (xx<floorstart || xx>width-floorstart)
		colour = Darkgrey; /* side wall */
	      else
		colour = Lightgrey; /* back wall */

	      if (yy > height-floorstart)
		if (xx < height-yy || height-yy > width-xx) /* floor */
		  colour = Darkgrey;
		else
		  colour = Midgrey;

	      if (g*(g+.6)+.09+h*h < 1)
		colour >>= 1;	/* ball shadow; make it darker  */
	    }

	  WriteStream (colour);	/* pixel to the LCD */
	}

      StopStream();
      spherespin += rotspeed;
      m += o;
      o = m > height-1.75*floorstart ? -.04*height : o+.002*height;
      n = (l+=n)<i || l>width-i ? rotspeed=-rotspeed,-n : n;
    }
}

/* ---------------------------------------------------------------------- */
