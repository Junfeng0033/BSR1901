//#include <stdio.h>


int reg_read(int);
int reg_read_8bit(int);
int reg_read_16bit(int);
int reg_read_32bit(int);
int reg_read_check(int, int);
void poll_reg_equal(int, int);
void poll_reg_equal_mask(int, int, int);
void reg_write(int, int);
void reg_write_8bit(int, int);
void reg_write_16bit(int, int);
void reg_write_32bit(int, int);



#if 0


/* Debug option */
#ifdef WIN32
#define pDEBUG                              1
#else
#define pDEBUG                              0
#endif


#endif


void delay_1us(unsigned int delay_val);

void sram_run_delay(unsigned long delay);

	
#define FW_TC_DEBUG_USE_SYS_REG     0
#define FW_TC_DEBUG_USE_PRINTF      0

