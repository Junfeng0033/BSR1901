#include "dpi_common.h"
#include "platform_config.h"

//typedef unsigned char       UINT8;
//typedef unsigned short      UINT16;
//typedef unsigned int        UINT32;


#define MREAD_8(addr) *((volatile UINT8 *)(addr))
#define MWRITE_8(addr,data) *((volatile UINT8 *)(addr)) = data

#define MREAD_16(addr) *((volatile UINT16 *)(addr))
#define MWRITE_16(addr,data) *((volatile UINT16 *)(addr)) = data

#define MREAD_32(addr) *((volatile UINT32 *)(addr))
#define MWRITE_32(addr,data)  { *((volatile UINT32 *)(addr)) = data; }




__RAM_CODE__ int reg_read(int addr)
{
    int data;
    //ahb_read(addr, 2, &data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Read:  addr = 0x%08x, rdata = 0x%08x\n", addr, data);
    #endif
    //data=MREAD_32(addr);
	  data=*((volatile UINT32 *)(addr));
    return data;
}

int reg_read_8bit(int addr)
{
    int data;
    //ahb_read(addr, 0, &data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Read_8:  addr = 0x%08x, rdata = 0x%02x\n", addr, data);
    #endif
    data=MREAD_8(addr);
    return data;
}

int reg_read_16bit(int addr)
{
    int data;
    //ahb_read(addr, 1, &data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Read_16:  addr = 0x%08x, rdata = 0x%04x\n", addr, data);
    #endif
    data=MREAD_16(addr);
    return data;
}

int reg_read_32bit(int addr)
{
    int data;
    //ahb_read(addr, 2, &data);
        #if FW_TC_DEBUG_USE_PRINTF
    //printf("C_TEST: APB_Read:  addr = 0x%08x, rdata = 0x%08x\n", addr, data);
      #endif
    data=MREAD_32(addr);
    return data;
}

int reg_read_check(int addr, int exp)
{
    int curr;
    curr = reg_read(addr);
    if (curr != exp)
    {
        #if FW_TC_DEBUG_USE_PRINTF
        printf("C_TEST: REG READ Error, addr = 0x%08x, curr = 0x%08x, exp = 0x%08x\n", addr, curr, exp);
        #endif
        return 0;
    }
    else
    {       
        #if FW_TC_DEBUG_USE_PRINTF
        printf("C_TEST: REG READ addr = 0x%08x, Check PASS!\n", addr);
        #endif
        return 1;
    }
}

void poll_reg_equal(int addr, int exp)
{
    int data;
    int done = 0;
    while (!done)
    {
        //ahb_read(addr, &data);
        data=MREAD_32(addr);
        done = data == exp;
    }
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: Polling done:  addr = 0x%08x, exp = 0x%08x\n", addr, exp);
    #endif
    return;
}

void poll_reg_equal_mask(int addr, int mask, int exp)
{
    int data;
    int done = 0;
    while (!done)
    {
        //ahb_read(addr, &data);
        data=MREAD_32(addr);
        done = (data & mask) == exp;
    }
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: Polling done:  addr = 0x%08x, exp = 0x%08x\n", addr, exp);
    #endif
    return;
}

__RAM_CODE__ void reg_write(int addr, int data)
{
    //ahb_write(addr, 2, data);
    #if 0// FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Write: addr = 0x%08x, wdata = 0x%08x\n", addr, data);
    #endif
    //MWRITE_32(addr,data); 
    *((volatile UINT32 *)(addr)) = data; 	
    return;
}

void reg_write_8bit(int addr, int data)
{
    //ahb_write(addr, 0, data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Write_8: addr = 0x%08x, wdata = 0x%02x\n", addr, data);
    #endif
    MWRITE_8(addr,data);
    return;
}

void reg_write_16bit(int addr, int data)
{
    //ahb_write(addr, 1, data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Write_16: addr = 0x%08x, wdata = 0x%04x\n", addr, data);
    #endif
    MWRITE_16(addr,data);    
    return;
}

void reg_write_32bit(int addr, int data)
{
    //ahb_write(addr, 2, data);
    #if FW_TC_DEBUG_USE_PRINTF
    printf("C_TEST: APB_Write: addr = 0x%08x, wdata = 0x%08x\n", addr, data);
    #endif
    MWRITE_32(addr,data);     
    return;
}

//__RAM_CODE__ void delay_1us(unsigned int delay_val)
void delay_1us(unsigned int delay_val)
{


    unsigned int i;
    unsigned int count;
    count = (delay_val<<4);
    for (i=0; i<count; i++)
    {
        __nop();
    }
	
#if 0
    unsigned int i;
    unsigned int count = 20;

    for (i=0; i<delay_val; i++)
    {
		while(count--);
		count = 20;
    }
#endif
		
}

//__RAM_CODE__ void delay_1us(unsigned int delay_val)
__RAM_CODE__ void sram_run_delay(unsigned long delay)
{
    unsigned int i;
    unsigned long count;
    count = (delay<<4);
    for (i=0; i<count; i++)
    {
        __nop();
			  __nop();
			  __nop();
    }
}

