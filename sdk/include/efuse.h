
/*
//Efuse State

DO     CELL STATE
H      Programmed
L      Un-programmed(default)

*/



#include "platform_config.h"
#include "bsr1901.h"

#include "iomux.h"



#define  			GECKO_APB_EFFUSE_ADDR								0X40015000


#define EFUSE_CTRL_BASE              0x40015000

#define EFUSE_APB_RW_CFG          *((volatile uint32_t *)(EFUSE_CTRL_BASE + 0x00))
#define EFUSE_APB_RW_ADDR         *((volatile uint32_t *)(EFUSE_CTRL_BASE + 0x04))
#define EFUSE_APB_BIT_DATA        *((volatile uint32_t *)(EFUSE_CTRL_BASE + 0x08))	
#define EFUSE_APB_TIMING          *((volatile uint32_t *)(EFUSE_CTRL_BASE + 0x10))
	




typedef struct
{
    uint8 clk32k_calib;
    uint8 osc_calib;                 
    uint8 adc_calib[8];               /* adc calibration data for 8 channel     */
    uint8 product_id;
} st_EFUSE_config;

st_EFUSE_config 		g_efuse_cfg;






/*
 * reg32_00 Register bits
 */

#define 				EFUSE_APB_RD_TRIGGER               			0
#define 				EFUSE_APB_WR_TRIGGER		                1

/*
* reg32_04 Register ,EFUSE_APB_RW_ADDR[31:0]
*/
//from 0 to 31
#define EFUSE_ADDR(bit)											(1 << bit)

/*
 * reg32_08 Register bits
 */

#define 				EFUSE_APB_RD_DATA               			0
#define 				EFUSE_APB_RD_FINISH		                1
#define 				EFUSE_APB_WR_FINISH		                2






