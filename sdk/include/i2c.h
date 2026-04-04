
//#include "gecko1108.h"

//define AHB I2C base address
#define    GECKO_AHB_I2C_BASE                     0X40000000
#define    GECKO_APB_I2C_BASE                     0X40000000




#define I2C_SPEED_100K     												0x40
#define I2C_SPEED_200K     												0x20
#define I2C_SPEED_400K     												0x10


#define I2C_WR_SPEED															I2C_SPEED_400K
#define I2C_TIMEOUT_COUNT  												800
#define EE_PAGE_SIZE															0x40






/* 
	iic verilog


   0x2c  bit2  int_mst_wr_fail  write/read fail  	1: fail 
         bit1  int_mst_tx       tx interrupt  		1: int
         bit0  int_mst_rx				rx interrupt  		1: int

*/



#define GECKO_I2C_READ(REG)                   (*(volatile uint32*)(REG))
#define GECKO_I2C_WRITE(REG,VAL)              (*(volatile uint32*)(REG)) = (uint32)(VAL)
	
//#define GECKO_WRITE_REG(REG,VAL)            (*(volatile uint32*)(REG)) = (uint32)(VAL)


void HW_I2C_Test(void);

void HW_I2C_Init(void);

