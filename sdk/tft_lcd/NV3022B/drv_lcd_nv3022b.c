////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "lcd_config.h"
#include "platform_config.h"



extern void Lcd_WriteIndex(uint8_t Index);
extern void Lcd_WriteData(uint8_t Data);

extern void delay_ms(unsigned int delay_val);



#ifdef LCD_TYPE_NV3022B_1P47_128X28




#if 1

typedef enum {
    Command    = 0x00000000,
    Parameter  = 0x00000001,
} LCD_DC_T;   




//Write(Command , 0xB2);
void Write(LCD_DC_T dc,uint8 data)
{
	
	if(dc==Command)
	{
		//LCD_NV3023_CMD;
		Lcd_WriteIndex(data);
	}		
	else if(dc==Parameter)
	{
		//LCD_NV3023_Parameter;
		Lcd_WriteData(data);
	}

}

#endif






// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width 
#define 	LCDD_DISP_X		        128

// Number of pixels in the display height 
#define 	LCDD_DISP_Y           128

//#define  	LCD_ILI9225B_ID      	0x9225


//LCD Init For 1.47Inch LCD Panel with NV3022B.
void LCD_Init_NV3022B_1P47_128X128(void)
{


Write(Command , 0x11);     

delay_ms(500);              

Write(Command , 0x36);     
Write(Parameter , 0x00);   

Write(Command , 0x3A);     
Write(Parameter , 0x05);   

Write(Command , 0xB2);     
Write(Parameter , 0x0C);   
Write(Parameter , 0x0C);   
Write(Parameter , 0x00);   
Write(Parameter , 0x33);   
Write(Parameter , 0x33);   

Write(Command , 0xB7);     
Write(Parameter , 0x00);   

Write(Command , 0xBB);     
Write(Parameter , 0x3F);   

Write(Command , 0xC0);     
Write(Parameter , 0x2C);   

Write(Command , 0xC2);     
Write(Parameter , 0x01);   

Write(Command , 0xC3);     
Write(Parameter , 0x0D);   

Write(Command , 0xC6);     
Write(Parameter , 0x0F);     

Write(Command , 0xD0);     
Write(Parameter , 0xA7);   

Write(Command , 0xD0);     
Write(Parameter , 0xA4);   
Write(Parameter , 0xA1);   

Write(Command , 0xD6);     
Write(Parameter , 0xA1);   //sleep inºó£¬gateÊä³öÎªGND

Write(Command , 0xE0);
Write(Parameter , 0xF0);
Write(Parameter , 0x00);
Write(Parameter , 0x02);
Write(Parameter , 0x03);
Write(Parameter , 0x02);
Write(Parameter , 0x10);
Write(Parameter , 0x29);
Write(Parameter , 0x33);
Write(Parameter , 0x43);
Write(Parameter , 0x22);
Write(Parameter , 0x0D);
Write(Parameter , 0x0D);
Write(Parameter , 0x2D);
Write(Parameter , 0x37);

Write(Command , 0xE1);
Write(Parameter , 0xF0);
Write(Parameter , 0x08);
Write(Parameter , 0x0D);
Write(Parameter , 0x0F);
Write(Parameter , 0x0D);
Write(Parameter , 0x08);
Write(Parameter , 0x29);
Write(Parameter , 0x32);
Write(Parameter , 0x43);
Write(Parameter , 0x3D);
Write(Parameter , 0x19);
Write(Parameter , 0x19);
Write(Parameter , 0x39);
Write(Parameter , 0x3F);

Write(Command , 0x21);     

Write(Command , 0x29);     

Write(Command , 0x2A);     //Column Address Set
Write(Parameter , 0x00);   
Write(Parameter , 0x00);   //0
Write(Parameter , 0x00);   
Write(Parameter , 0xEF);   //239

Write(Command , 0x2B);     //Row Address Set
Write(Parameter , 0x00);   
Write(Parameter , 0x00);   //0
Write(Parameter , 0x00);   
Write(Parameter , 0xEF);   //239

Write(Command , 0x2C);     
	
	
}

#endif

