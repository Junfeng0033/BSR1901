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



// =============================================================================
// 
// -----------------------------------------------------------------------------
// =============================================================================

// =============================================================================
//  MACROS
// =============================================================================



#define 	LCD_NV3023_CMD								Lcd_WriteIndex //写入命令
#define 	LCD_NV3023_Parameter					Lcd_WriteData	 //写入数据



// =============================================================================
// Screen properties
// =============================================================================

// Number of actual pixels in the display width 
#define 	LCDD_DISP_X		        240

// Number of pixels in the display height 
#define 	LCDD_DISP_Y           240

//#define  	LCD_ILI9225B_ID      	0x9225



void LCD_Init_NV3023(void)
{

	//----------------Star Initial Sequence-------//
	LCD_NV3023_CMD(0xff);
	LCD_NV3023_Parameter(0xa5); //
	LCD_NV3023_CMD(0x3E);
	LCD_NV3023_Parameter(0x08);
	LCD_NV3023_CMD(0x3A);
	LCD_NV3023_Parameter(0x65);
	LCD_NV3023_CMD(0x82);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x98);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x63);
	LCD_NV3023_Parameter(0x0f);
	LCD_NV3023_CMD(0x64);
	LCD_NV3023_Parameter(0x0f);
	LCD_NV3023_CMD(0xB4);
	LCD_NV3023_Parameter(0x34);
	LCD_NV3023_CMD(0xB5);
	LCD_NV3023_Parameter(0x30);
	LCD_NV3023_CMD(0x83);
	LCD_NV3023_Parameter(0x03);
	LCD_NV3023_CMD(0x86);//
	LCD_NV3023_Parameter(0x04);
	LCD_NV3023_CMD(0x87);
	LCD_NV3023_Parameter(0x16);
	LCD_NV3023_CMD(0x88);//VCOM
	LCD_NV3023_Parameter(0x28);
	LCD_NV3023_CMD(0x89);//
	LCD_NV3023_Parameter(0x2F);//2 F
	LCD_NV3023_CMD(0x93); //
	LCD_NV3023_Parameter(0x63);
	LCD_NV3023_CMD(0x96);
	LCD_NV3023_Parameter(0x81);
	LCD_NV3023_CMD(0xC3);
	LCD_NV3023_Parameter(0x11);
	LCD_NV3023_CMD(0xE6);
	LCD_NV3023_Parameter(0x00);
	LCD_NV3023_CMD(0x99);
	LCD_NV3023_Parameter(0x01);

	LCD_NV3023_CMD(0x44);
	LCD_NV3023_Parameter(0x00);

	////////////////////////gamma_set//////////////////////////////////////
	LCD_NV3023_CMD(0x70);LCD_NV3023_Parameter(0x02);//VRP 0 1
	LCD_NV3023_CMD(0x71);LCD_NV3023_Parameter(0x0E);//VRP 1 3
	LCD_NV3023_CMD(0x72);LCD_NV3023_Parameter(0x0a);//VRP 2 7
	LCD_NV3023_CMD(0x73);LCD_NV3023_Parameter(0x12);//VRP 3 9
	LCD_NV3023_CMD(0x74);LCD_NV3023_Parameter(0x19);//VRP 6 11
	LCD_NV3023_CMD(0x75);LCD_NV3023_Parameter(0x1D);//VRP 8 13
	LCD_NV3023_CMD(0x76);LCD_NV3023_Parameter(0x46);//VRP 10 5
	LCD_NV3023_CMD(0x77);LCD_NV3023_Parameter(0x0B);//VRP 14 15
	LCD_NV3023_CMD(0x78);LCD_NV3023_Parameter(0x0E);//VRP 17 16
	LCD_NV3023_CMD(0x79);LCD_NV3023_Parameter(0x3D);//VRP 21 6
	LCD_NV3023_CMD(0x7a);LCD_NV3023_Parameter(0x05);//VRP 23 14
	LCD_NV3023_CMD(0x7b);LCD_NV3023_Parameter(0x07);//VRP 25 12
	LCD_NV3023_CMD(0x7c);LCD_NV3023_Parameter(0x12);//VRP 28 10
	LCD_NV3023_CMD(0x7d);LCD_NV3023_Parameter(0x0B);//VRP 29 8
	LCD_NV3023_CMD(0x7e);LCD_NV3023_Parameter(0x0B);//VRP 30 4
	LCD_NV3023_CMD(0x7f);LCD_NV3023_Parameter(0x08);//VRP 31 2
	LCD_NV3023_CMD(0xa0);LCD_NV3023_Parameter(0x1E);//VRN 0 1
	LCD_NV3023_CMD(0xa1);LCD_NV3023_Parameter(0x3F);//VRN 1 3
	LCD_NV3023_CMD(0xa2);LCD_NV3023_Parameter(0x0A);//VRN 2 7
	LCD_NV3023_CMD(0xa3);LCD_NV3023_Parameter(0x0D);//VRN 3 9
	LCD_NV3023_CMD(0xa4);LCD_NV3023_Parameter(0x08);//VRN 6 11
	LCD_NV3023_CMD(0xa5);LCD_NV3023_Parameter(0x23);//VRN 8 13
	LCD_NV3023_CMD(0xa6);LCD_NV3023_Parameter(0x3D);//VRN 10 5
	LCD_NV3023_CMD(0xa7);LCD_NV3023_Parameter(0x04);//VRN 14 15
	LCD_NV3023_CMD(0xa8);LCD_NV3023_Parameter(0x09);//VRN 17 16
	LCD_NV3023_CMD(0xa9);LCD_NV3023_Parameter(0x30);//VRN 21 6
	LCD_NV3023_CMD(0xaa);LCD_NV3023_Parameter(0x0A);//VRN 23 14 //
	LCD_NV3023_CMD(0xab);LCD_NV3023_Parameter(0x0E);//VRN 25 12
	LCD_NV3023_CMD(0xac);LCD_NV3023_Parameter(0x0E);//VRN 28 10
	LCD_NV3023_CMD(0xad);LCD_NV3023_Parameter(0x07);//VRN 29 8
	LCD_NV3023_CMD(0xae);LCD_NV3023_Parameter(0x2D);//VRN 30 4
	LCD_NV3023_CMD(0xaf);LCD_NV3023_Parameter(0x10);//VRN 31 2
	//////////////////////////////////////////////////////////////////
	LCD_NV3023_CMD(0xff);
	LCD_NV3023_Parameter(0x00);

	LCD_NV3023_CMD(0x11);

	delay_ms(150);
	LCD_NV3023_CMD(0x36);
	LCD_NV3023_Parameter(0x08);
	LCD_NV3023_CMD(0x29);
	delay_ms(10);

}
