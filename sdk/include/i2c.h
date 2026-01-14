
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





#if 0

/***********************************************************************
 * I2C module
 **********************************************************************/
#define GECKO_AHB_IIC_CON_REG                    (GECKO_AHB_I2C_BASE + 0x04)
#define GECKO_AHB_IIC_BAUD_REG                   (GECKO_AHB_I2C_BASE + 0x08)
#define GECKO_AHB_IIC_INTRAW_REG                 (GECKO_AHB_I2C_BASE + 0x2C)
#define GECKO_AHB_IIC_INTMASK_REG                (GECKO_AHB_I2C_BASE + 0x28)
#define GECKO_AHB_IIC_INTFIN_REG                 (GECKO_AHB_I2C_BASE + 0x30)
#define GECKO_AHB_IIC_INTEN_REG                  (GECKO_AHB_I2C_BASE + 0x24)
#define GECKO_AHB_IIC_MSTWDATA_REG               (GECKO_AHB_I2C_BASE + 0x1C)
#define GECKO_AHB_IIC_MSTRDATA_REG               (GECKO_AHB_I2C_BASE + 0x10)
#define GECKO_AHB_IIC_SLVWDATA_REG               (GECKO_AHB_I2C_BASE + 0x20)
#define GECKO_AHB_IIC_SLVRDATA_REG               (GECKO_AHB_I2C_BASE + 0x18)
                                                  
#define GECKO_IIC_CONFIGEN_BIT                   0
#define GECKO_IIC_MODE_SELECT_BIT                3
#define GECKO_IIC_ACKEN_BIT                      5
#define GECKO_IIC_MODULEEN_BIT                   7

#define GECKO_IIC_MSTRCVABYTE_BIT                0
#define GECKO_IIC_MSTSNDABYTE_BIT                1
#define GECKO_IIC_MSTSNDNOACQ_BIT                2
#define GECKO_IIC_SLVRCVABYTE_BIT                3
#define GECKO_IIC_SLVSNDABYTE_BIT                4
#define GECKO_IIC_SLVSNDNOACQ_BIT                5
#define GECKO_IIC_RCVERRORRESP_BIT               6
#define GECKO_IIC_TIMEOUT_BIT                    7


#else


/***********************************************************************
 * APB IIC module
 **********************************************************************/
#define GECKO_I2C_EN_REG                          (GECKO_APB_I2C_BASE + 0X00)
#define GECKO_I2C_CONTROL_REG                     (GECKO_APB_I2C_BASE + 0X04)
#define GECKO_I2C_SPEED_REG                       (GECKO_APB_I2C_BASE + 0X08)
#define GECKO_I2C_EE_ID_REG                       (GECKO_APB_I2C_BASE + 0X0c)
#define GECKO_I2C_EE_RDATA_REG                    (GECKO_APB_I2C_BASE + 0X10)
#define GECKO_I2C_DEV_ID_REG                      (GECKO_APB_I2C_BASE + 0X14)
#define GECKO_I2C_SLV_RDATA_REG                   (GECKO_APB_I2C_BASE + 0X18)
#define GECKO_I2C_EE_WDATA_REG                    (GECKO_APB_I2C_BASE + 0X1c)
#define GECKO_I2C_SLV_WDATA_REG                   (GECKO_APB_I2C_BASE + 0X20)
#define GECKO_I2C_INT_EN_REG                      (GECKO_APB_I2C_BASE + 0X24)
#define GECKO_I2C_INT_MSK_REG                     (GECKO_APB_I2C_BASE + 0X28)
#define GECKO_I2C_RAW_STATUS_REG                  (GECKO_APB_I2C_BASE + 0X2c)
#define GECKO_I2C_INT_STATUS_REG                  (GECKO_APB_I2C_BASE + 0X30)
#define GECKO_I2C_CHIP_ADDR_REG                   (GECKO_APB_I2C_BASE + 0X34)

//////////////////////////////////////////////////////////////////
#define GECKO_I2C_SEL				                   		(GECKO_APB_I2C_BASE + 0X38)
#define GECKO_I2C_START_DLY_TIME									(GECKO_APB_I2C_BASE + 0X3c)
#define GECKO_I2C_CMD_DLY_TIME_REG								(GECKO_APB_I2C_BASE + 0X40)
#define GECKO_I2C_WRITE_DLY_TIME_REG							(GECKO_APB_I2C_BASE + 0X44)
#define GECKO_I2C_READ_DLY_TIME_REG								(GECKO_APB_I2C_BASE + 0X48)

#endif




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

