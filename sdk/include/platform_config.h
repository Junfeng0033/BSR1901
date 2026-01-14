
#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H


#define __RAM_CODE__ 		__attribute__((section("ram_code")))






/* Boot Option */


#define  BSR1901_SYS_8MHZ_MODE				1





#ifdef ROM_BOOT  
{  
#define  GEK1108_ROM_BOOT					1
#define  GEK1108_FLASH_BOOT				0
}  
#elif  FLASH_BOOT
{ 
#define  GEK1108_ROM_BOOT					0	
#define  GEK1108_FLASH_BOOT				1  
}  
#endif


typedef unsigned char   	uint8;
typedef unsigned char   	UINT8;
typedef unsigned char   	INT8U;
typedef signed char   		kal_int8;
typedef signed char   		int8;
typedef signed char   		sint8;
typedef signed char   		INT8S;

typedef unsigned char   	uint8_t;
typedef unsigned char   	kal_uint8;
typedef unsigned char   	u8;

typedef unsigned char BYTE;

typedef signed short   	  int16;
typedef unsigned short   	uint16;
typedef unsigned short   	uint16_t;
typedef unsigned short  	kal_uint16;
typedef unsigned short   	u_intHW;
typedef unsigned short   	ushort;

typedef signed int   		  int32;
typedef signed int   		  INT32;
typedef signed int   		  INT32S;
typedef signed int   		  sint32;

typedef unsigned int   		UINT32;
typedef unsigned int      INT32U;
typedef unsigned int   		uint32;
typedef unsigned int   		uint32_t;
typedef signed int   		  int32_t;
typedef unsigned int    	kal_uint32;

typedef unsigned char   	kal_bool; //* kal_bool, true=1, false=0 */

typedef unsigned char   	BOOL;

#define true		1
#define false		0	


#define TRUE		1
#define FALSE		0	


#define PUBLIC						
#define PRIVATE static
#define PROTECTED
#define CONST const
#define INT16 short
#define UINT16 unsigned short
#define INT32 int
#define UINT32 unsigned int
#define INT8 char
#define UINT8 unsigned char
#define CHAR char
#define VOID void
#define VOLATILE volatile	
#define EXPORT extern



typedef volatile unsigned short     REG16;
typedef volatile unsigned int       REG32;



#define  DEBUG_UATR0_PRINT_LOG            1




/* Debug option */
#ifdef WIN32
#define pDEBUG                              1
#else
#define pDEBUG                              0
#endif


#define     __I     volatile                  /*!< defines 'read only' permissions      */
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */



#define SET_BITFIELD(reg, field, value)     (((reg) & ~field ## _MASK) | field(value))
#define GET_BITFIELD(reg, field)            (((reg) & field ## _MASK) >> field ## _SHIFT)



//#define SET_BITFIELD(reg, field, value)     (((reg) & ~field ## _MASK) | field(value))
//#define GET_BITFIELD(reg, field)            (((reg) & field ## _MASK) >> field ## _SHIFT)



/* Error codes returned by API*/
#define NOERROR			    	0x00 
#define RETRY			    		0x01
#define TIMEOUT			    	0x02
#define INVALIDERRORCODE 	0xfe

#define NULL	0


//add by jf
//for message dispatch
struct st_dispatch_msg_t{
    uint16 msg_id;
    uint16 *data;    /* parameter with the message */
};

typedef struct st_dispatch_msg_t  batcase_msg_t;




/* data transport buffer */
typedef struct st_dataBuf {
    uint8  *buf;
    uint8  transport;
    uint16 dataLen;
    uint16 bufLen;
} DataBuf;




struct st_gecko_host_buf_t 
{
    uint8 	*data;
//    uint8 	transport;
    uint16 	len;
    uint16 	totalsize;
    uint16 	flags; 
};


//for lightning host buffer
typedef struct st_gecko_host_buf_t 			gecko_lightning_buf_t;


//for host uart command
typedef struct st_gecko_host_buf_t 			tws_batterycase_uart_buf;


typedef struct
{
    uint8_t *st;
    uint8_t *in;
    uint8_t *out;
    uint8_t *end;
	  //uint8 reserved;
		uint8 *base;
		int front;
		int rear;
		uint8 length;
} gecko_queue_st;



/* Structure Declarations */

/*
typedef struct st_gecko_pwm_cfg{ 
	int8 	 duty_cycle;//0.5
	uint16 freq;//1000hz
	uint8  port;//PWM_OUTPUT_CH8
}gecko_pwm_cfg;
*/


#endif 
