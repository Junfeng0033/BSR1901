
#if 0
/*---------------------------------------flash.h---------------------------------------------*/
#define STM32_FLASH_BASE 0x08000000 		//FLASH SATRT ADDR
#define FLASH_SAVE_ADDR  0X0800C004     //USER SAVE ADDR
 
//FLASH SECTOR START ADDR
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//Sector0, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//Sector1, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//Sector2, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//Sector3, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//Sector4, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//Sector5, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//Sector6, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//Sector7, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//Sector8, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//Sector9, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//Sector10,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//Sector11,128 Kbytes  
/*---------------------------------------flash.c---------------------------------------------*/
#endif



//QSPI Flash operation with Page Eraser and Page Program
void tc_qspi_to_xtx_page_op(void);

//Read XTX Flash chip ID
void tc_qspi_to_xtx_sanity(void);

//READ NorFlash Memory Test
void tc_qspi_to_xtx_s_ahb_rd (void);


//quad-wire spi norflash
//void gecko_task_cm0_sw_flash_quad();

//switch to quad spi nor flash mode
//extern void gecko_task_cm0_sw_flash_quad(void);

//SPI-to-AHB config Dual-wire to access NOR Flash
//extern void gecko_task_cm0_sw_flash(void);

//dual-wire spi norflash
//void gecko_task_cm0_sw_flash();


// Initialize Flash
int Init (unsigned long adr,  unsigned long clk,unsigned long fnc);

// Erase complete Device
int EraseChip(void);

// Erase Sector Function
int EraseSector (unsigned long sectornum);

// Program Page Function
int ProgramPage (unsigned long adr,  unsigned long sz,unsigned char *buf);

// Read Page Function
int ReadPage (unsigned long adr,  unsigned long sz,unsigned char *buf);



//------------------------------------------------------------------
//  The main test function
//------------------------------------------------------------------

void xtxflash_test_entry(void);

