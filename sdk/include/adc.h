
#include "platform_config.h"


#define ADC_12BIT_RANGE             0xFFF

#define ADC_RANGE    ADC_12BIT_RANGE


//define APB ADC base address
#define	 	 GECKO_APB_ADC_BASE		    							0x40016000
#define 	 REG_GPADC_BASE              						0x40016000

extern int hal_analogif_reg_read(unsigned short reg, unsigned short *val);
extern int hal_analogif_reg_write(unsigned short reg, unsigned short val);



typedef volatile struct
{
    REG32                          ctrl;                         //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          data;                         //0x00000008
    REG32                          data_ch0;                     //0x0000000C
    REG32                          data_ch1;                     //0x00000010
    REG32                          data_ch2;                     //0x00000014
    REG32                          data_ch3;                     //0x00000018	
    REG32                          data_ch4;                     //0x0000001C
    REG32                          data_ch5;                     //0x00000020
    REG32                          data_ch6;                     //0x00000024
    REG32                          data_ch7;                     //0x00000028
	
    REG32                          irq_mask;                     //0x0000002C	
    REG32                          irq_cause;                    //0x00000030	
    REG32                          irq_clr;                      //0x00000034
	
    REG32                          thrshd0;                      //0x00000038
    REG32                          thrshd1;                      //0x0000003C
    REG32                          thrshd2;                      //0x00000040
    REG32                          thrshd3;                      //0x00000044
    REG32                          thrshd4;                      //0x00000048
    REG32                          thrshd5;                      //0x0000004C
    REG32                          thrshd6;                      //0x00000050
    REG32                          thrshd7;                      //0x00000054
//2022-09-04
//ADC Data， Confirm data_ch8 and ch9 DATA register
    REG32                          data_ch8;                     //0x00000058
    REG32                          data_ch9;                     //0x0000005C

} HWP_GPADC_T;

#define hw_gpadc                   ((HWP_GPADC_T*) GECKO_APB_ADC_BASE)





#define 		HWgpadc_reg_read(reg,val)  											  hal_analogif_reg_read(reg,val)
#define 		HWgpadc_reg_write(reg,val) 											  hal_analogif_reg_write(reg,val)









/*
always@(*) begin
 case(gpadc_ch_en)
   10'b00_0000_0001: r_gpadc_sel = 4'd0;//channel 1
   10'b00_0000_0010: r_gpadc_sel = 4'd1;//channel 2
   10'b00_0000_0100: r_gpadc_sel = 4'd2;//channel 3
   10'b00_0000_1000: r_gpadc_sel = 4'd3;//channel 4
   10'b00_0001_0000: r_gpadc_sel = 4'd4;//channel 5
   10'b00_0010_0000: r_gpadc_sel = 4'd5;//channel 6
   10'b00_0100_0000: r_gpadc_sel = 4'd6;//channel 7
   10'b00_1000_0000: r_gpadc_sel = 4'd7;//channel 8
   10'b01_0000_0000: r_gpadc_sel = 4'd8;//channel 9
   10'b10_0000_0000: r_gpadc_sel = 4'd9;//channel 10 //channel 10 differential adc for fuel guage
   default     	   : r_gpadc_sel = 4'd15;
 endcase
end
*/


//ctrl
//0x00 register
#define GPADC_CH0_EN                (1<<0)
#define GPADC_CH1_EN                (1<<1)
#define GPADC_CH2_EN                (1<<2)
#define GPADC_CH3_EN                (1<<3)
#define GPADC_CH4_EN                (1<<4)
#define GPADC_CH5_EN                (1<<5)
#define GPADC_CH6_EN                (1<<6)
#define GPADC_CH7_EN                (1<<7)
//add ZJF 2022-09-01
#define GPADC_CH8_EN                (1<<8)
#define GPADC_CH9_EN                (1<<9)

#define GPADC_CH31_EN               ((uint32)1<<31) //default,none channel select


#define GPADC_CH_INGN(n)            (((n)&0x3)<<10) //00=bypass;01=1/2 input;10=1/4 input
#define GPADC_REF_SEL               (1<<12)

#define GPADC_CK_DIV(n)             (((n)&0x3)<<13)
#define ADC_IN_BUFF_EN              (1<<15)

#define ADC_EN                 			((uint32)1<<16) //New: 1109 change it to bit16 --->2022-09-01



//#define GPADC_EOC                   (1<<16)//20210210 JunfengZhou
//#define GPADC_BUSY                  (1<<17)

#define GPADC_EOC                   (1<<20)//New:1109 change it to bit20 --->2022-09-01


#define GPADC_BUSY_RSV              (1<<22)



//data
#define GPADC_DATA(n)               (((n)&0xFFF)<<0)









// =============================================================================
// GPADC_CHAN_T
// -----------------------------------------------------------------------------
// Channel selection.
// =============================================================================
typedef enum
{
    GPADC_CHAN_0,//AC_IN
    GPADC_CHAN_1,//VBAT
    GPADC_CHAN_2,//REAR_DET
    GPADC_CHAN_3,//LEAR_DET
    GPADC_CHAN_4,//QI_DET
    GPADC_CHAN_5,//VDD5V
    GPADC_CHAN_6,//TEMP_SEN
    GPADC_CHAN_7,//ISEN_BATT	
    GPADC_CHAN_8,
    GPADC_CHAN_9,		
    GPADC_CHAN_QTY,	
	
} GPADC_CHAN_T;





typedef uint16 HAL_ANA_GPADC_MV_T;




// Battery voltage = gpadc voltage * 3
#define HAL_ANA_GPADC_MVOLT_A   1133
#define HAL_ANA_GPADC_MVOLT_B   1400


#define GPADC_BAD_VALUE  0xFFFF



typedef struct{
	uint8_t ad_channel_select; //ADC通道索引    
	uint16_t advalue_buf[8];   //存放各通道ADC转换后的值	
}AdcTypedef;






//voltage = data * 3.3f / 4096.0f;

//ADC Calibration

//__IO uint16_t VREF_BG_CAL=0;
//VREF_BG_CAL=*(__IO uint16_t *)(0x1FF80078);//1.2V adc value


//store 3 groups calibration value:
//0v---------0xXXXX XXXX address
//1.2v-------0x1FF80078  address
//3.3v-------0xXXXX XXXX address


//#define GPADC_CH_INGN(n)            (((n)&0x3)<<9)
//#define GPADC_REF_SEL               (1<<11)
//#define GPADC_CK_DIV(n)             (((n)&0x7)<<12)


//#define 			GECKO_ADC_CLK_DIV(n)        (((n) & 0x7) << 0)
//#define 			GECKO_ADC_EN     						(1 << 3)
//#define 			GECKO_ADC_REF_SEL     			(1 << 4)
//#define 			GECKO_ADC_INBUF_EN     			(1 << 5)


//#define 			GECKO_ADC_CHAN_INGN(n)     	(((n) & 0x3) << 6)
//#define 			GECKO_ADC_CHAN_SEL(n)     	(((n) & 0xF) << 9)





//uint16  GeckoGpadcGetRawData(GPADC_CHAN_T channel);
//uint16  GeckoGpadcGetRawData(uint8 channel);






void ADC_Init(void);

//uint16 GeckoGpadcGetRawData(uint8 channel);


uint16 GeckoGpadcGetRawData(GPADC_CHAN_T channel);


void Get_Advalue_Func(void);


uint16 adc_vbatvalue_read(void);

uint16 adc_buck_curr_read(void);


	
