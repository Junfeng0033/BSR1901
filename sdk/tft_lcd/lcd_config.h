

#ifndef __LCD_CONFIG_H__
#define __LCD_CONFIG_H__


//#define LCD_TYPE_ST7735S_0P96_80X160 // 0.96寸 ST7735S

#define LCD_TYPE_GC9A01_1P28_240X240    // 1.28寸 GC9A01

// #define LCD_TYPE_NV3022B_1P47_128X28 // 1.47寸 NV3022B




extern void LCD_Init_GC9A01_M128T_240240(void);

extern void LCD_Init_ST7735S_UN096T_80X160(void);

extern void LCD_Init_NV3022B_1P47_128X128(void);
	
	
	
	
	

#endif /* __LCD_CONFIG_H__ */


