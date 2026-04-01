#ifndef __GLOBAL_H_
#define __GLOBAL_H_

#ifdef APP_GLOBALS
#define OS_CPU_EXT
#else
//#define OS_CPU_EXT extern
#endif

//#include "app_cfg.h"

volatile unsigned char bFlag_init_ipxs;
volatile unsigned char bFlag_gSysTick_change; 			// vFlagunsigned chars.unsigned char0
volatile unsigned char bFlag_chg_or_dischg;    			//
volatile unsigned char bFlag_charge_on;       			// 充电
volatile unsigned char bFlag_reg0xFD_bitx;
volatile unsigned char bFlag_charge_qc;
volatile unsigned char bFlag_in_small_current_mode;
volatile unsigned char bFlag_sink_pd;
volatile unsigned char bFlag_derating_power_non_pd_9v;
// OS_CPU_EXT volatile unsigned char bFlag_double_key;
volatile unsigned char bFlag_discharge_qc;
volatile unsigned char bFlag_discharge_pd;
volatile unsigned char bFlag_ovp_in_pd_9v;
volatile unsigned char bFlag_under_3v3;                	// vFlagunsigned chars.unsigned char3
volatile unsigned char bFlag_Timeslice_16ms;           	// vFlagunsigned chars.unsigned char4
volatile unsigned char bFlag_Timeslice_display;       	// vFlagunsigned chars.unsigned char5
volatile unsigned char bFlag_led_blue_always_on; 		// vFlagunsigned chars.unsigned char6
volatile unsigned char bFlag_blink;              		// vFlagunsigned chars.unsigned char7
volatile unsigned char bFlag_disable_send_cmd;  		// vFlag2unsigned chars.unsigned char2
volatile unsigned char bFlag_p_i2c_int_is_high; 		// vFlag2unsigned chars.unsigned char3
volatile unsigned char bFlag_isys_over_1A5;
volatile unsigned char bFlag_light_load;                // vFlag2unsigned chars.unsigned char5
volatile unsigned char bFlag_Bat_CC;                    // vFlag2unsigned chars.unsigned char6
// OS_CPU_EXT volatile unsigned char bFlag_ad_voltage_battery_update; // vFlag2unsigned chars.unsigned char7
volatile unsigned char bFlag_is_breathing_mode; 		// vFlag3unsigned chars.unsigned char0
volatile unsigned char bFlag_temp_alarm;        		// vFlag3unsigned chars.unsigned char1
volatile unsigned char bFlag_enable_charge;     		// vFlag3unsigned chars.unsigned char2
volatile unsigned char bFlag_enable_discharge;  		// vFlag3unsigned chars.unsigned char3
volatile unsigned char bFlag_temp_exception_over_chg;
volatile unsigned char bFlag_temp_exception_chg; 		// bFlag_disable_ip53xx_quick_charge; // vFlag3unsigned chars.unsigned char4
volatile unsigned char bFlag_temp_exception_dischg;
volatile unsigned char bFlag_ad_ntc_update;  			// vFlag3unsigned chars.unsigned char5
volatile unsigned char bFlag_battery_is_low; 			// vFlag3unsigned chars.unsigned char6
volatile unsigned char bFlag_shutdown_eta;   			// vFlag3unsigned chars.unsigned char7
volatile unsigned char bFlag_reverse;        			// vFlag3unsigned chars.unsigned char7
volatile unsigned char bFlag_charge_complete; 			// vFlag4unsigned chars.unsigned char0
volatile unsigned char bFlag_bt_standby;      			// vFlag4unsigned chars.unsigned char1
volatile unsigned char bFlag_init_ip53xs;     
volatile unsigned char bFlag_charge_ext;
volatile unsigned char bFlag_Timeslice_1ms;           	// vFlag4unsigned chars.unsigned char4
volatile unsigned char bFlag_is_trickle_charge;    		// vFlag4unsigned chars.unsigned char5
volatile unsigned char bFlag_charging_from_empty;  		// vFlag4unsigned chars.unsigned char6
volatile unsigned char bFlag_Calc_Coulomb_Counter; 		// vFlag4unsigned chars.unsigned char7
volatile unsigned char bFlag_vsys_over_6v3;
volatile unsigned char bFlag_vsys_over_12v8;
volatile unsigned char bFlag_vsys_under_4v5;
volatile unsigned char bFlag_in_scp_A3;
volatile unsigned char bFlag_adjust_scp_ocp_5A;
volatile unsigned char bFlag_adjust_scp_ocp_4A5;
volatile unsigned char bFlag_typec_in;
volatile unsigned char bFlag_usb_a1_in;
volatile unsigned char bFlag_usb_a2_in;
volatile unsigned char bFlag_all_led_is_off;
volatile unsigned char bFlag_disable_quick_discharge;
volatile unsigned char bFlag_disable_quick_charge;
volatile unsigned char bFlag_input_ovp;
volatile unsigned char bFlag_ocp;
volatile unsigned char bFlag_short_circuit;
volatile unsigned char bFlag_disable_discharge_on;
volatile unsigned char bFlag_vol_bat_over_4v;
volatile unsigned char bFlag_derating_power_temp_ex;
volatile unsigned char bFlag_vol_bat_under_3v5;
volatile unsigned char bFlag_under_3v5_derating_power;
volatile unsigned char bFlag_change_total_i_limit;
volatile unsigned char bFlag_multi_port;
volatile unsigned char bFlag_trel_over_7v;
volatile unsigned char bFlag_v_chg_5v;
volatile unsigned char bFlag_v_chg_7v;
volatile unsigned char bFlag_v_chg_9v;
volatile unsigned char bFlag_v_chg_12v;
volatile unsigned char bFlag_cc_src_sink_ok;
volatile unsigned char bFlag_power_on;
volatile unsigned char bFlag_init_battery_volume;
volatile unsigned char bFlag_vsys_under_4v75;
volatile unsigned char bFlag_input_voltage_5v;
volatile unsigned char bFlag_bat_vol_alarm;
volatile unsigned char bFlag_under_voltage;
volatile unsigned char bFlag_bat_under_voltage;
volatile unsigned char bFlag_Timeslice_1sec;
volatile unsigned char bFlag_charge_quick;
volatile unsigned char bFlag_light_load_right;
volatile unsigned char underVoltageDispTimer;
volatile unsigned char isys_grade;
volatile unsigned char last_isys_grade;
volatile unsigned char add_voltage_offset;
volatile unsigned char counter_isys_small;
volatile unsigned char counter_isys_big;
volatile unsigned char cc_dcp_mode;
volatile unsigned char last_cc_dcp_mode;
volatile unsigned char idexSum;
volatile unsigned char counter_dl;
volatile unsigned char counter_gl;
volatile unsigned char g_counter_cc;
volatile unsigned char timer_display_full;
volatile unsigned char indicator_light_mode;
volatile unsigned char displayDataLast;
volatile unsigned int display_cycle;
volatile unsigned int voltage_battery;
volatile unsigned int voltage_vsys;
// OS_CPU_EXT volatile unsigned int voltage_vsys;
volatile unsigned char restore_reg0x86;
volatile unsigned char restore_reg0xA6;
volatile unsigned char restore_reg0xAC;
volatile unsigned char restore_reg0xBB;
volatile unsigned char timer_display_slice_16ms;
volatile unsigned char gl_timer_out;
volatile unsigned char dl_timer_out;
volatile unsigned char under_voltage_timer_out;
volatile unsigned char battery_volume_voltage;
volatile unsigned char restore_power;
volatile unsigned char restore_reg0xb1;
volatile unsigned char restore_reg0xb5;
volatile unsigned char restore_reg0xb9;
volatile unsigned char restore_reg0x29;
volatile unsigned char default_power;
volatile unsigned int timer_derating_power;
volatile unsigned char derating_power_mode;
volatile unsigned char timer_dly_chk_vsys;
volatile unsigned char restore_chg_iset_vbus5v;
volatile unsigned char restore_chg_iset_vbus9v;
volatile unsigned char restore_chg_iset_vbus12v;
volatile unsigned int ack_status;
volatile unsigned int ad_ntc;
volatile unsigned char current_ntc;
volatile unsigned int ad_offset_by_power;
volatile unsigned char displayData;
volatile unsigned char timer_dly_chk_ovp;
volatile unsigned char compareTimer;
volatile unsigned int delaySleepTimer; //! 扩大 为了 2小时的计算时间
volatile unsigned char sendTimer;
volatile unsigned char display_timer_mask;
volatile unsigned char forceDispTimer;
volatile unsigned char ledDispTimer;
volatile unsigned char timer_30_seconds;
volatile unsigned char timer_test;
volatile unsigned char timer_dischg_minimum;
volatile unsigned char timer_led4_display;
volatile unsigned char gcounter;
volatile unsigned int display_timer_10minutes;
volatile unsigned int gSysTickCurrent;
volatile unsigned int gSysTick;
volatile unsigned int current_battery;
volatile unsigned int current_vsys;
volatile unsigned int power_vsys;   
volatile unsigned int voltage_ntc; //NTC电压
// OS_CPU_EXT volatile unsigned int i_charge;
// OS_CPU_EXT volatile unsigned int i_discharge;
// OS_CPU_EXT volatile unsigned int i_charge_sum;//!库伦计
// OS_CPU_EXT volatile unsigned int i_discharge_sum;//!库伦计
// OS_CPU_EXT volatile unsigned int i_charge_average;//!库伦计
// OS_CPU_EXT volatile unsigned int i_discharge_average;//!库伦计
// OS_CPU_EXT volatile unsigned int system_voltage;
volatile unsigned int system_current;
volatile unsigned int ad_current_vout;
volatile unsigned char timer_get_ip53xs_status;
volatile unsigned char timenode[5];
// OS_CPU_EXT volatile unsigned int v_input_output;    //充/放 电电压
// OS_CPU_EXT volatile unsigned int ad_i_input_output; //充/放 电流
//OS_CPU_EXT volatile GPIO_KEY_STATE gpioKeyState;
// OS_CPU_EXT volatile unsigned char gpioKeyWaitTimer;

void _dly_us(unsigned char Time);
void _dly_ms(unsigned char timer);
void init_gpio(void);
void init_wdt(void);
void init_timer0(void);
void init_system_parameter(void) ;

#endif
