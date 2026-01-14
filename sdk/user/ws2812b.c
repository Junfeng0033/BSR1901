

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
	uint8_t sendData[24];
	int indx = 0;

	for (int i=23; i>=0; i--)
	{
		if (((color>>i)&0x01) == 1) sendData[indx++] = 0b110;  // store 1
		else sendData[indx++] = 0b100;  // store 0
	}

	HAL_SPI_Transmit(&hspi1, sendData, 24, 1000);
}




void WS2812_Send (void)
{
	for (int i=0; i<NUM_LED; i++)
	{
		WS2812_Send_Spi(LED_Data[i][1],LED_Data[i][2],LED_Data[i][3]);
	}
	HAL_Delay (1);
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