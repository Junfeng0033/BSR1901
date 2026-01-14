/*************************************************************************
 *
 * MODULE NAME:    GPIO_I2C.c
 * PROJECT CODE:   gek1109
 * DESCRIPTION:    gek1109 iic drv.
 * MAINTAINER:     jf
 * CREATION DATE:  24/11/20
 *
 *
 *
 * NOTES TO USERS:
 *   None.
 *
 *************************************************************************/

#include "platform_config.h"
#include "bsr1901.h"

#include "gpio.h"
#include "lcd_driver.h"

#include <stdarg.h>
#include <stdio.h>

//enum {
//    LOW = 0x00,HIGH,TOGG,
//    INPUT = 0x00,OUTPUT,IN_PU,IN_PD,AF_OD,AF_PP,INT_DOWN,INT_UP,
//    PA0 = 0x00,PA1,PA2,PA3,PA4,PA5,PA6,PA7,
//    PB0 = 0x08,PB1,PB2,PB3,PB4,PB5,PB6,PB7,
//};




// Global variant
#define SCL_PIN		PA5
#define SDA_PIN		PA4

UINT8 level = 0;




#define DURATION_INIT_1 		48//600ns
#define DURATION_INIT_2	    	48//600ns
#define DURATION_INIT_3 		48//600ns

#define DURATION_START_1		48//600ns
#define DURATION_START_2		48//600ns
#define DURATION_START_3		64//800ns

#define DURATION_STOP_1			64//800ns
#define DURATION_STOP_2			48//600ns
#define DURATION_STOP_3			104//1300ns

#define DURATION_HIGH				100//900ns
#define DURATION_LOW				100//1600ns

#define DELAY(DURATION)		{volatile UINT32 j; for(j = DURATION ; j >0; j--){}}



void hal_GpioSetIn(uint8 nGpio)
{
	uint8 pin=nGpio;

	if(pin<=7)
	{
		GPIODIR_0&= (~BIT(pin));
	}
	else if((pin>=8) &&  (pin<=15))
	{
		pin=pin-8;
		GPIODIR_1&= (~BIT(pin));
	}
}

void hal_GpioSetOut(uint8 nGpio)
{
	uint8 pin=nGpio;	
	if(pin<=7)
	{
		GPIODIR_0|= BIT(pin);
	}
	else if((pin>=8) &&  (pin<=15))
	{
		pin=pin-8;
		GPIODIR_1|= BIT(pin);
	}
}

uint8 hal_GpioGet(uint8 nGpio)
{
	uint8 pin=nGpio;
	if(pin<=7)
	{
		return (*((volatile unsigned long *)(GPIO_GROUP0_BASE+(1 << (pin + 2)))));
	}
	else if((pin>=8) && (pin<=15))
	{
		pin=pin-8;
		return (*((volatile unsigned long *)(GPIO_GROUP1_BASE+(1 << (pin + 2)))));
	}	

	return 0xff;
	
}



void hal_GpioSet(uint8 gpio)
{
		uint8 pin=gpio;
	  uint8 val=1;
	  if(pin<=7)
		{
			//#define GPIO_GROUP_A_BASE       (0x40018000)
			(*(volatile uint32*)(0x40018000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}
		else if((pin>=8) &&  (pin<=15))
		{
			//#define GPIO_GROUP_B_BASE       (0x40019000)
			pin=pin-8;
			(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}	
}



void hal_GpioClr(uint8 gpio)
{
		uint8 pin=gpio;
	  uint8 val=0;
	  if(pin<=7)
		{
			//#define GPIO_GROUP_A_BASE       (0x40018000)
			(*(volatile uint32*)(0x40018000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}
		else if((pin>=8) &&  (pin<=15))
		{
			//#define GPIO_GROUP_B_BASE       (0x40019000)
			pin=pin-8;
			(*(volatile uint32*)(0x40019000 + (1 << (pin + 2)) ) ) = (uint32)(val << pin);
		}		
	
}




VOID DRV_GpioSetDirection_I2C( UINT8 nGpio, UINT8 nDirection)
{

		if (nDirection == 1)
		{
			hal_GpioSetOut(nGpio);
		}
		else
		{
			hal_GpioSetIn(nGpio);
		}

}







VOID DRV_GpioSetLevel_I2C(UINT8 nGpio,UINT8 nLevel)
{
	
		uint8 gpio=nGpio;
		if (nLevel == 1)
		{
				hal_GpioSet(gpio);
		}
		else
		{
				hal_GpioClr(gpio);
		}

}




VOID DRV_GpioGetLevel_I2C(UINT8 nGpio,UINT8* nLevel)
{
	// nGpio = nGpio >> 1;
	*nLevel = hal_GpioGet(nGpio);

}




VOID GPIO_InitIO(UINT8 direction, UINT8 port)
{
    DRV_GpioSetDirection_I2C(port,direction);
}

VOID GPIO_WriteIO(UINT8 data, UINT8 port)
{
    DRV_GpioSetLevel_I2C(port,data);
}

UINT8 GPIO_ReadIO(UINT8 port)
{
    DRV_GpioGetLevel_I2C(port,&level);
    return level;
}







//
//-------------------------------------------------------------------
// Function:  gpio_i2c_init
// Purpose:  This function is used to init I2C port of the  device   
// In: 
// Return:      bool
//-------------------------------------------------------------------


uint8 gpio_i2c_initialize(VOID)
{
  uint8 i;
	// Set the GPIO pin to output status
	DRV_GpioSetDirection_I2C(SCL_PIN,1);
	DRV_GpioSetDirection_I2C(SDA_PIN,1);
	DELAY(DURATION_INIT_1);

	// Make the I2C bus in idle status
	DRV_GpioSetLevel_I2C(SCL_PIN,1);
	DELAY(DURATION_INIT_1);
	DRV_GpioSetLevel_I2C(SDA_PIN,1);
	DELAY(DURATION_INIT_1);

#if 1
	for( i=0;i<30;i++)
	{
		GPIO_WriteIO(0, SCL_PIN); /* low */
		DELAY(DURATION_LOW);   
		GPIO_WriteIO(1, SCL_PIN);
		DELAY(DURATION_HIGH);
	}
#endif

	return (TRUE);
}



VOID gpio_i2c_start(VOID) /* start or re-start */
{
#ifndef MCU_WIFI_PLAT
   GPIO_InitIO(1,SDA_PIN);

   DELAY(DURATION_START_1);
   //COS_Sleep(1000);

   GPIO_WriteIO(1,SDA_PIN);
   DELAY(DURATION_START_1);
   GPIO_WriteIO(1,SCL_PIN);
  

   //COS_Sleep(1000);

   DELAY(DURATION_START_1);
   GPIO_WriteIO(0,SDA_PIN);   
   DELAY(DURATION_START_2);
   GPIO_WriteIO(0,SCL_PIN);

   DELAY(DURATION_START_3);/* start condition */
#else
   GPIO_InitIO(1,SDA_PIN);
   GPIO_InitIO(1,SCL_PIN);

   GPIO_WriteIO(1,SDA_PIN);
   DELAY(DURATION_LOW);
   GPIO_WriteIO(1,SCL_PIN);
  
   DELAY(DURATION_LOW);
   GPIO_WriteIO(0,SDA_PIN);   
   DELAY(DURATION_LOW);
   GPIO_WriteIO(0,SCL_PIN);

   DELAY(DURATION_LOW);/* start condition */
#endif
}


VOID gpio_i2c_stop(VOID)
{
   GPIO_WriteIO(0,SCL_PIN);
   DELAY(DURATION_LOW);
   GPIO_InitIO(1,SDA_PIN);
   GPIO_WriteIO(0,SDA_PIN);
   DELAY(DURATION_STOP_1);
   GPIO_WriteIO(1,SCL_PIN);
   DELAY(DURATION_STOP_2);
   GPIO_WriteIO(1,SDA_PIN); /* stop condition */
   DELAY(DURATION_STOP_3);
}



UINT8 gpio_i2c_TxByte(UINT8 data) /* return 0 --> ack */
{
#ifndef MCU_WIFI_PLAT
	int32 i;
	uint8 temp_value = 0;
	for(i=7; (i>=0)&&(i<=7); i--)
	{
		GPIO_WriteIO( 0, SCL_PIN); /* low */
		DELAY(DURATION_LOW);
		if(i==7)GPIO_InitIO(1,SDA_PIN);
		DELAY(DURATION_LOW);

		GPIO_WriteIO(((data>>i)&0x01), SDA_PIN);
		DELAY(DURATION_LOW/2);
		GPIO_WriteIO( 1, SCL_PIN); /* high */
		DELAY(DURATION_HIGH);
	}
	GPIO_WriteIO(0, SCL_PIN); /* low */
	DELAY(DURATION_LOW);
	GPIO_InitIO(0,SDA_PIN);/* input  */
	DELAY(DURATION_LOW/2);
	GPIO_WriteIO(1, SCL_PIN); /* high */
	DELAY(DURATION_HIGH);
	temp_value = GPIO_ReadIO(SDA_PIN);
	GPIO_WriteIO(0, SCL_PIN); /* low */
	DELAY(DURATION_LOW);
	
	//printf("\r\n!!!!!!!!!!!gpio_i2c_TxByte  ACK or NAK ~~000~~~~######### = 0x%x \r\n",temp_value);	
	
	
	return temp_value;
#else
	int32 i;
	uint8 temp_value = 0;
    GPIO_InitIO(1,SDA_PIN);
    GPIO_InitIO(1,SCL_PIN);
	for(i=7; (i>=0)&&(i<=7); i--)
	{
		GPIO_WriteIO( 0, SCL_PIN); /* low */
		DELAY(DURATION_LOW);

		GPIO_WriteIO(((data>>i)&0x01), SDA_PIN);
		DELAY(DURATION_LOW);
		GPIO_WriteIO( 1, SCL_PIN); /* high */
		DELAY(DURATION_HIGH);
	}
	GPIO_WriteIO(0, SCL_PIN); /* low */
	GPIO_InitIO(0,SDA_PIN);/* input  */
	DELAY(DURATION_LOW*4);
	GPIO_WriteIO(1, SCL_PIN); /* high */
	//DELAY(DURATION_HIGH);
	temp_value = GPIO_ReadIO(SDA_PIN);
    DELAY(DURATION_LOW);
	GPIO_WriteIO(0, SCL_PIN); /* low */
	return temp_value;
#endif
}


VOID gpio_i2c_RxByte(uint8 *data, uint8 ack)
{
	int32 i;
	uint32 dataCache;

	dataCache = 0;
	
	
#if 0	
	for(i=7; (i>=0)&&(i<=7); i--)
	{
		GPIO_WriteIO(0, SCL_PIN);
		DELAY(DURATION_LOW);
		if(i==7)GPIO_InitIO(0,SDA_PIN);
		DELAY(DURATION_LOW);
		GPIO_WriteIO(1, SCL_PIN);
		DELAY(DURATION_HIGH);
		dataCache |= (GPIO_ReadIO(SDA_PIN)<<i);
		DELAY(DURATION_LOW/2);
	}

	printf("\r\n!!!!!!!!!!!gpio_i2c_RxByte ~~~~~~######### = 0x%x \r\n",dataCache);	
	
	GPIO_WriteIO(0, SCL_PIN);
	DELAY(DURATION_LOW);
	GPIO_InitIO(1,SDA_PIN);
	GPIO_WriteIO(ack, SDA_PIN);
	DELAY(DURATION_LOW/2);
	GPIO_WriteIO(1, SCL_PIN);
	DELAY(DURATION_HIGH);
	GPIO_WriteIO(0, SCL_PIN); /* low */
	DELAY(DURATION_LOW);
#else
	GPIO_InitIO(0,SDA_PIN);//set sda input
	
	for (i = 0; i < 8; i++) {
			dataCache <<= 1;
			GPIO_WriteIO(1, SCL_PIN);;
			DELAY(DURATION_HIGH);
			if (GPIO_ReadIO(SDA_PIN)) dataCache |= 0x01;
			GPIO_WriteIO(0, SCL_PIN);
			DELAY(DURATION_LOW);
	}

    GPIO_InitIO(1,SDA_PIN);//set sda output
	
    if (ack) GPIO_WriteIO(1, SDA_PIN);
    else GPIO_WriteIO(0, SDA_PIN);
    
    GPIO_WriteIO(1, SCL_PIN);
    DELAY(DURATION_HIGH);
    GPIO_WriteIO(0, SCL_PIN);
    GPIO_WriteIO(1, SDA_PIN);
	
#endif
	
	
  //printf("\r\n!!!!!!!!!!!gpio_i2c_RxByte ~~~3333~~~######### = 0x%x \r\n",dataCache);	
	
	*data = (uint8)dataCache;
}




VOID i2c_test(void)
{
	//gpio_i2c_Start();///start
	//gpio_i2c_TxByte(0x23);//chip adress
	//DELAY(DURATION_LOW*100);
  volatile UINT8 temp_value;
	GPIO_InitIO(0,SDA_PIN);
  temp_value = GPIO_ReadIO(SDA_PIN);
  //printf("i2c_test  temp_value=%d\n",temp_value);
}





VOID gpio_i2c_write_data(UINT32 addr,UINT8 * regaddr,UINT8 addrlen,UINT8 *data, UINT8 datalen)
{
	UINT8 i;
	if (SCL_PIN == 0)
	{
    //GI2C_ASSERT(0, "initial the gpio_i2c firstly.", "xx");
    //printf("inital gpio_i2c firstly.\n");
	}
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr<<1);//chip adress

	for(i=addrlen;i>0;i--,regaddr++)//addr
	{
		gpio_i2c_TxByte(*regaddr);
	}

	for( i=datalen;i>0;i--,data++)//data
	{
		gpio_i2c_TxByte(*data);
	}
	gpio_i2c_stop();
}

UINT8 gpio_i2c_read_data(UINT32 addr,UINT8 * regaddr, UINT8 addrlen,UINT8 *datas, UINT8 datalen)
{
	UINT8 tempdata;
	UINT8 i;
	UINT8 *data = datas;
	
	if(addrlen)
	{
		gpio_i2c_start();///start
		gpio_i2c_TxByte(addr<<1);//chip adress

		for(i=addrlen;i>0;i--,regaddr++)//addr
		{
			gpio_i2c_TxByte(*regaddr);
		}
	}
	
	gpio_i2c_start();//start
	gpio_i2c_TxByte((addr<<1)+1);//chip adress
	
	for( i=datalen-1;i>0;i--,data++)//data
	{
		gpio_i2c_RxByte(&tempdata, 0);
		*data = tempdata;	
		//GI2C_TRACE(GI2C_INFO_TRC, 0, "gss: gpio read 1*datas 0x%x",tempdata);
	}
	
	gpio_i2c_RxByte(&tempdata, 1);
	*data = tempdata;	
	
	gpio_i2c_stop();
	//GI2C_TRACE(GI2C_INFO_TRC, 0, "gss: gpio read end *datas 0x%x",*data);
	return *datas;
}

//void gpio_i2c_rdabt_rf_write_data(UINT32 addr,unsigned char regaddr, unsigned short *data, unsigned char datalen)
void gpio_i2c_rdabt_rf_write_data(UINT32 addr,unsigned char regaddr, unsigned char *data, unsigned char datalen)	
{
	unsigned char i=0;
	
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr);//chip adress
	
	DELAY(100);
	
  //hal_HstSendEvent(gpio_i2c_TxByte(addr)|0xdd110000);//chip adress
	gpio_i2c_TxByte(regaddr);

	DELAY(100);
	
#if 0
	for(i=0;i<datalen;i++,data++)//data
	{
		gpio_i2c_TxByte(*data>>8);
		gpio_i2c_TxByte(*data);
	}
#else
	for(i=0;i<datalen;i++,data++)//data
	{
		gpio_i2c_TxByte(*data);
	}
#endif	
	
	gpio_i2c_stop();
}

//void gpio_i2c_rdabt_rf_read_data(UINT32 addr,unsigned char regaddr, unsigned short *data, unsigned char datalen)
void gpio_i2c_rdabt_rf_read_data(UINT32 addr,unsigned char regaddr, unsigned char *data, unsigned char datalen)
{
	unsigned char tempdata;
	//unsigned char i=0;
	
	//datalen=0x01;
	
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr);//chip adress
	
	DELAY(100);
	
	//hal_HstSendEvent(gpio_i2c_TxByte(addr)|0xdd220000);//chip adress
	gpio_i2c_TxByte(regaddr);

	DELAY(100);

	
	gpio_i2c_start();//start
	gpio_i2c_TxByte(addr+1);//chip adress
	
	DELAY(100);
	
	//hal_HstSendEvent(gpio_i2c_TxByte(addr+1)|0xdd330000);//chip adress
	
#if 0	
	for( i=0;i<datalen-1;i++,data++)//data
	{
		gpio_i2c_RxByte(&tempdata, 0);
		*data = (tempdata<<8);	
		gpio_i2c_RxByte(&tempdata, 0);			
		*data |= tempdata;		
	}
	
	gpio_i2c_RxByte(&tempdata, 0);
	*data = (tempdata<<8);	
	gpio_i2c_RxByte(&tempdata, 1);			
	*data |= tempdata;	
#else


	gpio_i2c_RxByte(&tempdata, 1);			
	*data = tempdata;	

#endif

	gpio_i2c_stop();
}

void gpio_i2c_rdabt_core_write_data(UINT32 addr,unsigned int regaddr, unsigned int *data, unsigned char datalen)
{
	unsigned char i=0;    
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr);//chip adress
  //hal_HstSendEvent(gpio_i2c_TxByte(addr)|0xd440000);//chip adress
	
	gpio_i2c_TxByte(regaddr>>24);/////reg adress
	gpio_i2c_TxByte(regaddr>>16);
	gpio_i2c_TxByte(regaddr>>8);	
	gpio_i2c_TxByte(regaddr);
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr);//chip adress
	for(i=0;i<datalen;i++,data++)//data
	{
		gpio_i2c_TxByte(*data>>24);
		gpio_i2c_TxByte(*data>>16);
		gpio_i2c_TxByte(*data>>8);
		gpio_i2c_TxByte(*data);
	}
	gpio_i2c_stop();
}







void gpio_i2c_rdabt_core_read_data(UINT32 addr,unsigned int regaddr, unsigned int *data, unsigned char datalen)
{
	unsigned char tempdata;
	unsigned char i=0;
	
	gpio_i2c_start();///start
	gpio_i2c_TxByte(addr);//chip adress
//	hal_HstSendEvent(gpio_i2c_TxByte(addr)|0xdd550000);//chip adress
	
	gpio_i2c_TxByte(regaddr>>24);
	gpio_i2c_TxByte(regaddr>>16);
	gpio_i2c_TxByte(regaddr>>8);
	gpio_i2c_TxByte(regaddr);
	
	gpio_i2c_start();//start
	gpio_i2c_TxByte(addr+1);//chip adress
//	hal_HstSendEvent(gpio_i2c_TxByte(addr+1)|0xdd660000);//chip adress
	for( i=0;i<datalen-1;i++,data++)//data
	{
		gpio_i2c_RxByte(&tempdata, 0);
		*data  = (tempdata<<24);	
		gpio_i2c_RxByte(&tempdata, 0);			
		*data |= (tempdata<<16);				
		gpio_i2c_RxByte(&tempdata, 0);
		*data |= (tempdata<<8);	
		gpio_i2c_RxByte(&tempdata, 0);			
		*data |= tempdata;		
	}
	gpio_i2c_RxByte(&tempdata, 0);
	*data = (tempdata<<24);	
	gpio_i2c_RxByte(&tempdata, 0);			
	*data |= (tempdata<<16);		
	gpio_i2c_RxByte(&tempdata, 0);
	*data |= (tempdata<<8);	
	gpio_i2c_RxByte(&tempdata, 1);			
	*data |= tempdata;	
	
	gpio_i2c_stop();
}




