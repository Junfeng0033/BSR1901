/******************************************************************************************************************

BLK1                   Flash encryption key                              
  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
BLK2                   Secure boot key                                   
  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 
BLK3                   Variable Block 3                                  
  = 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 R/W 

Identity fuses:
MAC                    MAC Address                                       = 30:ae:a4:c3:86:94 (CRC 99 OK) R/W 
CHIP_VER_REV1          Silicon Revision 1                                = 1 R/W (0x1)
CHIP_VERSION           Reserved for future chip versions                 = 2 R/W (0x2)
CHIP_PACKAGE           Chip package identifier                           = 1 R/W (0x1)

Calibration fuses:
BLK3_PART_RESERVE      BLOCK3 partially served for ADC calibration data  = 0 R/W (0x0)
ADC_VREF               Voltage reference calibration                     = 1093 R/W (0x11)

EFUSE block 0:
00000000 c40042xx xxxxxxxx 00000000 00000033 00000000 00000000
EFUSE block 1:
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
EFUSE block 2:
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
EFUSE block 3:
00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

//32*8 bit  8 blocks, 32bit per block

******************************************************************************************************************/


#include "efuse.h"





volatile uint32 efuse_data_buf;




/*
//Efuse State

DO     CELL STATE
H      Programmed
L      Un-programmed(default)

*/



int gecko_efuse_read(void) {
    efuse_data_buf = 0; // Initialize the data buffer	
    //SYS_HW32_REG_WR(0x310,0x0);	
	  reg_write(0x40020000+0x40,0x8888FFFF);
    for (uint32_t bit_pos = 0; bit_pos < 32; bit_pos++) {
        int timeout = 0;		
        // Set the bit address to be read (directly use the address value from 0 - 31)			
        //EFUSE_APB_RW_ADDR = bit_pos;
			  EFUSE_APB_RW_ADDR = (1<<bit_pos);

        // Trigger the read operation
        EFUSE_APB_RW_CFG = (1 << EFUSE_APB_RD_TRIGGER);

        // Wait for the read to complete or timeout
        while (!(EFUSE_APB_BIT_DATA & (1 << EFUSE_APB_RD_FINISH))) {
            if (timeout++ > 450) {
                // Optional: Add timeout error handling
                return 0;
            }
        }
        // Extract the bit value and splice it into the buffer
        //if (EFUSE_APB_BIT_DATA & (1 << EFUSE_APB_RD_DATA)) 
				if (EFUSE_APB_BIT_DATA & 0x1) 
				{
            efuse_data_buf |= (1u << bit_pos); // Set the bit according to the bit position
        }
    }	
     reg_write(0x40020000+0x40,efuse_data_buf);		
		//SYS_HW32_REG_WR(0x310,efuse_data_buf);//test new register for storing efuse value
		return efuse_data_buf;
}



/*
//Efuse State

DO     CELL STATE
H      Programmed
L      Un-programmed(default)

*/

void gecko_efuse_write(int dat) {

	  reg_write(0x40020000+0x44,dat);
    for (uint32_t bit_pos = 0; bit_pos < 32; bit_pos++) {
        int timeout = 0;		
        // Set the bit address to be read (directly use the address value from 0 - 31)			
        //EFUSE_APB_RW_ADDR = bit_pos;
			  EFUSE_APB_RW_ADDR = (1<<bit_pos);

			
			 if(dat & 0x00000001)
			 {			
						// Trigger the write operation
						EFUSE_APB_RW_CFG = (1 << EFUSE_APB_WR_TRIGGER);

						// Wait for the write to complete or timeout
						while (!(EFUSE_APB_BIT_DATA & (1 << EFUSE_APB_WR_FINISH))) {
								if (timeout++ > 450) {
										// Optional: Add timeout error handling
										return;
								}
						}
					
				}
				
				//else 
				//do nothing
				
				dat>>=1;
				
				reg_write(0x40020000+0x44,dat);
				
				

    }	

}















#if 0

/*
    read one efuse byte
    param-addr: efuse addr, from 0x000 to 0x1FF
*/
uint8_t duet_efuse_byte_read(uint16_t addr)
{
    EFUSE->B_ADDR = addr;
    EFUSE->CFG_TYPE = 0x0;//read type

    EFUSE->START = 0x1;
    while(EFUSE->START & 0x1);
    return EFUSE->RDBK_DATA;
}

/*
    read one efuse word
    param-addr: efuse addr, from 0x000 to 0x1FC
*/
uint32_t duet_efuse_word_read(uint16_t addr)
{
    uint32_t rd_word_data = 0;
    uint8_t rd_byte_data = 0;
    for(int i = 0; i < 4; i++)
    {
        rd_byte_data = duet_efuse_byte_read(addr+i);
        rd_word_data |= rd_byte_data << (i<<3);
    }
    return rd_word_data;
}

/*
    program one efuse byte
    param-addr: efuse addr, from 0x000 to 0x1FF
    param-data: data to be written
*/
void duet_efuse_byte_write(uint16_t addr, uint8_t data)
{
    EFUSE->PGM_DATA = data;
    EFUSE->B_ADDR = addr;
    EFUSE->CFG_TYPE = 0x1;//program type
    EFUSE->WR_TYPE = 0x0;//write type: byte
    EFUSE->START = 0x1;
    while(EFUSE->START & 0x1);
}

/*
    program one efuse word
    param-addr: efuse addr, from 0x000 to 0x1FC
    param-data: data to be written
*/
void duet_efuse_word_write(uint16_t addr, uint32_t data)
{
    EFUSE->PGM_DATA = data;
    EFUSE->B_ADDR = addr;
    EFUSE->CFG_TYPE = 0x1;//program type
    EFUSE->WR_TYPE = 0x1;//write type: word
    EFUSE->START = 0x1;
    while(EFUSE->START & 0x1);
}

#endif




#if 0
void Boot_Efuse_Read(st_EFUSE_config *efuse_cfg)
{

}





void ROM_SystemInit(void)
{
  	//uint8 g_boot_efuse_cfg;
		Boot_Efuse_Read(&g_efuse_cfg); //read efuse for booting software 

		g_efuse_cfg.clk32k_calib = 0x01;			///dbgCtrl &0xff;
    g_efuse_cfg.osc_calib = 0x02;					////(dbgCtrl>>8)&0xff;
    g_efuse_cfg.product_id = 0x58;			////(dbgCtrl>>16)&0xff;	
  
}
#endif





//物理不可克隆函数 (PUF)

//芯片间RC频率的固有差异，正是构建物理不可克隆函数（PUF）的核心原理之一




//RC 频率的漂移可能导致同一芯片在不同温度下读出的 ID 变化超过 ±5%。此时可以：

//在生产时记录每个芯片在室温下的“黄金 ID”并写入 Flash。

//运行时读取当前 ID，与黄金 ID 比较，允许 ±5% 的误差窗口。



//uint32_t Get_RC_Unique_ID(void)


