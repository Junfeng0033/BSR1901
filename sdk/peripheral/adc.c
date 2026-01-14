#include "platform_config.h"
#include "bsr1901.h"
#include "adc.h"
#include "uart.h"
/****************************************************************************
//
//PIN MAP
//
//ch_0				adc channel_0						AC_IN
//ch_1				adc channel_1						VBAT
//ch_2				adc channel_2						REAR_DET
//ch_3				adc channel_3						LEAE_DET
//ch_4				adc channel_4						QI_DET(QI_5VOUT)
//ch_5				adc channel_5						VDD5V(boost_5v_output)
//ch_6				adc channel_6						TEMP_SEN
//ch_7				adc channel_7						ISEN_BATT
//ch_8
//ch_9
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




#define ADC_10BIT_RANGE             0x3FF
#define ADC_12BIT_RANGE             0xFFF

#define ADC_RANGE    ADC_12BIT_RANGE

AdcTypedef adcstruct;

//voltage = data * 3.3f / 4096.0f;

//ADC Calibration

//__IO uint16_t VREF_BG_CAL=0;
//VREF_BG_CAL=*(__IO uint16_t *)(0x1FF80078);//1.2V adc value


//store 3 groups calibration value:
//0v---------0xXXXX XXXX address
//1.2v-------0x1FF80078  address
//3.0v-------0xXXXX XXXX address


//#define GPADC_CH_INGN(n)            (((n)&0x3)<<9)
//#define GPADC_REF_SEL               (1<<11)
//#define GPADC_CK_DIV(n)             (((n)&0x7)<<12)


//#define 			GECKO_ADC_CLK_DIV(n)        (((n) & 0x7) << 0)
//#define 			GECKO_ADC_EN     						(1 << 3)
//#define 			GECKO_ADC_REF_SEL     			(1 << 4)
//#define 			GECKO_ADC_INBUF_EN     			(1 << 5)


//#define 			GECKO_ADC_CHAN_INGN(n)     	(((n) & 0x3) << 6)
//#define 			GECKO_ADC_CHAN_SEL(n)     	(((n) & 0xF) << 9)



// Battery voltage = gpadc voltage * 4
// adc rate 0~2v(10bit) 
// Battery_voltage:Adc_rate = 4:1
#define GPADC_MVOLT_A   800
#define GPADC_MVOLT_B   1050

#define GPADC_MVOLT_STD   1024

#define GPADC_CALIB_DEFAULT_A   428
#define GPADC_CALIB_DEFAULT_B   565




//uint16 batterycase_voltage;
//static uint16 GeckoGpadcRawData2Volt(uint16 gpadcVal);
//uint16  GeckoGpadcGetRawData(GPADC_CHAN_T channel);
//uint16  GeckoGpadcGetRawData(uint8 channel);


typedef enum
{
	GECKO_ADC_CHAN_BATTERY=0x01,
	GECKO_ADC_CHAN_CHAN_0,
	GECKO_ADC_CHAN_CHAN_1,
	GECKO_ADC_CHAN_CHAN_2,
	GECKO_ADC_CHAN_CHAN_3,
	GECKO_ADC_CHAN_CHAN_4,	
	GECKO_ADC_CHAN_CHAN_5,	
	GECKO_ADC_CHAN_CHAN_6,
	GECKO_ADC_CHAN_CHAN_7,
	GECKO_ADC_CHAN_CHAN_8,
	GECKO_ADC_CHAN_CHAN_9,	
	GECKO_ADC_BAD_VALUE,
	GECKO_ADC_CHAN_QTY,
} HW_GPADC_CHAN_T;




void adc_delay_us(float dly1us)
{
	volatile uint32 jj=0;
	//for(ii=0;ii<units_of_u_secs;ii++)
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
	//uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_1);
	//uint16 mv = GeckoGpadcRawData2Volt(rawdata);
	return 0;
}



/*****************************************************************************
*
*boost always 5V output 
*convert to current,we can know only one earbud in holder or two earbuds in holder 
*
*sould detect erabuds in compartment or not
//monitor the loading
*****************************************************************************/
uint16 adc_sampling_boost_vout(void) 
{
	return 0;
}




uint16 adc_sampling_batterycase(void) 
{
	//uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_1);
	
	//uint16 mv = GeckoGpadcRawData2Volt(rawdata);
	//return rawdata;
	return 0;
}

uint16 adc_sampling_ntc_sensor(void) 
{
	//uint16 rawdata=GeckoGpadcGetRawData(GPADC_CHAN_6);
	
	//uint16 mv = GeckoGpadcRawData2Volt(rawdata);
	
	return 0;
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
//	hw_gpadc->thrshd0 = 0x1f0;	
//	hw_gpadc->thrshd1 = 0x1f1;		
//	hw_gpadc->thrshd2 = 0x1f2;
//	hw_gpadc->thrshd3 = 0x1f3;	
//	hw_gpadc->thrshd4 = 0x1f4;		
//	hw_gpadc->thrshd5 = 0x1f5;	
//	hw_gpadc->thrshd6 = 0x1f6;		
//	hw_gpadc->thrshd7 = 0x1f7;
//	hw_gpadc->irq_mask = 0x0000;
	hw_gpadc->ctrl = GPADC_CH31_EN;//enable channel 15, the first channel
	adcstruct.ad_channel_select = 0;
	hw_gpadc->ctrl |= ADC_EN; //bit12 ADC_EN=1
	
	hw_gpadc->ctrl |= ADC_IN_BUFF_EN;//ADC in buffer enable
	
	//hw_gpadc->ctrl = 0;//add 2022-2-16
	
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




void gecko_handle_adc_channels(HW_GPADC_CHAN_T channel)
{
			//uint8 channel;
			//uint16 mv = GeckoGpadcRawData2Volt(GeckoGpadcGetRawData(channel));
			//uint16 mv = GeckoGpadcRawData2Volt(GeckoGpadcGetRawData(GPADC_CHAN_1));
			switch (channel) {
			case GECKO_ADC_CHAN_BATTERY:
				adc_sampling_batterycase();
				break;
			case GECKO_ADC_CHAN_CHAN_0:
				adc_sampling_ntc_sensor();
				break;
			case GECKO_ADC_CHAN_CHAN_1:
				adc_sampling_boost_vout();
				break;
			case GECKO_ADC_CHAN_CHAN_2:
			case GECKO_ADC_CHAN_CHAN_3:
				adc_sampling_vin();
				break;
			case GECKO_ADC_CHAN_CHAN_4:
				adc_sampling_reserved1();
				break;
			case GECKO_ADC_CHAN_CHAN_5: 
				adc_sampling_reserved2();
				break;
			case GECKO_ADC_BAD_VALUE:
				//bad value;
				break;			
			default:
				break;
		}
	
}




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
//uint16 GeckoGpadcGetRawData(uint8 channel)
//uint16 adc_sampling_test(void) 
//uint16 adc_sampling_test(void)
//uint16 adc_sampling_test(GPADC_CHAN_T channel)	
{
	
		volatile UINT32 enabledMask = 0;
    //volatile unsigned int* dataAddress = NULL;
		volatile unsigned int tmp=0,datavalue = 0;
	  //uint8 channel;
		//char *string;	

	  //hw_gpadc->ctrl |= ADC_EN; //ADC_EN=1
	
	  //hw_gpadc->ctrl &= ~ADC_EN; //ADC_EN=0
		
		hw_gpadc->ctrl |= ADC_IN_BUFF_EN;//ADC in buffer enable
	
	
	
//for(channel=0;channel<10;channel++)	
{	
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
//		UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
//		UATR0_PRINT_LOG((unsigned char *)("hw_gpadc->status GPADC_EOC--- bit16:"));
//		string=my_itoa(datavalue);
//		UATR0_PRINT_LOG((unsigned char *)(string));
//		UATR0_PRINT_LOG((unsigned char *)("\r\n"));		
			
		while(!(hw_gpadc->status & GPADC_EOC))
		{
			//if (hw_gpadc->status & GPADC_EOC) = 0 , empty loop here ,wait here, wait,wait
			
			if(tmp++>3000)
			{
			//hw_gpadc->ctrl =0;//disable all channels
			datavalue=hw_gpadc->ctrl;
//			UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
//			UATR0_PRINT_LOG((unsigned char *)("hw_gpadc->ctrl ADC_EN--- bit12:"));
//			string=my_itoa(datavalue);
//			UATR0_PRINT_LOG((unsigned char *)(string));
//			UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
				
			datavalue=hw_gpadc->status;
//			UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
//			UATR0_PRINT_LOG((unsigned char *)("hw_gpadc->status GPADC_EOC--- bit16:"));
//			string=my_itoa(datavalue);
//			UATR0_PRINT_LOG((unsigned char *)(string));
//			UATR0_PRINT_LOG((unsigned char *)("\r\n"));					
			return 0xff;//timeout exit loop
				//break;
			}
		}
		
	  tmp=0;

    {   // Conversion is done, read the GPADC
        switch (channel)
        {
            case GPADC_CHAN_0:
                enabledMask = GPADC_CH0_EN;
                //dataAddress = &hw_gpadc->data_ch0;
								datavalue = hw_gpadc->data_ch0;
							  //char *string;
						    #if 0//DEBUG_UATR0_PRINT_LOG
								UATR0_PRINT_LOG((unsigned char *)("\r\n"));						
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_0 --- data_ch0 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
                #endif						
                break;
            case GPADC_CHAN_1:
                enabledMask = GPADC_CH1_EN;
                //dataAddress = &hw_gpadc->data_ch1;
								datavalue = hw_gpadc->data_ch1;
							  //char *string;
								#if 0//DEBUG_UATR0_PRINT_LOG						
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_1 --- data_ch1 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
                #endif						
                break;
            case GPADC_CHAN_2:
                enabledMask = GPADC_CH2_EN;
                //dataAddress = &hw_gpadc->data_ch2;
								datavalue = hw_gpadc->data_ch2;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG						
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_2 --- data_ch2 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
                #endif						
                break;
            case GPADC_CHAN_3:
                enabledMask = GPADC_CH3_EN;
                //dataAddress = &hw_gpadc->data_ch3;
								datavalue = hw_gpadc->data_ch3;
							  //char *string;
								#if 0//DEBUG_UATR0_PRINT_LOG						
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_3 --- data_ch3 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
                #endif						
                break;
						
            case GPADC_CHAN_4:
                enabledMask = GPADC_CH4_EN;
                //dataAddress = &hw_gpadc->data_ch0;
								datavalue = hw_gpadc->data_ch4;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG						
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_4 --- data_ch4 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
								#endif
                break;
            case GPADC_CHAN_5:
                enabledMask = GPADC_CH5_EN;
						    hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
						    adc_delay_us(20);
                //dataAddress = &hw_gpadc->data_ch1;
								datavalue = hw_gpadc->data_ch5;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG							
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_5 --- data_ch5 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
                #endif						
                break;
            case GPADC_CHAN_6:
                enabledMask = GPADC_CH6_EN;
                //dataAddress = &hw_gpadc->data_ch2;
								datavalue = hw_gpadc->data_ch6;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG							
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_6 --- data_ch6 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));	
                #endif						
                break;
            case GPADC_CHAN_7:
                enabledMask = GPADC_CH7_EN;
                //dataAddress = &hw_gpadc->data_ch3;
								datavalue = hw_gpadc->data_ch7;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG							
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_7 --- data_ch7 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
                #endif						
                break;
						
//2022-09-04
            case GPADC_CHAN_8:
                enabledMask = GPADC_CH8_EN;
								hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
								datavalue = hw_gpadc->data_ch8;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG							
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_8 --- data_ch8 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
                #endif						
                break;	
//2022-09-04
            case GPADC_CHAN_9:
                enabledMask = GPADC_CH8_EN;
								hw_gpadc->ctrl |= GPADC_REF_SEL;//0:1.2V reference;1:3.3V reference
								datavalue = hw_gpadc->data_ch9;
							  //char *string;	
                #if 0//DEBUG_UATR0_PRINT_LOG							
							  UATR0_PRINT_LOG((unsigned char *)("ADC GPADC_CHAN_9 --- data_ch9 data:"));
							  string=my_itoa(datavalue);
							  UATR0_PRINT_LOG((unsigned char *)(string));
							  UATR0_PRINT_LOG((unsigned char *)("\r\n"));
                #endif						
                break;	

						
            default:
								break;
        }
				
				hw_gpadc->ctrl =0;//disable all channels
				
		}
}		
		
		return datavalue;
}



// =============================================================================
// GpadcGet
// -----------------------------------------------------------------------------
/// Return the more up-to-date value measure for the given channel in mV. 
/// If the conversion is not finished yet, #GPADC_BAD_VALUE is returned.
///
/// Once the first measured has been correctly done, there will always
/// be a value ready to read for all the enabled channel.
///
/// @param channel Channel whose more up to date measured value
/// will be returned.
/// @return Measured value, in mV, or #GPADC_BAD_VALUE if no result is 
/// available yet.
// =============================================================================
uint16 GpadcGet(GPADC_CHAN_T channel)
{
    //uint16 mv = GeckoGpadcRawData2Volt(GeckoGpadcGetRawData(channel));
    return 0;//(mv);
}

// just for the factory mode now
// note: the battery volt is double of the return value.
//       0xffff mean the bad value

uint16 GetGpadcValue(GECKO_GPADC_CHAN_T channel)
{
//GetBatteryGpadcChannel();
//GetVINGpadcChannel();
//GetNTCGpadcChannel();
//GetBoostGpadcChannel();
 return 0;
}

void adc_qi_vc_read()
{
	
}

void Update_ADC_Hdlr()
{
    //uint8 i=0, j=0;
    //int8 buffer[32];
    //uint16 adc_value;
		
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
		

//	  //write reg_00 bit16 to clear r_status[p_eoc] bit
//	  hw_gpadc->ctrl &= ~ADC_EN; //ADC_EN=0	
//		hw_gpadc->ctrl |= ADC_EN; //ADC_EN=1
//	  hw_gpadc->ctrl &= ~ADC_EN; //ADC_EN=0	
		
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
