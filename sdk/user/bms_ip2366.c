#include "platform_config.h"
#include "bsr1901.h"
#include "gpio.h"
#include "bms_ip2366.h"
#include "bms_global.h"
#include "lcd_driver.h"

#include "iomux.h"

#include <stdarg.h>
#include <stdio.h>



// PA6 ------> I2C_INT  --- 充电芯片唤醒MCU 功能
#define PORT_P_INT      GPIOA
#define PIN_P_INT       6

//#define P_I2C_INT       gpio_get_value(PORT_P_INT,PIN_P_INT)
#define P_I2C_INT       ((gpio_get_value(PORT_P_INT, PIN_P_INT) == 0x40) ? 1 : 0)

#define P_I2C_INT_CLR   gpio_set_value(PORT_P_INT, 0, PIN_P_INT)
#define P_I2C_INT_SET   gpio_set_value(PORT_P_INT, 1, PIN_P_INT)

void HAL_Delay(uint32_t Delay)
{
	
}

void _dly_ms(unsigned char timer)
{
  unsigned char a, b;

  for (a = 0; a < timer; a++)
  {
    for (b = 0; b < 5; b++)
    {
      //_dly_us(197); // 快1%
    }
  }
}

/*

#define I2C_MEMADD_SIZE_8BIT 0x00000001U
#define I2Cx_TIMEOUT_MAX 0x1000
unsigned char ipxs_readByte(unsigned char deviceAddr, unsigned char regAddr)
{
    HAL_StatusTypeDef status = HAL_OK;
    uint8_t value = 0;

    status = HAL_I2C_Mem_Read(&I2cHandle, deviceAddr, (uint16_t)regAddr, I2C_MEMADD_SIZE_8BIT, &value, 1, I2Cx_TIMEOUT_MAX); // I2cxTimeout


    if (status != HAL_OK)
    {

        I2Cx_Error(deviceAddr);
    }
    Delay_us(100);
    return value;
}


void ipxs_writeByte(unsigned char deviceAddr, unsigned char regAddr, unsigned char sendByte)
{
    
    if (P_I2C_INT)
    {
        HAL_I2C_Mem_Write(&I2cHandle, deviceAddr, regAddr, 1, &sendByte, 1, 1000);
    }
}


*/

extern void gpio_i2c_rdabt_rf_write_data(UINT32 addr,unsigned char regaddr, unsigned char *data, unsigned char datalen);
extern void gpio_i2c_rdabt_rf_read_data(UINT32 addr,unsigned char regaddr, unsigned char *data, unsigned char datalen);

unsigned char ipxs_readByte(unsigned char deviceAddr, unsigned char regAddr)
{
    unsigned char value = 0;
    gpio_i2c_rdabt_rf_read_data(deviceAddr,regAddr,&value,1);
    //Delay_us(100);
    return (value);	
}

void ipxs_writeByte(unsigned char deviceAddr, unsigned char regAddr, unsigned char sendByte)
{
    if (P_I2C_INT)
    {
		gpio_i2c_rdabt_rf_write_data(deviceAddr,regAddr,&sendByte,1);
    }	
}


unsigned char gek1109_ipxs_readByte(unsigned char deviceAddr, unsigned char regAddr)
{
    unsigned char value = 0;
	
		//config INT pin for I2C talk
	  gecko_pinmux_config(PAD10,GPIO_A_6);
	  Set_GPIOA6_Input();
	  bsr1901_pullup_pulldown_config(PAD_10,PAD_HIZ);
	
	
    gpio_i2c_rdabt_rf_read_data(deviceAddr,regAddr,&value,1);
    //Delay_us(100);
    return (value);	
}


void gek1109_ipxs_writeByte(unsigned char deviceAddr, unsigned char regAddr, unsigned char sendByte)
{
	
		//config INT pin for I2C talk
	  gecko_pinmux_config(PAD10,GPIO_A_6);
	  Set_GPIOA6_output();
    IP2366_INT_SET;
	
    {
		gpio_i2c_rdabt_rf_write_data(deviceAddr,regAddr,&sendByte,1);
    }	
		
}






// 初始化ip23xs 每次ip23xs休眠唤醒都要调用一次  (io口int由低变高，持续100ms高电平)
void init_ip23xs(void)
{
    unsigned char reg_ipxs;
    unsigned char itmpData;

	  //printf("\r\n 123456789init_ip23xs ============= \r\n");
	
    if (bFlag_init_ipxs)
    {
        reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL12);
			  //printf(" init_ip23xs  -----  ipxs_readByte REG_SYS_CTL12 reg_ipxs = 0x%x \r\n",reg_ipxs);
			
        itmpData = reg_ipxs & 0xE0;

        if (0x40 == itmpData)
        {
            bFlag_init_ipxs = 0;
        }
        else
        {
            reg_ipxs &= 0x1F;
            reg_ipxs |= (BIT6); // 010 60W
            HAL_Delay(2);
            ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL12, reg_ipxs);
        }
    }

    // reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL12);

#if 0
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL8);
    reg_ipxs &= 0x3F;
    reg_ipxs |= (BIT6); //! [7:6] 01：DFP
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL8, reg_ipxs);

    if (bFlag_init_ipxs)
    {
        reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL8);
        reg_ipxs &= 0xC0;

        if (0x40 == reg_ipxs)
        {
            bFlag_init_ipxs = 0;
        }
    }
#endif
}

// C口 输入 Pd 快充 设置
void ipxs_vbus_scp(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL0);
    if (on_off)
    {                                    // 使能 C口输入SCP快充
        reg_ipxs |= (b_En_Vbus_SinkSCP); // bit2 set
    }
    else
    {                                     // 关闭 C口输入SCP快充
        reg_ipxs &= ~(b_En_Vbus_SinkSCP); // bit2 clear
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL0, reg_ipxs);
}

// 充电 设置
void ipxs_charge(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL0);
    if (on_off)
    {                               // 使能 充电
        reg_ipxs |= (b_En_Charger); // bit0 set
    }
    else
    {                                // 关闭 充电
        reg_ipxs &= ~(b_En_Charger); // bit0 clear
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL0, reg_ipxs);
}

// VBUS口MOS输出 设置
void ipxs_vbus_output(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL1);
    if (on_off)
    {                                    // 使能 VBUS口MOS输出
        reg_ipxs |= (b_En_Vbus_Src_Mos); // bit7 set
    }
    else
    {                                     // 关闭 VBUS口MOS输出
        reg_ipxs &= ~(b_En_Vbus_Src_Mos); // bit7 clear
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL1, reg_ipxs);
}

// VBUS口MOS输入 设置
void ipxs_vbus_input(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL1);
    if (on_off)
    {                                     // 使能 VBUS口MOS输入
        reg_ipxs |= (b_En_Vbus_Sink_Mos); // bit2 set
    }
    else
    {                                      // 关闭  VBUS口MOS输入
        reg_ipxs &= ~(b_En_Vbus_Sink_Mos); // bit2 clear
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL1, reg_ipxs);
}

// VOUT1 口输出 设置
void ipxs_usba_output(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL1);
    if (on_off)
    {                                   // 关闭 VOUT1 口输出
        reg_ipxs |= (b_En_Vin_Src_Mos); // bit4 set
    }
    else
    {                                    // 使能 VOUT1 口输出
        reg_ipxs &= ~(b_En_Vin_Src_Mos); // bit4 clear
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL1, reg_ipxs);
}

// 设定单节电池充满电压 （最高4.4V单节）
void vSet_single_battery_full(unsigned char vSet) // Set the full charge voltage
{
    unsigned char reg_ipxs;
    if (vSet > CONST_VSET_4V4)
    {
        reg_ipxs = 190;
    }
    else
    {
        reg_ipxs = (unsigned char)(vSet);
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL2, reg_ipxs);
}

// 设置电池端电流最大限制（不可配置小于停充电流） //Iset充电功率或电流设置
void iSet_limit_battery(unsigned char iSet) //
{
    unsigned char reg_ipxs;
    if (iSet > CONST_ISET_9A7)
    {
        reg_ipxs = CONST_ISET_9A7;
    }
    else
    {
        reg_ipxs = iSet;
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL3, reg_ipxs);
}

// trickle Itk=N*50mA
// 涓流充电电流设置
void iSet_trickle_charge(unsigned char iTK) //
{
    unsigned char reg_ipxs;
    reg_ipxs = iTK;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL6, reg_ipxs);
}

// 停充电流设置
void iSet_iStop(unsigned char iStop)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL8);
    reg_ipxs &= 0x0F;
    iStop &= 0x0F;
    reg_ipxs |= (iStop << 4);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL8, reg_ipxs);
}

// 再充电阈值
void vSet_reCharge(unsigned char Vrch)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL8);
    reg_ipxs &= 0xF3;
    Vrch &= 0x03;
    reg_ipxs |= (Vrch << 2);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL8, reg_ipxs);
}

// 让ip2369进入常开模式
void enter_always_on_mode(void)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_SYS_CTL9); // Device 0xEA ，Reg 0x86 bit7 < -- 1
    reg_ipxs &= 0x7F;                             // bit7 写 1
    ipxs_writeByte(0xEA, REG_SYS_CTL9, reg_ipxs); //
}

// 当需要关机时，可以通过写寄存器的方式让IP2369进入休眠状态
// I2C 地址 0XEA 寄存器地址 0x09 Bit7== 1 状态下 bit6 写1，非充电情况下立刻进入待机，
// 写完后就不要再访问I2C，等INT为低后MCU就可以进入休眠状态。
void set_force_standby_mode(void)
{
    unsigned char reg_ipxs;
    unsigned char timerOut;

    reg_ipxs = ipxs_readByte(0xEA, REG_SYS_CTL9); // Device 0xEA ，Reg 0x86 bit7 < -- 1
    reg_ipxs &= 0xBF;
    reg_ipxs |= (b_En_Standby | b_Standby);       // 0x09 Bit7== 1 状态下; bit6 写 1
    ipxs_writeByte(0xEA, REG_SYS_CTL9, reg_ipxs); //
    bFlag_p_i2c_int_is_high = 0;
    timerOut = 100;

    while (P_I2C_INT)
    {
        _dly_ms(10);
        if (!(--timerOut))
        {
            break;
        }
    }
}

// 电池低电电压设置
void vSet_bat_low(unsigned char vSet)
{
    unsigned char reg_ipxs;
    reg_ipxs = (unsigned char)(vSet);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL10, reg_ipxs);
}

// 输出快充设置
void quick_discharge(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_SYS_CTL11);
    if (on_off)
    { // 使能 输出快充
        reg_ipxs |= (b_En_Vbus_Src_DPDM | b_En_Vbus_Src_Pd | b_En_Vbus_Src_SCP | b_En_Vin_Src_DPDM | b_En_Vin_SrcSCP);
    }
    else
    { // 禁止 输出快充
        reg_ipxs &= ~(b_En_Vbus_Src_DPDM | b_En_Vbus_Src_Pd | b_En_Vbus_Src_SCP | b_En_Vin_Src_DPDM | b_En_Vin_SrcSCP);
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL11, reg_ipxs);
}

// Vbus输出/输入功率选择
void pSet_vbus_Src_Power(unsigned char pSet)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_SYS_CTL12);
    reg_ipxs &= 0x1F;
    reg_ipxs |= (unsigned char)(pSet << 5);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SYS_CTL12, reg_ipxs);
}

// （选择充电PDO档位）
//  需要先读到0x35对应档位才能选择,默认选择适配器的最大PD档位，适配器拔出后配置失效，需要重新识别适配器档位再行配置。
void pSet_pdo_select(unsigned char pdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_SELECT_PDO);
    reg_ipxs &= 0xF8;
    reg_ipxs |= (unsigned char)(pdo);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_SELECT_PDO, reg_ipxs);
}

// TYPE-C 模式设置
void mSet_role_typec(unsigned char role)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_TypeC_CTL8);
    reg_ipxs &= 0x3F;
    reg_ipxs |= (unsigned char)(role << 6);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL8, reg_ipxs);
}

// VBUS 5V Pdo电流设置
void iSet_5v_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_5VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL10, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_5VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS 9V Pdo电流设置
void iSet_9v_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_9VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL11, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_9VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS 12V Pdo电流设置
void iSet_12v_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_12VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL12, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_12VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS 15V Pdo电流设置
void iSet_15v_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_15VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL13, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_15VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS 15V Pdo电流设置
void iSet_20v_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_20VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL14, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_20VPdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS pps1 Pdo电流设置
void iSet_pps1_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_Pps1Pdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL23, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_Pps1Pdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// VBUS pps2 Pdo电流设置
void iSet_pps2_pdo(unsigned char ipdo)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs |= (b_En_Pps2Pdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipdo;
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL24, reg_ipxs);
    _dly_ms(5);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs &= ~(b_En_Pps2Pdo_Iset);
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL9, reg_ipxs);
}

// （VBUS输出Pdo设置 Pps2Pdo ）
void pSet_src_pps2_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_Pps2Pdo); // Pps2Pdo使能
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_Pps2Pdo); // Pps2Pdo禁止
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUS输出Pdo设置 Pps1Pdo ）
void pSet_src_pps1_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_Pps1Pdo); // Pps2Pdo使能
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_Pps1Pdo); //  Pps2Pdo禁止
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUS输出Pdo设置 20VPdo ）
void pSet_src_20v_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_20VPdo); //
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_20VPdo); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUS输出Pdo设置 15VPdo ）
void pSet_src_15v_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_15VPdo); //
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_15VPdo); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUS输出Pdo设置 12VPdo ）
void pSet_src_12v_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_12VPdo); //
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_12VPdo); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUS输出Pdo设置 9VPdo ）
void pSet_src_9v_pdo(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    if (on_off)
    {
        reg_ipxs |= (b_En_Src_9VPdo); //
    }
    else
    {
        reg_ipxs &= ~(b_En_Src_9VPdo); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TypeC_CTL17, reg_ipxs);
}

// （VBUSPDO加10mA电流使能，需要和电流设置寄存器一起配置）
void iSet_20v_pdo_add10mA(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    if (on_off)
    {
        reg_ipxs |= (b_EN_20VPDO_ADD); //
    }
    else
    {
        reg_ipxs &= ~(b_EN_20VPDO_ADD); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18, reg_ipxs);
}

// （VBUSPDO加10mA电流使能，需要和电流设置寄存器一起配置）
void iSet_15v_pdo_add10mA(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    if (on_off)
    {
        reg_ipxs |= (b_EN_15VPDO_ADD); //
    }
    else
    {
        reg_ipxs &= ~(b_EN_15VPDO_ADD); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18, reg_ipxs);
}

// （VBUSPDO加10mA电流使能，需要和电流设置寄存器一起配置）
void iSet_12v_pdo_add10mA(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    if (on_off)
    {
        reg_ipxs |= (b_EN_12VPDO_ADD); //
    }
    else
    {
        reg_ipxs &= ~(b_EN_12VPDO_ADD); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18, reg_ipxs);
}

// （VBUSPDO加10mA电流使能，需要和电流设置寄存器一起配置）
void iSet_9v_pdo_add10mA(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    if (on_off)
    {
        reg_ipxs |= (b_EN_9VPDO_ADD); //
    }
    else
    {
        reg_ipxs &= ~(b_EN_9VPDO_ADD); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18, reg_ipxs);
}

// （VBUSPDO加10mA电流使能，需要和电流设置寄存器一起配置）
void iSet_5v_pdo_add10mA(unsigned char on_off)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    if (on_off)
    {
        reg_ipxs |= (b_EN_5VPDO_ADD); //
    }
    else
    {
        reg_ipxs &= ~(b_EN_5VPDO_ADD); //
    }
    ipxs_writeByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18, reg_ipxs);
}

#define CONST_P_VIN_FILTER 31
// 判断I2C的上升沿，下降沿，确认IC状态：休眠 -> 唤醒； 工作 --> 休眠
void get_i2c_int_status(void)
{
    static unsigned char counter;
    if (bFlag_p_i2c_int_is_high)
    {
        if (!P_I2C_INT)
        {
					
					  printf("\r\n status ip2366 sleep \r\n ");
					  printf("\r\n777_get_i2c_int_status counter= 0x%x \r\n",counter);
					
            if (++counter > CONST_P_VIN_FILTER) // 工作 --> 休眠
            {
                counter = 0;
                bFlag_p_i2c_int_is_high = 0;
                bFlag_init_ipxs = 1;
                bFlag_charge_on = 0;
            }
        }
        else
        {
            counter = 0;
        }
    }
    else
    {
        if (P_I2C_INT)
        {
					  printf("\r\n status ip2366 wakeup \r\n ");
					
					  printf("\r\n6666_get_i2c_int_status counter= 0x%x \r\n",counter);
					
            if (++counter > CONST_P_VIN_FILTER) // 休眠 -> 唤醒；
            {
							
                counter = 0;
                _dly_ms(100); // 延时100mS，确认int持续100ms高电平再通过i2c初始化ic
							  printf("\r\n #########P_I2C_INT!!!!!!!! \r\n ");
                if (!P_I2C_INT)
                {
                    return;
                }
                init_ip23xs();
								printf("\r\n 8888  init_ip23xs  99999 \r\n ");
                bFlag_p_i2c_int_is_high = 1;
            }
        }
        else
        {
            counter = 0;
        }
    }
}

//------------------------------------------------------------------------
#define CNST_FILTER_COM (3)
// 多个寄存器表示同一个状态的，每次读低8位寄存器会更新一次高8位和低8位的数据，
// 所以读寄存器必须按照先读低8位再读高8位的顺序，才能保证读到的是同一个数据。
// 例：读BAT端电压两个寄存器，顺序应该是先读0x50,再读0x51。

// 判断充电 放电 待机 状态
void get_ipxs_state(void)
{
    unsigned char reg_ipxs;
    static unsigned char counter;

    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL0);
	
	  //printf("\r\n get_ipxs_state  reg_ipxs = %x \r\n",reg_ipxs);

    if(bFlag_charge_on) //! 在充电状态下
    {
        //! 1：放电状态且输出口已经打开，没有任何异常
        // 0：放电状态输出没有打开或者有放电异常
        if (reg_ipxs & b_Output_En) // == 1：放电状态
        {
            if (++counter > 5) // CNST_FILTER_COM
            {
                counter = 0;
                bFlag_charge_on = 0; //! 放电
            }
        }
        else
        {
            counter = 0;
            if (reg_ipxs & b_high_voltage)
            {
                bFlag_charge_quick = 1;
            }
            else
            {
                bFlag_charge_quick = 0;
            }
        }
    }
    else // 在放电状态下
    {
        //! 1：充电状态（VbusOk 就算充电状态）
        // 0：非充电状态
        if (reg_ipxs & b_CHG_En)
        {
            if (++counter > CNST_FILTER_COM) // 2次
            {
                counter = 0;
                bFlag_charge_on = 1; //! 充电
            }
        }
        else
        {
            counter = 0;
			if (0 == bFlag_light_load) //! 非轻载
			{
			}
			else
			{
				//! 轻载
			}
        }
    }
}

// 通过 输出 MOS 状态 判断多口输出 和A口插入 播出
void get_multi_port_status(void)
{
    unsigned char reg_ipxs;
    unsigned char port_num;
    reg_ipxs = ipxs_readByte(0xEA, REG_STATE_CTL1);

    port_num = 0;

    if (reg_ipxs & b_Mos_Vout1) //! VOUT1  即 A1口
    {
        port_num++;
    }

    if (reg_ipxs & b_Mos_Vbus) //! VBUS  即 C口
    {
        port_num++;

        if (port_num > 1)
        {
            // disable_vbus_dishg_i_low();
        }
    }
    else
    {
        // enable_vbus_dishg_i_low();
    }

    if (bFlag_multi_port)
    {
        if (port_num < 2)
        {
            bFlag_multi_port = 0;
        }
    }
    else
    {
        if (port_num > 1)
        {
            bFlag_multi_port = 1;
        }
    }

    if (bFlag_usb_a1_in)
    {
        if (0 == (reg_ipxs & b_Mos_Vout1))
        {
            bFlag_usb_a1_in = 0;
        }
    }
    else
    {
        if (reg_ipxs & b_Mos_Vout1)
        {
            bFlag_usb_a1_in = 1;
        }
    }
}

// 获取Vbus充电电压
void get_chg_vbus_voltage(void)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_STATE_CTL2);
	
	  printf("\r\n get_chg_vbus_voltage  reg_ipxs = %x \r\n",reg_ipxs);
	
	
    switch (reg_ipxs & 0x07)
    {
    case 1:
        //  5V充电
		    printf("\r\n get_chg_vbus_voltage 5v \r\n");
        break;
    case 2:
        //  7V充电
				printf("\r\n get_chg_vbus_voltage 7v \r\n");
        break;
    case 3:
        //  9V充电
				printf("\r\n get_chg_vbus_voltage 9v \r\n");
        break;
    case 4:
        //  12V充电
				printf("\r\n get_chg_vbus_voltage 12v \r\n");
        break;
    case 5:
        //  15V充电
				printf("\r\n get_chg_vbus_voltage 15v \r\n");
        break;
    case 6:
        //  20V充电
				printf("\r\n get_chg_vbus_voltage 20v \r\n");
        break;

    default:
				printf("\r\n get_chg_vbus_voltage unknow \r\n");
        break;
    }
}

// 判断是否在快充状态
void get_quick_charge_status(void)
{
    unsigned char reg_ipxs; // 0x05

    static unsigned char counter;

    reg_ipxs = ipxs_readByte(0xEA, REG_TypeC_STATE); //

    if (bFlag_charge_on)
    {
        // 充电快充判断
        bFlag_discharge_qc = 0;

        if (bFlag_charge_qc)
        {
            if (0 == (reg_ipxs & b_Vbus_Sink_Qc_Ok))
            {
                if (++counter > 2)
                {
                    counter = 0;
                    bFlag_charge_qc = 0;
                }
            }
            else
            {
                counter = 0;
            }
        }
        else
        {
            if (reg_ipxs & b_Vbus_Sink_Qc_Ok)
            {
                if (++counter > 2)
                {
                    counter = 0;
                    bFlag_charge_qc = 1;
                }
            }
            else
            {
                counter = 0;
            }
        }
    }
    else
    {
        // 放电快充判断
        bFlag_charge_qc = 0;

        if (bFlag_discharge_qc)
        {
            if (0 == (reg_ipxs & b_Vbus_Src_Qc_Ok))
            {
                if (++counter > CNST_FILTER_COM)
                {
                    counter = 0;
                    bFlag_discharge_qc = 0;
                }
            }
            else
            {
                counter = 0;
            }
        }
        else
        {
            if (reg_ipxs & b_Vbus_Src_Qc_Ok)
            {
                if (++counter > CNST_FILTER_COM)
                {
                    counter = 0;
                    bFlag_discharge_qc = 1;
                }
            }
            else
            {
                counter = 0;
            }
        }
    }
}

// 获取VBUS接收PDO档位
void get_received_pdo(void)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_RECEIVED_PDO);
    if (reg_ipxs & b_PDO_20V)
    {
        // 设备接收到PDO20V
    }

    if (reg_ipxs & b_PDO_15V)
    {
        // 设备接收到PDO15V
    }

    if (reg_ipxs & b_PDO_12V)
    {
        // 设备接收到PDO12V
    }

    if (reg_ipxs & b_PDO_9V)
    {
        // 设备接收到PDO9V
    }

    if (reg_ipxs & b_PDO_5V)
    {
        // 设备接收到PDO5V
    }
}

// 获取Vsys输出过流或者短路状态
void get_vsys_OC_SC_status(void)
{ //! 128ms 调用一次
    unsigned char reg_ipxs;
    unsigned char i;

loop_oc_sc_again:

    if ((bFlag_p_i2c_int_is_high) && (P_I2C_INT))
    {
        reg_ipxs = ipxs_readByte(0xEA, REG_STATE_CTL3); // Device 0xEA  0xFC

        if (0 == bFlag_ocp) // 过流
        {
            if ((reg_ipxs & BIT2))
            {
                ipxs_writeByte(0xEA, REG_STATE_CTL3, reg_ipxs);
                if (++counter_gl > 1)
                {
                    counter_gl = 0;
                    gl_timer_out = 0;
                    bFlag_ocp = 1;
                    forceDispTimer = 0; //
                    set_force_standby_mode();
                    return;
                }
                else
                {
                    gl_timer_out = 36; // 3584mS
                }
            }
            else
            {
                if (0 == gl_timer_out)
                {
                    counter_gl = 0;
                }
                else
                {
                    gl_timer_out--;
                }
            }
        }

        if (0 == bFlag_short_circuit) // 短路
        {
            if ((reg_ipxs & BIT0))
            {
                ipxs_writeByte(0xEA, REG_STATE_CTL3, reg_ipxs);
                if (++counter_dl > 1)
                {
                    counter_dl = 0;
                    dl_timer_out = 0;
                    bFlag_short_circuit = 1;
                    forceDispTimer = 0;
                    set_force_standby_mode();
                    return;
                }
                else
                {
                    dl_timer_out = 36; // x128ms = 3584ms
                }
            }
            else
            {

                if (0 == dl_timer_out)
                {
                    counter_dl = 0;
                }
                else
                {
                    dl_timer_out--;
                }
            }
        }

        if ((counter_dl) || (counter_gl))
        {

            for (i = 0; i < 2; i++)
            {
                _dly_ms(20);
                if ((bFlag_p_i2c_int_is_high) && (P_I2C_INT))
                {
                    get_multi_port_status();
                }
                _dly_ms(32);
            }

            if ((bFlag_p_i2c_int_is_high) && (P_I2C_INT)) // !important
            {
                goto loop_oc_sc_again;
            }
        }
    }
}

// 获取电池电压mV
void get_voltage_battery(void)
{ //! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_BATVADC_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_BATVADC_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    voltage_battery = itmp_16bits;
	
		printf("\r\n ########get_voltage_battery#######123  voltage_battery = %x \r\n",voltage_battery);		
}

// 获取vsys电压
void get_voltage_vsys(void)
{ //! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_VsysVADC_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_VsysVADC_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    voltage_vsys = itmp_16bits;
	
		printf("\r\n #####################get_voltage_vsys#######789  voltage_vsys = %x \r\n",voltage_vsys);	
	
}



// 获取ic固件版本
void get_ipxs_firmware_version(void)
{ //! （时间戳符号为ASCII码字符）
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_TIMENODE1);
    timenode[0] = reg_ipxs;
	  //printf("\r\n get_ipxs_firmware_version = 0x%x \r\n",timenode[0]);	
    HAL_Delay(1);
    reg_ipxs = ipxs_readByte(0xEA, REG_TIMENODE2);
    timenode[1] = reg_ipxs;
	  //printf("\r\n get_ipxs_firmware_version = 0x%x \r\n",timenode[1]);	
    HAL_Delay(1);
    reg_ipxs = ipxs_readByte(0xEA, REG_TIMENODE3);
    timenode[2] = reg_ipxs;
	  //printf("\r\n get_ipxs_firmware_version = 0x%x \r\n",timenode[2]);	
    HAL_Delay(1);
    reg_ipxs = ipxs_readByte(0xEA, REG_TIMENODE4);
    timenode[3] = reg_ipxs;
	  //printf("\r\n get_ipxs_firmware_version = 0x%x \r\n",timenode[3]);	
    HAL_Delay(1);
    reg_ipxs = ipxs_readByte(0xEA, REG_TIMENODE5);
    timenode[4] = reg_ipxs;
	  printf("\r\n get_ipxs_firmware_version = 0x%x \r\n",timenode[4]);
}

// 获取BAT 端电流
void get_current_battery(void)
{ //! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_IBATIADC_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_IBATIADC_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    current_battery = itmp_16bits;
}

// 获取vsys电流
void get_current_vsys(void)
{ //! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_ISYS_IADC_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_Isys_IADC_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    current_vsys = itmp_16bits;
}

// 获取vsys 功率 单位 10mW ； Vsys_POW= Vsys_POW_ADC(10mW)
void get_power_vsys(void)
{ //! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_Vsys_POW_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_Vsys_POW_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    power_vsys = itmp_16bits;
}

// 根据ntc电压和电流，计算ntc电阻，然后推算出当前检测区域的温度
void get_current_ntc(void)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_INTC_IADC_DAT0);
    if (reg_ipxs & b_INTC_80uA)
    {
        current_ntc = CONST_80uA;
    }
    else
    {
        current_ntc = CONST_20uA;
    }
}

//! 先读低8位再读高8位的顺序，才能保证读到的是同一个数据
// 根据ntc电压和电流，计算ntc电阻，然后推算出当前检测区域的温度
void get_voltage_ntc(void)
{
    unsigned char reg_ipxs;
    unsigned int itmp_8bits;
    unsigned int itmp_16bits;
    reg_ipxs = ipxs_readByte(0xEA, REG_VGPIO0_NTC_DAT0); // 先读低8位
    itmp_8bits = reg_ipxs;
    reg_ipxs = ipxs_readByte(0xEA, REG_VGPIO0_NTC_DAT1); // 再读高8位
    itmp_16bits = reg_ipxs;
    itmp_16bits <<= 8;
    itmp_16bits |= itmp_8bits;
    voltage_ntc = itmp_16bits;
}

#ifdef USING_TEST
void init_ip23xs(void)
{
    uint8_t reg[] = {0x00, 0x01, 0x02, 0x03, 0x06, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
                     0x0d, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a,
                     0x2b, 0x2c, 0x31, 0x32, 0x33, 0x34, 0x35, 0x38, 0x50, 0x51,
                     0x52, 0x53, 0x69, 0x6a, 0x6b, 0x6d, 0x6e, 0x6f, 0x70, 0x71,
                     0x74, 0x75, 0x77, 0x78, 0x79};
    uint8_t buf[240] = {0};
    uint8_t lenth = sizeof(reg);

    for (uint8_t i = 0; i < lenth; i++)
    {
        buf[i] = ipxs_readByte(DEVICE_ADDR_IP, (reg[i]));
        // printf("%02d__[0x%02x=0x%02x]____\n",i,reg[i],buf[i]);
    }
}
#endif

#ifdef USING_TEST2
// 初始化ip23xs 每次ip23xs休眠唤醒都要调用一次  (io口int由低变高，持续100ms高电平)
void init_ip23xs(void)
{
    unsigned char reg_ipxs;
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL0);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL1);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL2);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL3);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL6);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL9);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL10);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL11);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SYS_CTL12);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_SELECT_PDO);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL8);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL9);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL10);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL11);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL13);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL14);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL23);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL24);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_CTL17);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TYPEC_CTL18);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL0);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL1);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL2);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_TypeC_STATE);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_RECEIVED_PDO);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_STATE_CTL3);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_BATVADC_DAT0);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_BATVADC_DAT1);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_VsysVADC_DAT0);
    reg_ipxs = ipxs_readByte(DEVICE_ADDR_IP, REG_VsysVADC_DAT1);
}

#endif
