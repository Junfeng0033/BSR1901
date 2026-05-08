/*****************************************************************************************************************************
Note:

At the moment of powering up the chip, before the software takes over the hardware, 

it must be ensured that both Q1 and Q2 are non-conductive, which means that the default state of the PWM pin must be correct.

*****************************************************************************************************************************/


/*

ADC channels 5, 0, and 1 have undergone a fixed voltage division process by dividing by 2 internally. 
This is because considering that the external voltage is higher than the internal voltage, 
the internal switch cannot be controlled, so it is fixed.

ADC 通道5，通道0， 通道1 内部做了除2固定分压处理，因为考虑到外部电压高于内部电压，内部无法控制开关，所以固定死了。


4.7uH inductance, with a PWM switching frequency set at 500kHz, achieves the highest stability and efficiency.

4.7uH 电感，PWM开关频率设500kHz最稳、效率最高

*/



/*****************************************************************************************************************************

硬件管脚资源 | 管脚位置 | 备注
------------ | -------- | ------------------------------------------------
ADC5         | 无       | 测量电池电压
ADC4         | PAD19    | 测量 DC5V 电压（boost 升压时），分压后保证低于VBAT/3.3V接入ADC4

ADC7         | PAD12    | 测量电池充电电流
PWM0         | PAD13    | PWM_N
PWM1         | PAD14    | PWM_P

*****************************************************************************************************************************/




#include "platform_config.h"

#include "iomux.h"

#include "pwm.h"

#include "charger.h"
#include "adc.h"



//uint8 chg_insert_flag;
uint8 pwm_init_flag=0;





//charger PWM0 config
void Chg_PWM0_NMOS_Config(void)
{
	struct HAL_PWM_CFG_T PWM0_CFG;
	PWM0_CFG.freq= 500000;//500KHZ
	PWM0_CFG.ratio= 50;
	PWM0_CFG.Tdead_cycle_count=0;//need config it to 400ns 
	Config_PWM(HW_PWM_CHAN_0,&PWM0_CFG);	
  //hw_pwm_invert(HW_PWM_CHAN_0);
  hw_pwm_enable(HW_PWM_CHAN_0);	
}





//charger PWM1 config
void Chg_PWM1_PMOS_Config(void)
{
	struct HAL_PWM_CFG_T PWM1_CFG;
	PWM1_CFG.freq= 500000;//500KHZ
	PWM1_CFG.ratio= 50;
	PWM1_CFG.Tdead_cycle_count=0;//need config it to 400ns 
	Config_PWM(HW_PWM_CHAN_1,&PWM1_CFG);	
  //hw_pwm_invert(HW_PWM_CHAN_1);
  hw_pwm_enable(HW_PWM_CHAN_1);		
}









/**
 * 初始化充电管理器
 */
void charger_init(charger_manager_t *charger) {
    charger->state = CHARGE_STATE_IDLE;
    charger->v_bat_mv = 0;
    charger->i_charge_ma = 0;
    charger->pwm_duty = PWM_MAX; // 初始关闭

    // 初始化 PID 参数 (需根据实际电路参数调试)
    charger->pid.Kp = 0.5f;
    charger->pid.Ki = 0.1f;
    charger->pid.Kd = 0.01f;
    charger->pid.integral = 0;
    charger->pid.last_error = 0;
    charger->pid.output_min = 0;
    charger->pid.output_max = 1000;
}




/**
 * PID 计算
 * 注意：由于是 PMOS 控制，栅极电压越高(占空比越大)，导通越小(电流越小)
 * 所以误差计算和输出方向需要适配 PMOS 的特性。
 */
float pid_update(pid_controller_t *pid, float current_val) {
    float error = pid->target - current_val;
    pid->integral += error;
    
    // 积分限幅 (防止积分饱和)
    if (pid->integral > pid->output_max) pid->integral = pid->output_max;
    if (pid->integral < pid->output_min) pid->integral = pid->output_min;

    float derivative = error - pid->last_error;
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    pid->last_error = error;

    // 限幅输出
    if (output > pid->output_max) output = pid->output_max;
    if (output < pid->output_min) output = pid->output_min;

    return output;
}







uint16_t hal_get_vbat_mv(void) {
    // 实际项目中此处应读取 ADC 寄存器并转换
	  
    //return 3500; // 模拟返回 3.5V
	  return Get_Vbat_Voltage();
}

/**
 * 硬件抽象层：读取充电电流 (OPA + ADC)
 */
/**
 * 硬件抽象层：读取电池电流 (ADC7)
 */
//adc_buck_curr_read
uint16_t hal_get_icharge_ma(void) {
	
    // 实际项目中此处应读取 OPA 放大后的电流采样 ADC 值
    //return 400; // 模拟返回 400mA
	   //adc_buck_curr_read();
	   return GeckoGpadcGetRawData(GPADC_CHAN_7);
}

/**
 * 硬件抽象层：设置 PWM 占空比
 * @param duty 0-1000 (0: 全导通, 1000: 全关断)
 */
void hal_set_pwm_duty(uint16_t duty) {
    // 实际项目中此处应写入 PWM 比较寄存器
    // printf("PWM Duty Set to: %d\n", duty);
}









/**
 * 充电状态机核心逻辑
 * 该函数应在定时器中断（如 10ms/100ms）中周期性调用
 */
void charger_process(charger_manager_t *charger) {
    // 1. 数据采样
    charger->v_bat_mv = hal_get_vbat_mv();
    charger->i_charge_ma = hal_get_icharge_ma();

    switch (charger->state) {
        case CHARGE_STATE_IDLE:
            // 初始检查，进入对应阶段
            if (charger->v_bat_mv < V_PRE_THRESHOLD_MV) {
                charger->state = CHARGE_STATE_PRE;
            } else if (charger->v_bat_mv < V_CV_THRESHOLD_MV) {
                charger->state = CHARGE_STATE_CC;
                charger->pid.target = I_CC_TARGET_MA; // 设置 CC 阶段目标电流
            } else {
                charger->state = CHARGE_STATE_CV;
                charger->pid.target = V_CV_THRESHOLD_MV; // 设置 CV 阶段目标电压
            }
            break;

        case CHARGE_STATE_PRE:
            /**
             * 第一阶段：预充阶段 (Pre-Charge)
             * PWM 动作：高占空比信号 (PMOS 微导通)
             */
            charger->pwm_duty = PWM_PRECHARGE_DUTY;
            
            // 触发条件：电压升至 3.0V 以上进入 CC
            if (charger->v_bat_mv >= V_PRE_THRESHOLD_MV) {
                charger->state = CHARGE_STATE_CC;
                charger->pid.target = I_CC_TARGET_MA;
                charger->pid.integral = 0; // 重置积分
            }
            break;

        case CHARGE_STATE_CC:
            /**
             * 第二阶段：恒流充电阶段 (Constant Current, CC)
             * 动态调节：PID 维持电流恒定
             */
            {
                // PID 计算输出改变量或绝对占空比
                // 注意：如果电流小于设定值，应减小占空比(增加导通)
                float pid_out = pid_update(&charger->pid, (float)charger->i_charge_ma);
                
                // PMOS 逻辑：电流不足 -> pid_out 为正 -> 减小占空比
                // 我们将 PID 输出映射为占空比的减量
                int32_t next_duty = PWM_MAX - (int32_t)pid_out; 
                
                if (next_duty > PWM_MAX) next_duty = PWM_MAX;
                if (next_duty < PWM_MIN) next_duty = PWM_MIN;
                
                charger->pwm_duty = (uint16_t)next_duty;
            }

            // 触发条件：电压达到终止电压进入 CV
            if (charger->v_bat_mv >= V_CV_THRESHOLD_MV) {
                charger->state = CHARGE_STATE_CV;
                charger->pid.target = V_CV_THRESHOLD_MV; // 切换目标为电压
                charger->pid.integral = 0;
            }
            break;

        case CHARGE_STATE_CV:
            /**
             * 第三阶段：恒压充电阶段 (Constant Voltage, CV)
             * PWM 动作：监控 Vbat，逐渐增大占空比以维持电压
             */
            {
                float pid_out = pid_update(&charger->pid, (float)charger->v_bat_mv);
                
                // PMOS 逻辑：电压过高 -> pid_out 为正 -> 增大占空比(减少导通)
                int32_t next_duty = PWM_MAX - (int32_t)pid_out; 

                if (next_duty > PWM_MAX) next_duty = PWM_MAX;
                if (next_duty < PWM_MIN) next_duty = PWM_MIN;

                charger->pwm_duty = (uint16_t)next_duty;
            }

            // 触发条件：检测到充电电流减小至设定阈值以下
            if (charger->i_charge_ma < I_TERM_THRESHOLD_MA) {
                charger->state = CHARGE_STATE_TERMINATED;
            }
            break;

        case CHARGE_STATE_TERMINATED:
            /**
             * 第四阶段：充电终止 (Termination)
             * 动作：PWM 输出持续高电平 (PMOS 完全关断)
             */
            charger->pwm_duty = PWM_MAX; 
            break;

        default:
            charger->state = CHARGE_STATE_IDLE;
            break;
    }

    // 3. 执行 PWM 输出
    hal_set_pwm_duty(charger->pwm_duty);
}









void pwm_io_init(void)
{
#if 0	
	 P0DPL|=BIT(5);P0DPL&=~BIT(4);       //P02 Push-Pull Output-----PWMn
	 P0DPL|=BIT(7);P0DPL&=~BIT(6);       //P03 Push-Pull Output-----PWMp
	 P0 &= ~BIT(2);
	 P0 |= BIT(3);
#endif	
		//iomux setting PAD8(GPIOA4) as PWM_CH2 function
		//gecko_pinmux_config(PAD8,PWM_CH2);	
	  //config PAD9(GPIOA5) as PWM_CH3
	  //gecko_pinmux_config(PAD9,PWM_CH6);	
}


void pwm_init(u8 index)
{
#if 0
		mypwmduty = MINDUTY;//min
	 if(!index){	//boost
			IOMUXC3 &=~0X06;
			IOMUXC3 |= 0x06;				//en p02 p03 pwm
			PWMCFG = 0x01;					//PWM-IHRCCLK 32MHz
			PWMPSC = 1;							//PWM-31 + 1 = 32  PWM DIV
			PWMP = PERIOD+1;				//PEROID = (0x33 + 1) * 32 / 32MHz = 52us ZHOUQI
			PWMD1 = PWMP-mypwmduty; //PWM1 DUTY = (0x11 + 1) * 32 / 32MHz = 18us DUTY	
			PWMCON &=~0X07;
			PWMCON |= 0X02;					// DT= 5 * 32 / 32MHz = 5us  //DEADTIME 5us 0-7----Modfiy
			PWMCON |= 0X08;					// P03 InV
			PWMCON |= 0x80;	 				//PWM EN 
	 }else{//bulk
			IOMUXC3 &=~0X06;
			IOMUXC3 |= 0x02;				//en  p03 pwm
			P02=0;
			PWMCFG = 0x01;					//PWM-IHRCCLK 32MHz
			PWMPSC = 1;							//PWM-31 + 1 = 32
			PWMP = PERIOD+1;				//PEROID = (0x33 + 1) * 32 / 32MHz = 52us ZHOUQI
			PWMD1 = PWMP-mypwmduty-1; //PWM1 DUTY = (0x11 + 1) * 32 / 32MHz = 18us DUTY	
			PWMCON &=~0X07;	 
			PWMCON |= 0X01;					// DT= 5 * 32 / 32MHz = 5us  //DEADTIME 5us 0-7
			PWMCON |= 0x80;	 				//PWM EN 
	 }
#endif
   gecko_pwm_init();
}



void pwm_dutyset(u8 index)
{
#if 0
	 if(!index){	//boost
		 if(mypwmduty>BOOST_MAXDUTY){
			 mypwmduty=BOOST_MAXDUTY;
		 }
		 PWMD1 = PWMP-mypwmduty; 
		 
	 }else{//bulk
		 if(mypwmduty>BULK_MAXDUTY){
			 mypwmduty=BULK_MAXDUTY;
		 }
		 PWMD1 = PWMP-mypwmduty-1; 
	 }
#endif
}





void pwm_close(void)
{
#if 0
	PWMCON=0;
	IOMUXC3 &=~0X06;
	P0 &= ~BIT(2);
	P0 |= BIT(3);
	pwm_init_flag =1;//next time use pwm must init	
#endif

	gecko_pwm_stop();
	pwm_init_flag =1;//next time use pwm must init	
	
}




#if 0

#define  boost5v  0x363//
#define  boost5v_adj  0x0a

void boost_fun(void)
{

			u8 flag=0;
			if(dc5v_online_flag)return;
			if(pwm_init_flag){
				 pwm_init_flag=0;
				 pwm_init(0);//
			}

			adc_vdd_read();

			vdd_adcall += adc_121_chadc;
			testcnt1++;
			if(testcnt1<7){
				return;
			}
			testcnt1=0;
			vdd_adcall >>=3;// /8
			
			//uart0_send_byte(0xaa);	
			//uart0_send_byte(0xaa);
			//uart0_send_byte((vdd_adcall>>8));//adc_121_chadc//vdd_adcall
			//uart0_send_byte((vdd_adcall&0xff));
			
			if(vdd_adcall<(boost5v-boost5v_adj)){
				flag =1;//DEC
				//uart0_send_byte(0xac);	
			}else if(vdd_adcall>(boost5v+boost5v_adj)){
				flag =2;//ADD
				//uart0_send_byte(0xad);
			}
			//flag=0;
			vdd_adcall=0;
			if(flag){//
				if(flag==1){
					if(mypwmduty)
					mypwmduty--;
				}else{
					if(mypwmduty<BOOST_MAXDUTY)
					mypwmduty++;
				}
				flag=0;
				pwm_dutyset(0);
			}
			testcnt1=0;
			vdd_adcall  =0;// /8
}
#endif









u16 Vbat_advalue;
u16 bulk_adcvalue;

BOOL cc_charge_flag;//CC charging flag


u16 buck_vbat;
u16 buck_curadc;
u8  buckadc_cnt=0;
u8  charge_full_cnt;

//bool charge_full_flag;
BOOL charge_full_flag;

// K=(0.423/2)*4095=866
// 3.0*K= 2658
// 4.20*K= 3637
#define  vabt_3v  2658
#define  vabt_4v2  3637

//0.1*0.1/2*4095= 20
//0.35*0.1/2*4095= 71

#define  Icharge_100ma   20 
#define  Icharge_350ma   71








//High-Efficiency, 1.5-MHz,Synchronous Switch-Mode Buck Charge
void bulk_func(void)
{
		static u8 bulkcnt=0;
		U8 flag=0;
		if(!dc5v_online_flag)return;

		if(pwm_init_flag){
		 pwm_init_flag=0;
			cc_charge_flag =0;
			charge_full_cnt =0;
		 pwm_init(1);//
		}
	
		bulkcnt++;
		if(bulkcnt>10){
			bulkcnt=0;
		}
		if(bulkcnt)return;
		
	  bulk_adcvalue=adc_buck_curr_read();
	  Vbat_advalue=adc_vbatvalue_read();//220/(300+220)=0.423

		buck_vbat +=Vbat_advalue;
		buck_curadc += bulk_adcvalue;
					
		buckadc_cnt++;
		if(buckadc_cnt<7){
			return;
		}
		buckadc_cnt=0;
		buck_vbat >>=3;//
		buck_curadc >>=3;//
					
		//#define  Icharge_100ma   20 
		//#define  Icharge_350ma   71
		if(buck_vbat<vabt_3v)//<3.0,Trickle Mode
		{
			if(buck_curadc>Icharge_100ma+3)//100MA
			{
				flag=1;
			}else if(buck_curadc<Icharge_100ma-3)
			{
				flag=2;
			}
						
		}else if(!cc_charge_flag)//<4.1v,CC Mode
		{
			if(buck_vbat>vabt_4v2)
				cc_charge_flag=1;				 
			if(buck_curadc>Icharge_350ma+5)//350ma
			{
				 flag=1;
			}else if(buck_curadc<Icharge_350ma-5)
			{
				 flag=2;
			}						
		}else//>4.10v,CV Mode
		{
			if(buck_vbat>vabt_4v2){
				flag=1;
			}else if(buck_vbat<vabt_4v2){
				flag=2;	
			}
			charge_full_cnt++;
			if(buck_curadc>Icharge_100ma)
			 charge_full_cnt=0;
			if(charge_full_cnt>100){
				charge_full_flag=1;
			}
		}
	     
	  if(buck_curadc>Icharge_350ma+5){////350ma
		 flag=1;
	  }else if(buck_curadc<Icharge_350ma-5){
		 flag=2;
	  }
						 
		if(flag)
		{
			#if 0
			if(flag==1)
			{
				if(mypwmduty>BULK_MAXDUTY)
				mypwmduty--;
			}else
			{
				if(mypwmduty<BULK_MAXDUTY)
				mypwmduty++;
			}	
			#endif
			
			flag=0;
			pwm_dutyset(1);
		}
					
		buck_vbat =0;
		buck_curadc =0;

}




//DC 5V Insert Detect
//void GPIO_0_6_IRQHandler(void)
#if 0
u8 dconcnt;
u8 dcoffcnt;

void dc_detect(void)
{
	  if(P34==1){
			if(dconcnt<21)
			dconcnt++;
			dcoffcnt =0;
			if(dconcnt==20){
				if(!dc5v_online_flag){
          pwm_close();
				}
				dc5v_online_flag=1;
			}
		}else{
			dconcnt=0;
			if(dcoffcnt<21)
			dcoffcnt++;
			if(dcoffcnt==20){
				if(dc5v_online_flag){
           pwm_close();
				}
				dc5v_online_flag=0;
			}
		}
}
#endif




#if 0

int main() {
    charger_manager_t my_charger;
    
    charger_init(&my_charger);
    
    printf("Charger Initialized. Starting loop...\n");
    
    // 模拟运行
    for (int i = 0; i < 10; i++) {
        charger_process(&my_charger);
        // 此处打印状态用于验证
        // printf("State: %d, Vbat: %dmV, Icharge: %dmA, PWM: %d\n", 
        //        my_charger.state, my_charger.v_bat_mv, my_charger.i_charge_ma, my_charger.pwm_duty);
    }

    return 0;
}

#endif




charger_manager_t my_charger; 

void Task_Charger_Control(void)
{
	
    charger_process(&my_charger);
		//bulk_func();

}

