#include "platform_config.h"
#include "bsr1901.h"
#include "adc.h"
#include "uart.h"

#include <stdarg.h>
#include <stdio.h>


AdcTypedef adcstruct;






/****************************************************************************

****************************************************************************/
/*
*
*
*ADC_CLKIN
*
*
****************************************************************************/


/*
*ADC_REF_SEL			RSVD<3>					ADC_VREF
*
*    1              X                3.3V
*    0              1               0.5VBAT
*    0              0                1.2V
****************************************************************************/






void adc_delay_us(float dly1us)
{
	volatile uint32 jj=0;
	while(dly1us--)
	{
		jj++;
	}
	
	jj=0;
	
}






//detect charger plug in
uint16 adc_sampling_vin(void) 
{
  //detect charger plug in or not
	uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_0);

	return rawdata;
	//return 0;
}



/*****************************************************************************
*boost always 5V output 
*convert to current,we can know only one earbud in holder or two earbuds in holder 
*
*detect erabuds in compartment or not
*****************************************************************************/
//monitor the loading
uint16 adc_sampling_boost_vout(void) 
{
	return 0;
}




uint16 adc_sampling_batterycase(void) 
{
	uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_1);
	
	return rawdata;
	//return 0;
}



uint16 adc_sampling_ntc_sensor(void) 
{
	uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_6);
	return rawdata;
	//return 0;
}



//maybe for left earbud vbat
uint16 adc_sampling_reserved1(void) 
{
  //detect the current of earbud between power and ground
	return 0;
}


//maybe for right earbud vbat
uint16 adc_sampling_reserved2(void) 
{
	return 0;
}






void ADC_Init(void)
{

	hw_gpadc->ctrl = GPADC_CH31_EN;//enable channel 15, the first channel
	
	adcstruct.ad_channel_select = 0;
	
	hw_gpadc->ctrl |= ADC_EN; //bit12 ADC_EN=1
	
	hw_gpadc->ctrl |= ADC_IN_BUFF_EN;//ADC in buffer enable
		
	hw_gpadc->ctrl &= ~GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
}









#if 0

void Get_Advalue_Func(void)
{
#if 1//def DEBUG_EN	
	char *string;	
#endif	
	if((hw_gpadc->status & GPADC_EOC))   //转换完成标记
	{
		switch(adcstruct.ad_channel_select)
		{
			case 0:
					adcstruct.advalue_buf[0] = hw_gpadc->data_ch0;
			    hw_gpadc->ctrl = GPADC_CH1_EN;
					adcstruct.ad_channel_select = 1;
				break;
			
			case 1:
					adcstruct.advalue_buf[1] = hw_gpadc->data_ch1;
					hw_gpadc->ctrl = GPADC_CH2_EN;
					adcstruct.ad_channel_select = 2;
				break;
			
			case 2:
					adcstruct.advalue_buf[2] = hw_gpadc->data_ch2;
					hw_gpadc->ctrl = GPADC_CH3_EN;
					adcstruct.ad_channel_select = 3;

				break;
			
			case 3:
					adcstruct.advalue_buf[3] = hw_gpadc->data_ch3;
					hw_gpadc->ctrl = GPADC_CH4_EN;
					adcstruct.ad_channel_select = 4;

				break;
			
			case 4:
					adcstruct.advalue_buf[4] = hw_gpadc->data_ch4;
					hw_gpadc->ctrl = GPADC_CH5_EN;
			    //channel 5 ---> VDD5V detect
					hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
					adcstruct.ad_channel_select = 5;
				break;
			
			case 5:
					adcstruct.advalue_buf[5] = hw_gpadc->data_ch5;
			
					#if DEBUG_UATR0_PRINT_LOG						
					UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_2 --- data_ch2 data:"));
					string=my_itoa(hw_gpadc->data_ch5);
					UATR0_PRINT_LOG((unsigned char *)(string));
					UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
					#endif					
			
					hw_gpadc->ctrl = GPADC_CH6_EN;
					adcstruct.ad_channel_select = 6;
			
			    hw_gpadc->ctrl &= ~GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
			
				break;
			
			case 6:
					adcstruct.advalue_buf[6] = hw_gpadc->data_ch6;
					hw_gpadc->ctrl = GPADC_CH7_EN;
					adcstruct.ad_channel_select = 7;
				break;
			
			case 7:
					adcstruct.advalue_buf[7] = hw_gpadc->data_ch7;
					hw_gpadc->ctrl = GPADC_CH0_EN;
					adcstruct.ad_channel_select = 0;
				break;
			
			default:
				break;
		}
#ifdef DEBUG_EN	
		UATR0_PRINT_LOG((unsigned char *)("adcchannel=0x"));
		if(adcstruct.ad_channel_select>0)
		{
			string=my_itoa(adcstruct.ad_channel_select-1);
		}
		else
		{
			string=my_itoa(7);
		}
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("---"));
		UATR0_PRINT_LOG((unsigned char *)("adcvalue=0x"));
		if(adcstruct.ad_channel_select>0)
		{
			string=my_itoa(adcstruct.advalue_buf[adcstruct.ad_channel_select-1]);
		}
		else
		{
			string=my_itoa(adcstruct.advalue_buf[7]);
		}
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\n"));
#endif
		hw_gpadc->ctrl |= ADC_EN;
//		hw_gpadc->ctrl |= GPADC_START;    //start convert
	}
}

#endif









#if 0
void ADC_Data_PRINT(unsigned int datavalue)
{
		char *string;				
		UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_0 --- data_ch0 data:"));
		string=my_itoa(datavalue);
		UATR0_PRINT_LOG((unsigned char *)(string));
		UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
}
#endif





uint16 GeckoGpadcGetRawData(GPADC_CHAN_T channel)
{
	
		volatile unsigned int tmp=0,datavalue = 0;

	  //hw_gpadc->ctrl |= ADC_EN; //ADC_EN=1	
	  //hw_gpadc->ctrl &= ~ADC_EN; //ADC_EN=0
		
		hw_gpadc->ctrl |= ADC_IN_BUFF_EN;//ADC in buffer enable
	
	
		hw_gpadc->ctrl =(1<<channel) | ADC_EN;	
	  hw_gpadc->ctrl =(1<<channel);	
	  hw_gpadc->ctrl =(1<<channel) | ADC_EN;	
	
	
	
    if(channel==8)
		{
		hw_gpadc->ctrl = GPADC_CH8_EN | ADC_EN;
		}
		else if(channel==9)
		{
		hw_gpadc->ctrl = GPADC_CH9_EN | ADC_EN;
		}

		
		
		adc_delay_us(10);
		
		datavalue=hw_gpadc->status;
		
		while(!(hw_gpadc->status & GPADC_EOC))
		{
				if(tmp++>3000)
				{
					datavalue=hw_gpadc->ctrl;				
					printf("\r\n hw_gpadc->ctrl ADC_EN--- bit12 = %d",datavalue);				
					datavalue=hw_gpadc->status;				
					printf("\r\n hw_gpadc->status GPADC_EOC--- bit16 = %d",datavalue);	
					return 0xff;//timeout exit loop
					//break;
				}
		}
		
		
	  tmp=0;
		
		//conversion done, read GPADC data
		switch (channel)
		{
				case GPADC_CHAN_0:

						datavalue = hw_gpadc->data_ch0;				
				    printf("\r\n ADC GPADC_CHAN_0 --- data_ch0 data = %d",datavalue);						
						break;
				
				case GPADC_CHAN_1:

						datavalue = hw_gpadc->data_ch1;				    
				    printf("\r\n ADC GPADC_CHAN_1 --- data_ch1 data = %d",datavalue);						
						break;
				
				case GPADC_CHAN_2:

						datavalue = hw_gpadc->data_ch2;				
				    printf("\r\n ADC GPADC_CHAN_2 --- data_ch2 data = %d",datavalue);						
						break;
				
				case GPADC_CHAN_3:

						datavalue = hw_gpadc->data_ch3;
				    printf("\r\n ADC GPADC_CHAN_3 --- data_ch3 data = %d",datavalue);	
						break;
				
				case GPADC_CHAN_4:

						datavalue = hw_gpadc->data_ch4;
				    printf("\r\n ADC GPADC_CHAN_4 --- data_ch4 data = %d",datavalue);	
						break;
				
				case GPADC_CHAN_5:

						hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
						adc_delay_us(20);
						datavalue = hw_gpadc->data_ch5;
				    printf("\r\n ADC GPADC_CHAN_5 --- data_ch5 data = %d",datavalue);						
						break;
				
				case GPADC_CHAN_6:

						datavalue = hw_gpadc->data_ch6;
				    printf("\r\n ADC GPADC_CHAN_6 --- data_ch6 data = %d",datavalue);						
						break;
				
				case GPADC_CHAN_7:

						datavalue = hw_gpadc->data_ch7;
				    printf("\r\n ADC GPADC_CHAN_7 --- data_ch7 = %d",datavalue);
						break;
				
//2022-09-04
				case GPADC_CHAN_8:

						hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
						datavalue = hw_gpadc->data_ch8;
				    printf("\r\n ADC GPADC_CHAN_8 --- data_ch8 data = %d",datavalue);				
						break;
				
//2022-09-04
				case GPADC_CHAN_9:

						hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
						datavalue = hw_gpadc->data_ch9;
				    printf("\r\n ADC GPADC_CHAN_9 --- data_ch9 data = %d",datavalue);
						break;				
				default:
						break;
		}
		
		hw_gpadc->ctrl =0;//disable all channels
		
		return datavalue;
}














/*
always @ ( posedge pclk_mod or negedge presetn ) begin
  if ( !presetn )
        r_status[p_eoc] <= 1'b0;
  //Roy@2022-09-01. Change bit-loc from bit[8] to bit[16]
  //else if ( register_write_h && (paddr == at_ctrl) && pwdata[8] == 1'b1)
  else if ( register_write_h && (paddr == at_ctrl) && pwdata[16] == 1'b1) //at_ctrl       : r_reg_00 <= pwdata   ;//assign adc_en      = r_reg_00[16];
        r_status[p_eoc] <= 1'b0;
  else if ( eoc_pulse )
        r_status[p_eoc] <= 1'b1;
end
*/


uint16 Get_Vbat_Voltage(void)
{
	  uint16 datavalue;
	  int adc_convert;
	
	  volatile unsigned int tmp=0;
	
		hw_gpadc->ctrl = GPADC_CH5_EN | ADC_EN | ADC_IN_BUFF_EN;	
	
		hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference	

		adc_delay_us(10);	
	
	  while(!(hw_gpadc->status & GPADC_EOC))
		{
			if(tmp++>3000)
			{
			return 0x0;//timeout exit loop
			}
		}
		
		
    tmp=0;
		
		adc_delay_us(20);

		datavalue = hw_gpadc->data_ch5;
		
		hw_gpadc->ctrl =0;//disable all channels	


		adc_convert=(datavalue*1000)/4095;//Expand 1000 times
		//printf("\r\nnGet_Vbat_Voltage adc_convert___1 = %d",adc_convert);		
		adc_convert=adc_convert*(3.3*2);//3.3V x 2 = 6.6V
		//printf("\r\nnGet_Vbat_Voltage adc_convert___2 = %d",adc_convert);			
		
		return adc_convert;
}





//ADC7
//detect battery charging current 
uint16 adc_buck_curr_read(void) 
{
	//char *string;
	uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_7);
	
	uint16 mv=(rawdata*1.2)/1023;
	
	printf("\r\n adc_buck_curr_read  mv = %d",mv);	

	return mv;	
	//return 0x0;
}







//ADC4
//detect battery voltage 
uint16 adc_vbatvalue_read(void) 
{
	//char *string;
	uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_4);
	//uint16 mv = GeckoGpadcRawData2Volt(rawdata);
	uint16 mv=(rawdata*1.2)/1023;
	
	return mv;	
	//return 0x0;
}




