
#include "spi.h"
#include "uart.h"
#include "dma.h"
#include "gpio.h"


//#include <stdarg.h>
//#include <stdio.h>



/*

连接示意图如下：

MCU引脚					WS2812点阵屏							备注

GND								GND								必须共地

SPI MOSI						DIN								通过电阻连接

-								VCC								接独立5V电源

*/






/*

可以使用SPI+DMA模式。如果SPI时钟设置为6MHz，发送一个字节是8/6000000=1.333us。
ws2812b设置一个灯珠的颜色需要3个字节(r, g, b)，3*8=24位。使SPI的字节正好等于ws2812b的一个位。
因此控制一个ws2812b需要SPI发送24byte，在头部加入两个字节0，在尾部加入一个字节0进行消抖，
总共需要24+3=27byte。



一个RGB灯亮起需要24个数据，0x0F代表1码，0x01是0码。
在colorful_lights_display_buffer数据中填入

u8 colorful_lights_display_buffer[24] = 
{
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
};   //rgb数据BUF


*/


#define NUM_LED 8
uint8_t LED_Data[NUM_LED][4];



void setLED (int led, int RED, int GREEN, int BLUE)
{
	LED_Data[led][0] = led;
	LED_Data[led][1] = GREEN;
	LED_Data[led][2] = RED;
	LED_Data[led][3] = BLUE;
}



void ws2812_spi (int GREEN, int RED, int BLUE)
{
#if USE_BRIGHTNESS
	if (brightness>100)brightness = 100;
	GREEN = GREEN*brightness/100;
	RED = RED*brightness/100;
	BLUE = BLUE*brightness/100;
#endif
	uint32_t color = GREEN<<16 | RED<<8 | BLUE;
	//uint8_t sendData[24];
	uint16 sendData[24];
	int indx = 0;

	for (int i=23; i>=0; i--)
	{
		if (((color>>i)&0x01) == 1) sendData[indx++] = 0b110;  // store 1
		else sendData[indx++] = 0b100;  // store 0
	}

	//HAL_SPI_Transmit(&hspi1, sendData, 24, 1000);
	HW_SPI_Tx_DMA(sendData, 24);
}




void WS2812_Send (void)
{
	for (int i=0; i<NUM_LED; i++)
	{
		//WS2812_Send_Spi(LED_Data[i][1],LED_Data[i][2],LED_Data[i][3]);
	}
	//HAL_Delay (1);
}






#if 0

while (1)
{
	  for (int i=0; i<4; i++)
	  {
		 setLED(i, 255, 0, 0);
	  }
	  WS2812_Send(5);
	  HAL_Delay(1000);

	  for (int i=0; i<4; i++)
	  {
		 setLED(i, 0, 255, 0);
	  }
	  WS2812_Send(5);
	  HAL_Delay(1000);

	  for (int i=0; i<4; i++)
	  {
		 setLED(i, 0, 0, 255);
	  }
	  WS2812_Send(5);
	  HAL_Delay(1000);
}
  
#endif






/*****************************WS2812 SPI *****************************************

//https://blog.csdn.net/karaxiaoyu/article/details/120944765


rgbbyte = rgb2byte(0xff,0x0,0x0)---[SPI_24bit_Transfer]
rstbyte = bytes([0xff]*16)---------[SPI_8bit_Transfer]

outbyte = rstbyte+rgbbyte+rstbyte




while True:

    rgbbyte = rgb2byte(0xff,0x0,0x0)		//输出RGB为（0xff，0x0,0x0），对应WS2812应该是输出红色
    outbyte = rstbyte+rgbbyte+rstbyte
    hspi.write(outbyte)
    time.sleep_ms(500)
 
    rgbbyte = rgb2byte(0x0,0xff,0x0)
    outbyte = rstbyte+rgbbyte+rstbyte
    hspi.write(outbyte)
    time.sleep_ms(500)
 
    rgbbyte = rgb2byte(0x0,0x0,0xff)
    outbyte = rstbyte+rgbbyte+rstbyte
    hspi.write(outbyte)
    time.sleep_ms(500)
		
		
*****************************WS2812 SPI *****************************************/




//ws2812_show(frame_buf, LED_COUNT);

void ws2812_show(uint16_t *buf, uint16_t led_count) {
	HW_SPI_Tx_DMA(buf, led_count * 24);
}










