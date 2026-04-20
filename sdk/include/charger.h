
#include "platform_config.h"




/*


第一阶段：预充阶段 (Pre-Charge)

触发条件：当检测到电池电压 V_{bat} < 3.0V（典型值）。

PWM 动作：由于此时电池内阻不稳定，不能大电流充电。PWM 此时输出高占空比信号。
原理：在 PMOS 方案中，栅极电压越接近源极（高电平），MOS 管导通程度越小（处于微导通状态）。通过微弱的电流将电池“唤醒”，直到电压升至安全阈值。


第二阶段：恒流充电阶段 (Constant Current, CC)
触发条件：电池电压升至 3.0V 以上。

PWM 动作：这是最关键的闭环控制阶段。电流采样：内部 OPA 实时放大采样电阻上的压降，ADC 读取电流数值。
动态调节：如果电流小于设定值（如 500mA），软件减小 PWM 占空比（降低栅极电压，加大导通）；如果电流过大，则增大占空比。
全过程：软件不断进行 PID 调节，使 PWM 信号在 PMOS 的线性区（或通过快速开关调节平均电流）摆动，维持电流恒定。


第三阶段：恒压充电阶段 (Constant Voltage, CV)

触发条件：电池电压达到终止电压（通常为 4.2V 或 4.35V）。

PWM 动作：软件切换目标。此时不再追求电流恒定，而是通过 ADC7 严密监控 V_{bat}。
过程：随着电池电量变满，为了维持电压不超标，PWM 会逐渐增大占空比。这会使 PMOS 趋向关断，导致充电电流自然下降。


第四阶段：充电终止 (Termination)

触发条件：在恒压阶段，当检测到充电电流减小至设定阈值（如 50mA，即 0.1C）以下。

动作：PWM 输出持续高电平，PMOS 完全关断，停止充电。


*/






/**
 * 电池充电状态枚举
 */
typedef enum {
    CHARGE_STATE_IDLE,          // 空闲状态
    CHARGE_STATE_PRE,           // 预充阶段 (Pre-Charge)
    CHARGE_STATE_CC,            // 恒流充电阶段 (Constant Current)
    CHARGE_STATE_CV,            // 恒压充电阶段 (Constant Voltage)
    CHARGE_STATE_TERMINATED     // 充电终止 (Termination)
} charge_state_t;

/**
 * PID 控制器结构体
 */
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float target;
    float integral;
    float last_error;
    float output_min;
    float output_max;
} pid_controller_t;

/**
 * 充电管理器配置与状态
 */
typedef struct {
    charge_state_t state;       // 当前状态
    uint16_t v_bat_mv;          // 电池电压 (mV)
    uint16_t i_charge_ma;       // 充电电流 (mA)
    uint16_t pwm_duty;          // 当前 PWM 占空比 (0-1000, 1000代表100%)
    pid_controller_t pid;       // PID 控制器实例
} charger_manager_t;




// --- 常量定义 ---
#define V_PRE_THRESHOLD_MV    3000   // 预充阈值 3.0V
#define V_CV_THRESHOLD_MV     4200   // 恒压阈值 4.2V
#define I_CC_TARGET_MA        500    // 恒流目标电流 500mA
#define I_TERM_THRESHOLD_MA   50     // 终止电流阈值 50mA
#define PWM_MAX               1000   // PWM 最大值 (100% 占空比, PMOS 关断)
#define PWM_MIN               0      // PWM 最小值 (0% 占空比, PMOS 全导通)
#define PWM_PRECHARGE_DUTY    950    // 预充阶段高占空比 (微导通)







#define PERIOD      				60//60//80 //195khz  80step can adjust 260K----60
#define BOOST_MAXDUTY      	PERIOD/2 //BOOST duty max 1/2 period
#define BULK_MAXDUTY     		PERIOD 
#define MINDUTY     				1
 







extern uint8 dc5v_online_flag;

void pwm_io_init(void);
void pwm_init(u8 index);

void bulk_func(void);

void Task_Charger_Control(void);


