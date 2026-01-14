
/**********************************************************************************
*
*
************************************************************************************/

#include "platform_config.h"
#include "iomux.h"
#include "uart.h"
#include "adc.h"

#include "gpio.h"
#include "aon.h"




/*
pclk branch node config for PCLK_OUT PAD

assign reg_pclk_out_div         = reg_0x30C[1:0];

pinmux_core_wrap.v


reg_pclk_out_div=0: 8分频		//default
reg_pclk_out_div=1: 16分频
reg_pclk_out_div=2: 32分频
reg_pclk_out_div=3: 64分频

*/

#define reg_pclk_out_div(n)           (((n) & 0x3) << 0) //2-bit [1:0]//default value n=0

void gecko_branchnode_pclkout_cfg(void)
{
	uint32 TempV;
	
	TempV=SYS_HW32_REG_RD(0x30C);
	TempV|=reg_pclk_out_div(1);
	SYS_HW32_REG_WR(0x30C,TempV);
	
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);	

	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV=SYS_HW32_REG_RD(0x30C);	
	
	TempV=SYS_HW32_REG_RD(0x30C);	
	TempV&=(~reg_pclk_out_div(1));
	SYS_HW32_REG_WR(0x30C,TempV);	

}




//assign reg_pad_func0 = reg_0x300[29:0];
//assign reg_pad_func1 = reg_0x304[29:0];
//assign reg_pad_func2 = reg_0x308[29:0];

void gecko_pinmux_default_config(void)
{
	SYS_HW32_REG_WR(0x300,0x0);
	SYS_HW32_REG_WR(0x304,0x0);
	SYS_HW32_REG_WR(0x308,0x0);
}




void gecko_pinmux_config(Pad_Num_Type pad_num, Pad_Func_Type pad_func)
{
	uint32 pad;
	uint32 iomux_val;
	//char *string;	

	
//	iomux_val=SYS_HW32_REG_RD(0x0);
//	iomux_val=SYS_HW32_REG_RD(0x80);	
//	iomux_val=SYS_HW32_REG_RD(0x84);
//	iomux_val=SYS_HW32_REG_RD(0x88);	
//	iomux_val=SYS_HW32_REG_RD(0x90);
//	iomux_val=SYS_HW32_REG_RD(0x94);
//	iomux_val=SYS_HW32_REG_RD(0x98);
//	iomux_val=SYS_HW32_REG_RD(0xA0);
//	iomux_val=SYS_HW32_REG_RD(0xA4);	
//	iomux_val=SYS_HW32_REG_RD(0xA8);
//	iomux_val=SYS_HW32_REG_RD(0xB0);
//	iomux_val=SYS_HW32_REG_RD(0xB4);	
//	iomux_val=SYS_HW32_REG_RD(0x300);
//	iomux_val=SYS_HW32_REG_RD(0x304);
//	iomux_val=SYS_HW32_REG_RD(0x308);	
	
	
	//char *string;	
	pad=pad_num;
		
	switch (pad)
  {
     case PAD0:
			    //3p3_domain
			    if(pad_func==UART0_OUT)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_UART0_OUT;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_UART0_OUT);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART0_OUT);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}
					else if (pad_func==GPIO_A_0)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_0;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_0);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_0);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
						
					}
					
			    break;
     case PAD1:
			    //3p3_domain		 
			    if(pad_func==UART0_IN)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_UART0_IN;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_UART0_IN);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART0_IN);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					else if (pad_func==GPIO_A_1)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_1;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_1);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_1);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}
				
			    break;				 
     case PAD2:	 
			    break;
     case PAD3:
			    break;	
     case PAD4:
			    break;
     case PAD5:
			    break;
     case PAD6:
			    //3p3_domain
			    if(pad_func==GPIOA_7)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_GPIO_A_7;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_GPIO_A_7);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_GPIO_A_7);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}	
					else if (pad_func==UART0_SOUT)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_UART0_SOUT;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_UART0_SOUT);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_UART2_IN);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
					}
					
			    break;
     case PAD7:
			    //3p3_domain
			    if(pad_func==GPIOB_7)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_GPIO_B_7;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_GPIO_B_7);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_GPIO_B_7);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					else if (pad_func==UART0_SIN)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_UART0_SIN;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_UART0_SIN);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_UART2_OUT);
//						SYS_HW32_REG_WR(0x308,iomux_val);								
					}
			    break;
     case PAD8:
			    //aon_domain
			    if(pad_func==PWM_CH3)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH3;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH3);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART2_OUT);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
						
					}
					else if (pad_func==GPIO_A_4)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_4;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_4);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_4);
//						SYS_HW32_REG_WR(0x308,iomux_val);					
					}
				
			    break;	
     case PAD9:
			    //aon_domain
			    if(pad_func==PWM_CH6)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH6;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH6);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART2_IN);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}
					else if (pad_func==GPIO_A_5)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_5;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_5);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_5);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
					}
					
			    break;
     case PAD10:
			    //aon_domain
			    if(pad_func==PWM_CH7)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH7;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH7);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_I2C0_SDA);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}
					else if (pad_func==GPIO_A_6)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_6;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_6);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_6);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
					}
					
			    break;
     case PAD11:
			    //aon_domain
			    if(pad_func==PWM_CH2)
					{

						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH2;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH2);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_I2C0_SCL);
//						SYS_HW32_REG_WR(0x308,iomux_val);								
						

						
					}
					else if (pad_func==GPIO_A_7)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_A_7;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_A_7);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_7);
//						SYS_HW32_REG_WR(0x308,iomux_val);		
					}
				
			    break;	
     case PAD12:
			    //aon_domain
			    if(pad_func==PCLK_OUT)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PCLK_OUT;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PCLK_OUT);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_PWM_CH5);
//						SYS_HW32_REG_WR(0x308,iomux_val);		
					}
					else if (pad_func==GPIO_B_0)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_0;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_0);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_0);
//						SYS_HW32_REG_WR(0x308,iomux_val);								
					}
			
			    break;
     case PAD13:
			    //aon_domain
			    if(pad_func==UART1_OUT)
					{

						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_UART1_OUT;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_UART1_OUT);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART1_IN);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
						
					
						
					}
					else if (pad_func==GPIO_B_1)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_1;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_1);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_1);
//						SYS_HW32_REG_WR(0x308,iomux_val);		
					}
				
			    break;	
     case PAD14:
			    //aon_domain
			    if(pad_func==UART1_IN)
					{

						
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_UART1_IN;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_UART1_IN);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_UART1_OUT);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
						
						
						#if 0//DEBUG_UATR0_PRINT_LOG
						UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
						UATR0_PRINT_LOG((unsigned char *)("PAD14 AON_IOMUX_SEL_1(FUNC_I2C_SDA)--- = 0x"));
						string=my_itoa(iomux_val);
						UATR0_PRINT_LOG((unsigned char *)(string));
						UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
						#endif
						
					}
					else if (pad_func==GPIO_B_2)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_2;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_2);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_2);
//						SYS_HW32_REG_WR(0x308,iomux_val);								
					}
				
			    break;	
     case PAD15:
			    //aon_domain
			    if(pad_func==SPI_CSN1)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_SPI_CSN1;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_SPI_CSN1);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_PWM_CH4);
//						SYS_HW32_REG_WR(0x308,iomux_val);
					}
					else if (pad_func==GPIO_B_3)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_3;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_3);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_3);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					
			    break;
     case PAD16:
			    //aon_domain
			    if(pad_func==SPI_MOSI)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_SPI_MOSI;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_SPI_MOSI);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_SPI1_DI);
//						SYS_HW32_REG_WR(0x308,iomux_val);
					}
					else if (pad_func==GPIO_B_4)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_4;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_4);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_4);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}	
						
			    break;
     case PAD17:
			    //aon_domain
			    if(pad_func==SPI_CLK)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_SPI_CLK;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_SPI_CLK);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_SPI1_DO);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
					}
					else if (pad_func==GPIO_B_5)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_5;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_5);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_5);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}

			    break;					
     case PAD18:
			    //aon_domain
			    if(pad_func==SPI_MISO)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_SPI_MISO;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_SPI_MISO);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_SPI1_CLK);
//						SYS_HW32_REG_WR(0x308,iomux_val);		
						
					}
					else if (pad_func==GPIO_B_6)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_6;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_6);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_6);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}

			    break;					
     case PAD19:
			    //aon_domain
			    if(pad_func==SPI_CSN2)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_SPI_CSN2;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_SPI_CSN2);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_SPI1_CSN);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					else if (pad_func==GPIO_B_7)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_GPIO_B_7;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_GPIO_B_7);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_B_7);
//						SYS_HW32_REG_WR(0x308,iomux_val);							
					}
					
			    break;


					
     case PAD20:
			    //1p8_domain
			    if(pad_func==GPIO_A_2)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_GPIOA_2;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_GPIOA_2);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_PWM_CH0);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					else if (pad_func==SPI_CSN_1)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_SPI_CS1;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_SPI_CS1);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_2);
//						SYS_HW32_REG_WR(0x308,iomux_val);								
					}
				
			    break;
     case PAD21:
			    //1p8_domain
			    if(pad_func==GPIO_A_3)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH1;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH1);
						SYS_HW32_REG_WR(0x304,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC0_I2C1_SDA);
//						SYS_HW32_REG_WR(0x308,iomux_val);	
					}
					else if (pad_func==SPIMISO)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_SPI_MISO;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_SPI_MISO);
						SYS_HW32_REG_WR(0x300,iomux_val);
//						//clear func2						
//						iomux_val = SYS_HW32_REG_RD(0x308);
//						iomux_val&=(~FUNC1_GPIO_A_3);
//						SYS_HW32_REG_WR(0x308,iomux_val);						
					}
						
			    break;
			
     case PAD22:
			    //1p8_domain
			    if(pad_func==PWM_CH0)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH0;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH0);
						SYS_HW32_REG_WR(0x304,iomux_val);

					}
					else if (pad_func==SPICLK)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_SPI_CLK;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_SPI_CLK);
						SYS_HW32_REG_WR(0x300,iomux_val);
							
					}		 
			    break;
     case PAD23:
			    //1p8_domain
			    if(pad_func==PWM_CH1)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH1;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH1);
						SYS_HW32_REG_WR(0x304,iomux_val);

					}
					else if (pad_func==SPIMOSI)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_SPI_MOSI;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_SPI_MOSI);
						SYS_HW32_REG_WR(0x300,iomux_val);
							
					}		 
			    break;						
     case PAD24:
			    //1p8_domain
			    if(pad_func==PWM_CH4)
					{
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val|=FUNC0_PWM_CH4;
						SYS_HW32_REG_WR(0x300,iomux_val);
						
						//clear func1
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val&=(~FUNC0_PWM_CH4);
						SYS_HW32_REG_WR(0x304,iomux_val);

					}
					else if (pad_func==SPI_CSN_2)
					{
						iomux_val = SYS_HW32_REG_RD(0x304);
						iomux_val|=FUNC1_SPI_CS2;
						SYS_HW32_REG_WR(0x304,iomux_val);	
						
						//clear func0
						iomux_val = SYS_HW32_REG_RD(0x300);
						iomux_val&=(~FUNC1_SPI_CS2);
						SYS_HW32_REG_WR(0x300,iomux_val);
							
					}		 
			    break;	
	   default:
				  break;
	}


	
}









void bsr1901_pullup_pulldown_config(Pad_Num pad_num,Pad_PULL_CFG pull_type)
{
	unsigned int wr_data;
	switch (pad_num)
  {
     case PAD_00:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD00_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD00_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD00_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD00_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;
					
     case PAD_01:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD01_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD01_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD01_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD01_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;	
					
     case PAD_02:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD02_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD02_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD02_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD02_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;
     case PAD_03:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD03_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD03_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD03_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD03_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;
     case PAD_04:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD04_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD04_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD04_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD04_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;

     case PAD_05:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD05_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD05_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD05_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD05_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;		

     case PAD_06:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD06_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD06_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD06_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD06_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;							

     case PAD_07:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD07_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD07_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD07_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD07_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;	
					
     case PAD_08:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD08_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD08_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD08_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD08_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD08_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD08_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;						


		 case PAD_09:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD09_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD09_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD09_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD09_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD09_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD09_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	
					
		 case PAD_10:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD10_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD10_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD10_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD10_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD10_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD10_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;		


		 case PAD_11:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD11_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD11_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD11_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD11_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD11_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD11_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	


		 case PAD_12:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD12_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD12_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD12_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD12_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD12_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD12_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	
					
		 case PAD_13:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD13_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD13_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD13_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD13_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD13_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD13_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;						

		 case PAD_14:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD14_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD14_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD14_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD14_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD14_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD14_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;						

		 case PAD_15:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD15_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD15_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD15_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD15_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD15_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD15_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;		

		 case PAD_16:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD16_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD16_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD16_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD16_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD16_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD16_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;						

		 case PAD_17:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD17_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD17_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD17_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD17_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD17_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD17_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	

		 case PAD_18:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD18_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD18_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD18_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD18_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD18_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD18_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;						


		 case PAD_19:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD19_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD19_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD19_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD19_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD19_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD19_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	

		 case PAD_20:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD20_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD20_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD20_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD20_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD20_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD20_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	

		 case PAD_21:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD21_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD21_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD21_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD21_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
			    else if(pull_type==PAD_HIZ)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD21_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD21_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }						
     break;	



		 case PAD_22:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD22_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD22_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD22_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD22_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;						

					
		 case PAD_23:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD23_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD23_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD23_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD23_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;	
					
		 case PAD_24:
				  if(pull_type==PAD_PULLUP)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data &= (~PAD24_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data &= (~PAD24_PULL);
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }
			    else if(pull_type==PAD_PULLDOWN)
					{			 
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x088);
							wr_data |= PAD24_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x088, wr_data);
						
							wr_data=reg_read(GECKO_AON_BASE_ADDR+0x08C);
							wr_data |= PAD24_PULL;
							reg_write(GECKO_AON_BASE_ADDR+0x08C, wr_data);	
          }	
     break;	
					
		 default:
		 break;					
	}		 

	
}





