#include "glcd.h" 
#include "fonts.h"
#include <math.h>

static uint8_t orientation;
static uint16_t DeviceCode, temp;

static __inline void lcdWriteIndex(uint16_t index) {
	Clr_RS;
	Set_RD;
  
	LCD_DATA_PORT->ODR = index;

	Clr_WR;
	Set_WR;
}

static __inline void lcdWriteData(uint16_t data) {
	Set_RS;

	LCD_DATA_PORT->ODR = data;

	Clr_WR;
	Set_WR;
}

static __inline uint16_t lcdReadData(void) { 
	uint16_t value;

	Set_RS;
	Set_WR;
	Clr_RD;

  	// change pin mode to digital input
	LCD_DATA_PORT->CRH = 0x44444444;
	LCD_DATA_PORT->CRL = 0x44444444;
	
	value = LCD_DATA_PORT->IDR; // dummy
	value = LCD_DATA_PORT->IDR;

	// change pin mode back to digital output
	LCD_DATA_PORT->CRH = 0x33333333;
	LCD_DATA_PORT->CRL = 0x33333333;

	Set_RD;

	return value;
}

static __inline void lcdWriteReg(uint16_t lcdReg,uint16_t lcdRegValue) { 
	Clr_CS;
	lcdWriteIndex(lcdReg);         
	lcdWriteData(lcdRegValue);  
	Set_CS; 
}

uint16_t lcdReadReg(uint16_t lcdReg) {
	uint16_t lcdRAM;

	Clr_CS;
	lcdWriteIndex(lcdReg);
	lcdRAM = lcdReadData();
	      	
	Set_CS;

	return lcdRAM;
}

static void lcdSetCursor(uint16_t x, uint16_t y) {
	if(DeviceCode==0x8989) {
		if(orientation == portrait) {
			lcdWriteReg(0x004e, x);
	    	lcdWriteReg(0x004f, y);
		} else if(orientation == landscape) {
			lcdWriteReg(0x004e, y);
			lcdWriteReg(0x004f, x);
		}
	}
	else if(DeviceCode==0x9919) {
		if(orientation == portrait) {
			lcdWriteReg(0x004e, x);
			lcdWriteReg(0x004f, y);
		} else if(orientation == landscape) {
			lcdWriteReg(0x004e, y);
			lcdWriteReg(0x004f, x);
		}
	} else {
		lcdWriteReg(0x0020, x);
		lcdWriteReg(0x0021, y); 
	}
}

static void lcdDelay(uint16_t nCount) {
	uint16_t TimingDelay; 
	
	while(nCount--) {
		for(TimingDelay=0;TimingDelay<10000;TimingDelay++)
			asm("nop");
	}
}

void lcdSetOrientation(uint8_t newOrientation) {
	orientation = newOrientation;

	switch(orientation) {
		case portrait:
			lcdWriteReg(0x0001, 0x2B3F); 
			lcdWriteReg(0x0011, 0x6070);
			break;
		case landscape:
			lcdWriteReg(0x0001, 0x293F);
			lcdWriteReg(0x0011, 0x6078);
			break;
		case 2:
			break;
		case 3:
			break;
	}
}

void lcdSetWindows(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	if(orientation == portrait) {
		lcdWriteReg(0x0050, x);                    /* Horizontal GRAM Start Address      */
		lcdWriteReg(0x0051, x+width-1);                /* Horizontal GRAM End   Address (-1) */
		lcdWriteReg(0x0052, y);                    /* Vertical   GRAM Start Address      */
		lcdWriteReg(0x0053, y+height-1);                /* Vertical   GRAM End   Address (-1) */
		lcdWriteReg(0x0020, x);
		lcdWriteReg(0x0021, y);
	} else if(orientation == landscape) {
		lcdWriteReg(0x0050, y);                    /* Vertical   GRAM Start Address      */
		lcdWriteReg(0x0051, y+height-1);                /* Vertical   GRAM End   Address (-1) */
		lcdWriteReg(0x0052, x);                    /* Horizontal GRAM Start Address      */
		lcdWriteReg(0x0053, x+width-1);                /* Horizontal GRAM End   Address (-1) */
		lcdWriteReg(0x0020, y);
		lcdWriteReg(0x0021, x);
	}
}

void lcdClear(uint16_t color) {
	uint32_t index=0;

	lcdSetCursor(0,0); 
	Clr_CS; 
	lcdWriteIndex(0x0022);
	for(index=0;index<76800;index++)
		lcdWriteData(color);
	Set_CS;
}

uint16_t lcdGetPoint(uint16_t x,uint16_t y) {
	uint16_t dummy;

	lcdSetCursor(x,y);
	Clr_CS;
	lcdWriteIndex(0x0022);  
	dummy = lcdReadData();
	dummy = lcdReadData(); 	
	Set_CS;

	if( DeviceCode==0x7783 || DeviceCode==0x4531 || DeviceCode==0x8989 )
		return dummy;
	else
		return lcdBGR2RGB(dummy);
}

void lcdDrawPixel(uint16_t x,uint16_t y,uint16_t point) {
	lcdSetCursor(x,y);
	lcdWriteReg(0x0022,point);
}

void lcdDrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
   int16_t dy, dx;
   int8_t  addx=1, addy=1;
   int16_t P, diff;

   int16_t i=0;
   dx = abs((int8_t)(x2 - x1));
   dy = abs((int16_t)(y2 - y1));

   if(x1 > x2)
      addx = -1;
   if(y1 > y2)
      addy = -1;

   if(dx >= dy)
   {
      dy *= 2;
      P = dy - dx;
      diff = P - dx;

      for(; i<=dx; ++i)
      {
         lcdDrawPixel(x1, y1, color);

         if(P < 0)
         {
            P  += dy;
            x1 += addx;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
   else
   {
      dx *= 2;
      P = dx - dy;
      diff = P - dy;

      for(; i<=dy; ++i)
      {
         lcdDrawPixel(x1, y1, color);

         if(P < 0)
         {
            P  += dx;
            y1 += addy;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
}

void lcdChar(unsigned short x,unsigned short y,unsigned char c,unsigned short charcolor,unsigned short bkcolor) {
  unsigned short i=0;
  unsigned short j=0;
  unsigned char buffer[16];
  unsigned char tmp_char=0;
  GetASCIICode(buffer,c);
  for (i=0;i<16;i++)
  {
    tmp_char=buffer[i];
    for (j=0;j<8;j++)
    {
      if (((tmp_char >> (7-j)) & 0x01) == 0x01)
        {
          lcdDrawPixel(x+j,y+i,charcolor);
        }
        else
        {
          lcdDrawPixel(x+j,y+i,bkcolor);
        }
    }
  }
}

void lcdString(uint16_t x, uint16_t y, uint8_t *str,uint16_t color, uint16_t bkcolor) {
	uint8_t TempChar;

	do {
		TempChar=*str++;  
		lcdChar(x,y,TempChar,color,bkcolor);    
		if (x<232) {
			x+=8;
		} else if (y<304) {
			x=0;
			y+=16;
		} else {
			x=0;
			y=0;
		}    
	} while (*str!=0);
}

uint16_t lcdBGR2RGB(uint16_t color) {
	uint16_t  r, g, b, rgb;

	b = ( color>>0 )  & 0x1f;
	g = ( color>>5 )  & 0x3f;
	r = ( color>>11 ) & 0x1f;
 
	rgb =  (b<<11) + (g<<5) + (r<<0);

	return( rgb );
}

void lcdFillArea(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint16_t i;
 
	for(i = y0; i < y1; i++)
		lcdDrawLine(x0, i, x1, i, color);
}

void lcdFillArea2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	uint32_t index, area;

	area = ((x1-x0) * (y1-y0));

	lcdSetWindows(x0, y0, x1, y1);
	lcdSetCursor(x0, x1);
	
	Clr_CS;
	lcdWriteIndex(0x0022);
	for(index = 0; index < area; index++)
		lcdWriteData(color);
	Set_CS;
}

void lcdDrawRect(uint8_t x0, uint16_t y0, uint8_t x1, uint16_t y1, uint8_t filled, uint16_t color) {
	uint8_t i,TempX;
	uint16_t j, TempY;

	if (x0 > x1) {
		TempX = x1;
		x1 = x0;
		x0 = TempX;
	}
	if (y0 > y1) {
		TempY = y1;
		y1 = y0;
		y0 = TempY;
	}
	if(filled) {
		for(i=x0; i<x1; i++)
			for(j=y0; j<y1; j++)
				lcdDrawPixel(i , j , color);
	} else {
		lcdDrawLine(x0, y0, x1, y0, color);
		lcdDrawLine(x0, y1, x1, y1, color);
		lcdDrawLine(x0, y0, x0, y1, color);
		lcdDrawLine(x1, y0, x1, y1, color);
	}
}

void lcdDrawCircle(uint8_t x, uint16_t y, uint16_t radius, uint8_t filled, uint16_t color) {
	int16_t a, b, P;
	a = 0;
	b = radius;
	P = 1 - radius;

	do {
		if(filled) {
			lcdDrawLine(x-a, y+b, x+a, y+b, color);
			lcdDrawLine(x-a, y-b, x+a, y-b, color);
			lcdDrawLine(x-b, y+a, x+b, y+a, color);
			lcdDrawLine(x-b, y-a, x+b, y-a, color);
		} else {
			lcdDrawPixel(a+x, b+y, color);
			lcdDrawPixel(b+x, a+y, color);
			lcdDrawPixel(x-a, b+y, color);
			lcdDrawPixel(x-b, a+y, color);
			lcdDrawPixel(b+x, y-a, color);
			lcdDrawPixel(a+x, y-b, color);
			lcdDrawPixel(x-a, y-b, color);
			lcdDrawPixel(x-b, y-a, color);
		}

	if(P < 0)
		P += 3 + 2*a++;
	else
		P += 5 + 2*(a++ - b--);
	} while(a <= b);
}

void lcdTest(void) {
	uint16_t x;
	uint8_t y;
	uint8_t r,g,b;

	for(y = 0; y < SCREEN_HEIGHT; y++) {
		for(x = 0; x < SCREEN_WIDTH; x++) {
			r = x + y;
			g = x - y;
			b = y - x;
			lcdDrawPixel(y, x, RGB565CONVERT(r,g,b));
		}
	}
}

void lcdInit() {
  lcdDelay(5);
  DeviceCode = lcdReadReg(0x0000);
  if(DeviceCode==0x9325 || DeviceCode==0x9328)
  {
    lcdWriteReg(0x00e7,0x0010);      
    lcdWriteReg(0x0000,0x0001);  	
    lcdWriteReg(0x0001,0x0100);     
    lcdWriteReg(0x0002,0x0700); 
    lcdWriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) ); 
    lcdWriteReg(0x0004,0x0000);                                   
    lcdWriteReg(0x0008,0x0207);	           
    lcdWriteReg(0x0009,0x0000);         
    lcdWriteReg(0x000a,0x0000);
    lcdWriteReg(0x000c,0x0001);	
    lcdWriteReg(0x000d,0x0000); 			        
    lcdWriteReg(0x000f,0x0000);
    lcdWriteReg(0x0010,0x0000);   
    lcdWriteReg(0x0011,0x0007);
    lcdWriteReg(0x0012,0x0000);                                                                 
    lcdWriteReg(0x0013,0x0000);                 
    lcdDelay(5);  
    lcdWriteReg(0x0010,0x1590);   
    lcdWriteReg(0x0011,0x0227);
    lcdDelay(5);  
    lcdWriteReg(0x0012,0x009c);                  
    lcdDelay(5);  
    lcdWriteReg(0x0013,0x1900);   
    lcdWriteReg(0x0029,0x0023);
    lcdWriteReg(0x002b,0x000e);
    lcdDelay(5);  
    lcdWriteReg(0x0020,0x0000);                                                            
    lcdWriteReg(0x0021,0x0000);           
    lcdDelay(5);  		
    lcdWriteReg(0x0030,0x0007); 
    lcdWriteReg(0x0031,0x0707);   
    lcdWriteReg(0x0032,0x0006);
    lcdWriteReg(0x0035,0x0704);
    lcdWriteReg(0x0036,0x1f04); 
    lcdWriteReg(0x0037,0x0004);

    lcdWriteReg(0x0038,0x0000);        
    lcdWriteReg(0x0039,0x0706);     
    lcdWriteReg(0x003c,0x0701);
    lcdWriteReg(0x003d,0x000f);
    lcdDelay(5);  		
    lcdWriteReg(0x0050,0x0000);        
    lcdWriteReg(0x0051,0x00ef);   
    lcdWriteReg(0x0052,0x0000);     
    lcdWriteReg(0x0053,0x013f);
    lcdWriteReg(0x0060,0xa700);        
    lcdWriteReg(0x0061,0x0001); 
    lcdWriteReg(0x006a,0x0000);
    lcdWriteReg(0x0080,0x0000);
    lcdWriteReg(0x0081,0x0000);
    lcdWriteReg(0x0082,0x0000);
    lcdWriteReg(0x0083,0x0000);
    lcdWriteReg(0x0084,0x0000);
    lcdWriteReg(0x0085,0x0000);
      
    lcdWriteReg(0x0090,0x0010);     
    lcdWriteReg(0x0092,0x0000);  
    lcdWriteReg(0x0093,0x0003);
    lcdWriteReg(0x0095,0x0110);
    lcdWriteReg(0x0097,0x0000);        
    lcdWriteReg(0x0098,0x0000);  
    lcdWriteReg(0x0007,0x0133);
    
    lcdWriteReg(0x0020,0x0000); 
    lcdWriteReg(0x0021,0x0000); 
  }
  else if(DeviceCode==0x9320 || DeviceCode==0x9300)
  {
    lcdWriteReg(0x00,0x0000);
	lcdWriteReg(0x01,0x0100);
	lcdWriteReg(0x02,0x0700);
	lcdWriteReg(0x03,0x1018);
	
	lcdWriteReg(0x04,0x0000);
    lcdWriteReg(0x08,0x0202);
	lcdWriteReg(0x09,0x0000);
	lcdWriteReg(0x0a,0x0000);
    lcdWriteReg(0x0c,(1<<0));
	lcdWriteReg(0x0d,0x0000);
	lcdWriteReg(0x0f,0x0000);
	
    lcdDelay(10);
	lcdWriteReg(0x07,0x0101);	
    lcdDelay(10); 

	lcdWriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));
	lcdWriteReg(0x11,0x0007);				
	lcdWriteReg(0x12,(1<<8)|(1<<4)|(0<<0));		
	lcdWriteReg(0x13,0x0b00);						
	lcdWriteReg(0x29,0x0000);							
	
	lcdWriteReg(0x2b,(1<<14)|(1<<4));
		
	lcdWriteReg(0x50,0);   
	lcdWriteReg(0x51,239);	
	lcdWriteReg(0x52,0);	 
	lcdWriteReg(0x53,319);	  
	
	lcdWriteReg(0x60,0x2700);
	lcdWriteReg(0x61,0x0001);
	lcdWriteReg(0x6a,0x0000);	
	
	lcdWriteReg(0x80,0x0000);
	lcdWriteReg(0x81,0x0000);
	lcdWriteReg(0x82,0x0000);
	lcdWriteReg(0x83,0x0000);
	lcdWriteReg(0x84,0x0000);
	lcdWriteReg(0x85,0x0000);	
	
    lcdWriteReg(0x90,(0<<7)|(16<<0));
	lcdWriteReg(0x92,0x0000);	
	lcdWriteReg(0x93,0x0001);	
    lcdWriteReg(0x95,0x0110);
	lcdWriteReg(0x97,(0<<8));	
	lcdWriteReg(0x98,0x0000);

    lcdWriteReg(0x07,0x0173);
  }
  else if(DeviceCode==0x9331)
  {
	lcdWriteReg(0x00E7, 0x1014);
	lcdWriteReg(0x0001, 0x0100);
	lcdWriteReg(0x0002, 0x0200);
	lcdWriteReg(0x0003, 0x1030);
	lcdWriteReg(0x0008, 0x0202); 
    lcdWriteReg(0x0009, 0x0000); 
	lcdWriteReg(0x000A, 0x0000); 
	lcdWriteReg(0x000C, 0x0000); 
	lcdWriteReg(0x000D, 0x0000);   
	lcdWriteReg(0x000F, 0x0000);  
	lcdWriteReg(0x0010, 0x0000);  
	lcdWriteReg(0x0011, 0x0007);   
	lcdWriteReg(0x0012, 0x0000); 
	lcdWriteReg(0x0013, 0x0000); 
    lcdDelay(20);                
	lcdWriteReg(0x0010, 0x1690); 
	lcdWriteReg(0x0011, 0x0227); 
    lcdDelay(5);                 
	lcdWriteReg(0x0012, 0x000C); 
    lcdDelay(5);                 
	lcdWriteReg(0x0013, 0x0800); 
	lcdWriteReg(0x0029, 0x0011); 
	lcdWriteReg(0x002B, 0x000B); 
    lcdDelay(5);                 
	lcdWriteReg(0x0020, 0x0000);  
	lcdWriteReg(0x0021, 0x0000);   
	lcdWriteReg(0x0030, 0x0000);
	lcdWriteReg(0x0031, 0x0106);
	lcdWriteReg(0x0032, 0x0000);
	lcdWriteReg(0x0035, 0x0204);
	lcdWriteReg(0x0036, 0x160A);
	lcdWriteReg(0x0037, 0x0707);
	lcdWriteReg(0x0038, 0x0106);
	lcdWriteReg(0x0039, 0x0707);
	lcdWriteReg(0x003C, 0x0402);
	lcdWriteReg(0x003D, 0x0C0F);
	lcdWriteReg(0x0050, 0x0000);  
	lcdWriteReg(0x0051, 0x00EF); 
	lcdWriteReg(0x0052, 0x0000);
	lcdWriteReg(0x0053, 0x013F);
	lcdWriteReg(0x0060, 0x2700);
	lcdWriteReg(0x0061, 0x0001); 
	lcdWriteReg(0x006A, 0x0000);
	lcdWriteReg(0x0080, 0x0000);
	lcdWriteReg(0x0081, 0x0000);
	lcdWriteReg(0x0082, 0x0000);
	lcdWriteReg(0x0083, 0x0000);
	lcdWriteReg(0x0084, 0x0000);
	lcdWriteReg(0x0085, 0x0000);
	lcdWriteReg(0x0090, 0x0010);
	lcdWriteReg(0x0092, 0x0600);
	lcdWriteReg(0x0007,0x0021);		
    lcdDelay(5); 
	lcdWriteReg(0x0007,0x0061);
    lcdDelay(5);  
	lcdWriteReg(0x0007,0x0133); 
    lcdDelay(5);   
  }
  else if(DeviceCode==0x9919)
  {
	lcdWriteReg(0x28,0x0006);
	lcdWriteReg(0x00,0x0001);		
	lcdWriteReg(0x10,0x0000);		
	lcdWriteReg(0x01,0x72ef);
	lcdWriteReg(0x02,0x0600);
	lcdWriteReg(0x03,0x6a38);	
	lcdWriteReg(0x11,0x6874);
	lcdWriteReg(0x0f,0x0000);
	lcdWriteReg(0x0b,0x5308);
	lcdWriteReg(0x0c,0x0003);
	lcdWriteReg(0x0d,0x000a);
	lcdWriteReg(0x0e,0x2e00);  
	lcdWriteReg(0x1e,0x00be);
	lcdWriteReg(0x25,0x8000);
	lcdWriteReg(0x26,0x7800);
	lcdWriteReg(0x27,0x0078);
	lcdWriteReg(0x4e,0x0000);
	lcdWriteReg(0x4f,0x0000);
	lcdWriteReg(0x12,0x08d9);
	lcdWriteReg(0x30,0x0000);
	lcdWriteReg(0x31,0x0104);	 
	lcdWriteReg(0x32,0x0100);	
    lcdWriteReg(0x33,0x0305);	
    lcdWriteReg(0x34,0x0505);	 
	lcdWriteReg(0x35,0x0305);	
    lcdWriteReg(0x36,0x0707);	
    lcdWriteReg(0x37,0x0300);	
	lcdWriteReg(0x3a,0x1200);	
	lcdWriteReg(0x3b,0x0800);		 
    lcdWriteReg(0x07,0x0033);
  }
  else if(DeviceCode==0x1505)
  {
    lcdWriteReg(0x0007,0x0000);
    lcdDelay(5);              
    lcdWriteReg(0x0012,0x011C);
    lcdWriteReg(0x00A4,0x0001); 
    lcdWriteReg(0x0008,0x000F);
    lcdWriteReg(0x000A,0x0008);
    lcdWriteReg(0x000D,0x0008);    
    lcdWriteReg(0x0030,0x0707);
    lcdWriteReg(0x0031,0x0007); 
    lcdWriteReg(0x0032,0x0603); 
    lcdWriteReg(0x0033,0x0700); 
    lcdWriteReg(0x0034,0x0202); 
    lcdWriteReg(0x0035,0x0002); 
    lcdWriteReg(0x0036,0x1F0F);
    lcdWriteReg(0x0037,0x0707); 
    lcdWriteReg(0x0038,0x0000); 
    lcdWriteReg(0x0039,0x0000); 
    lcdWriteReg(0x003A,0x0707); 
    lcdWriteReg(0x003B,0x0000); 
    lcdWriteReg(0x003C,0x0007); 
    lcdWriteReg(0x003D,0x0000); 
    lcdDelay(5);               
    lcdWriteReg(0x0007,0x0001);
    lcdWriteReg(0x0017,0x0001);
    lcdDelay(5);              
    lcdWriteReg(0x0010,0x17A0); 
    lcdWriteReg(0x0011,0x0217);
    lcdWriteReg(0x0012,0x011E);
    lcdWriteReg(0x0013,0x0F00);
    lcdWriteReg(0x002A,0x0000);  
    lcdWriteReg(0x0029,0x000A);
    lcdWriteReg(0x0012,0x013E);
    lcdWriteReg(0x0050,0x0000);
    lcdWriteReg(0x0051,0x00EF); 
    lcdWriteReg(0x0052,0x0000); 
    lcdWriteReg(0x0053,0x013F); 
    lcdWriteReg(0x0060,0x2700); 
    lcdWriteReg(0x0061,0x0001); 
    lcdWriteReg(0x006A,0x0000); 
    lcdWriteReg(0x0080,0x0000); 
    lcdWriteReg(0x0081,0x0000); 
    lcdWriteReg(0x0082,0x0000); 
    lcdWriteReg(0x0083,0x0000); 
    lcdWriteReg(0x0084,0x0000); 
    lcdWriteReg(0x0085,0x0000); 
    lcdWriteReg(0x0090,0x0013);    	
    lcdWriteReg(0x0092,0x0300); 
    lcdWriteReg(0x0093,0x0005); 
    lcdWriteReg(0x0095,0x0000); 
    lcdWriteReg(0x0097,0x0000); 
    lcdWriteReg(0x0098,0x0000); 
  
    lcdWriteReg(0x0001,0x0100); 
    lcdWriteReg(0x0002,0x0700); 
    lcdWriteReg(0x0003,0x1030); 
    lcdWriteReg(0x0004,0x0000); 
    lcdWriteReg(0x000C,0x0000); 
    lcdWriteReg(0x000F,0x0000); 
    lcdWriteReg(0x0020,0x0000); 
    lcdWriteReg(0x0021,0x0000); 
    lcdWriteReg(0x0007,0x0021); 
    lcdDelay(20);                 
    lcdWriteReg(0x0007,0x0061); 
    lcdDelay(20);                		
    lcdWriteReg(0x0007,0x0173); 
    lcdDelay(20);                  		
  }							 
  else if(DeviceCode==0x8989)
  {
    lcdWriteReg(0x0000,0x0001);    lcdDelay(5);
    lcdWriteReg(0x0003,0xA8A4);    lcdDelay(5);   
    lcdWriteReg(0x000C,0x0000);    lcdDelay(5);   
    lcdWriteReg(0x000D,0x080C);    lcdDelay(5);   
    lcdWriteReg(0x000E,0x2B00);    lcdDelay(5);   
    lcdWriteReg(0x001E,0x00B0);    lcdDelay(5);  
	lcdWriteReg(0x0001,0x2B3F);
    lcdWriteReg(0x0002,0x0600);    lcdDelay(5);
    lcdWriteReg(0x0010,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0011,0x6070);    lcdDelay(5);
    lcdWriteReg(0x0005,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0006,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0016,0xEF1C);    lcdDelay(5);
    lcdWriteReg(0x0017,0x0003);    lcdDelay(5);
    lcdWriteReg(0x0007,0x0133);    lcdDelay(5);         
    lcdWriteReg(0x000B,0x0000);    lcdDelay(5);
    lcdWriteReg(0x000F,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0041,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0042,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0048,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0049,0x013F);    lcdDelay(5);
    lcdWriteReg(0x004A,0x0000);    lcdDelay(5);
    lcdWriteReg(0x004B,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0044,0xEF00);    lcdDelay(5);
    lcdWriteReg(0x0045,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0046,0x013F);    lcdDelay(5);
    lcdWriteReg(0x0030,0x0707);    lcdDelay(5);
    lcdWriteReg(0x0031,0x0204);    lcdDelay(5);
    lcdWriteReg(0x0032,0x0204);    lcdDelay(5);
    lcdWriteReg(0x0033,0x0502);    lcdDelay(5);
    lcdWriteReg(0x0034,0x0507);    lcdDelay(5);
    lcdWriteReg(0x0035,0x0204);    lcdDelay(5);
    lcdWriteReg(0x0036,0x0204);    lcdDelay(5);
    lcdWriteReg(0x0037,0x0502);    lcdDelay(5);
    lcdWriteReg(0x003A,0x0302);    lcdDelay(5);
    lcdWriteReg(0x003B,0x0302);    lcdDelay(5);
    lcdWriteReg(0x0023,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0024,0x0000);    lcdDelay(5);
    lcdWriteReg(0x0025,0x8000);    lcdDelay(5);
    lcdWriteReg(0x004f,0);      
    lcdWriteReg(0x004e,0);     
  }
  lcdDelay(5);  		
}

