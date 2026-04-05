import numpy as np
import matplotlib.pyplot as plt

# 通用参数
f_pwm = 500e3  # PWM频率 500kHz
T_pwm = 1 / f_pwm  # PWM周期
t_total = 4 * T_pwm  # 仿真4个周期
t = np.linspace(0, t_total, 10000)  # 时间轴
t_dead = 500e-9  # 死区时间 500ns
L = 4.7e-6  # 电感值 4.7uH

# --------------------------
# 1. 充电模式（Buck降压，5V→3.7V）
# --------------------------
V_in_buck = 5
V_out_buck = 3.7
D_buck = V_out_buck / V_in_buck  # 占空比≈74%

# 生成PWM_P（Q2，P沟道：低电平导通）
pwm_p_buck = np.ones_like(t)  # 初始高电平（关断）
for i in range(int(t_total / T_pwm)):
    t_start = i * T_pwm
    t_end = i * T_pwm + D_buck * T_pwm
    pwm_p_buck[(t >= t_start) & (t < t_end)] = 0  # 低电平导通

# 生成PWM_N（Q1，N沟道：高电平导通，带死区）
pwm_n_buck = np.zeros_like(t)
for i in range(int(t_total / T_pwm)):
    t_start = i * T_pwm + D_buck * T_pwm + t_dead
    t_end = (i + 1) * T_pwm - t_dead
    pwm_n_buck[(t >= t_start) & (t < t_end)] = 1  # 高电平导通

# 电感电流计算（CCM模式）
I_L_buck = np.zeros_like(t)
I_L_ripple = (V_in_buck - V_out_buck) * D_buck * T_pwm / L  # 电流纹波
I_L_avg = 1.0  # 平均电流1A
for i in range(len(t)):
    phase = t[i] % T_pwm
    if phase < D_buck * T_pwm:
        # 上管导通，电感充电，电流上升
        I_L_buck[i] = I_L_avg - I_L_ripple/2 + (V_in_buck - V_out_buck)/L * phase
    else:
        # 下管导通，电感续流，电流下降
        I_L_buck[i] = I_L_avg + I_L_ripple/2 - V_out_buck/L * (phase - D_buck * T_pwm)

# 电感电压
V_L_buck = np.where(pwm_p_buck == 0, V_in_buck - V_out_buck, -V_out_buck)

# --------------------------
# 2. 放电模式（Boost升压，3.7V→5V）
# --------------------------
V_in_boost = 3.7
V_out_boost = 5
D_boost = 1 - V_in_boost / V_out_boost  # 占空比≈26%

# 生成PWM_N（Q1，N沟道：高电平导通，主开关管）
pwm_n_boost = np.zeros_like(t)
for i in range(int(t_total / T_pwm)):
    t_start = i * T_pwm
    t_end = i * T_pwm + D_boost * T_pwm
    pwm_n_boost[(t >= t_start) & (t < t_end)] = 1  # 高电平导通

# 生成PWM_P（Q2，P沟道：低电平导通，带死区）
pwm_p_boost = np.ones_like(t)
for i in range(int(t_total / T_pwm)):
    t_start = i * T_pwm + D_boost * T_pwm + t_dead
    t_end = (i + 1) * T_pwm - t_dead
    pwm_p_boost[(t >= t_start) & (t < t_end)] = 0  # 低电平导通

# 电感电流计算（CCM模式）
I_L_boost = np.zeros_like(t)
I_L_ripple_boost = V_in_boost * D_boost * T_pwm / L
I_L_avg_boost = V_out_boost / V_in_boost * 1.0  # 输入平均电流≈1.35A
for i in range(len(t)):
    phase = t[i] % T_pwm
    if phase < D_boost * T_pwm:
        # 下管导通，电感充电，电流上升
        I_L_boost[i] = I_L_avg_boost - I_L_ripple_boost/2 + V_in_boost/L * phase
    else:
        # 上管导通，电感放电，电流下降
        I_L_boost[i] = I_L_avg_boost + I_L_ripple_boost/2 - (V_out_boost - V_in_boost)/L * (phase - D_boost * T_pwm)

# 电感电压
V_L_boost = np.where(pwm_n_boost == 1, V_in_boost, V_in_boost - V_out_boost)

# --------------------------
# 绘图
# --------------------------
plt.rcParams['font.sans-serif'] = ['SimHei']
plt.rcParams['axes.unicode_minus'] = False

# 充电模式波形
fig1, axs1 = plt.subplots(4, 1, figsize=(12, 10), sharex=True)
# PWM_P
axs1[0].plot(t*1e6, pwm_p_buck*5, 'r', linewidth=2, label='PWM_P (Q2驱动，低电平导通)')
axs1[0].set_ylabel('电压 (V)')
axs1[0].set_title('充电模式（Buck降压，5V→3.7V）完整仿真波形')
axs1[0].legend()
axs1[0].grid(True)
# PWM_N
axs1[1].plot(t*1e6, pwm_n_buck*5, 'b', linewidth=2, label='PWM_N (Q1驱动，高电平导通)')
axs1[1].set_ylabel('电压 (V)')
axs1[1].legend()
axs1[1].grid(True)
# 电感电流
axs1[2].plot(t*1e6, I_L_buck, 'g', linewidth=2, label='电感电流 I_L')
axs1[2].set_ylabel('电流 (A)')
axs1[2].legend()
axs1[2].grid(True)
# 电感电压
axs1[3].plot(t*1e6, V_L_buck, 'm', linewidth=2, label='电感电压 V_L')
axs1[3].set_xlabel('时间 (μs)')
axs1[3].set_ylabel('电压 (V)')
axs1[3].legend()
axs1[3].grid(True)
plt.tight_layout()
plt.savefig('buck_mode_waveform.png', dpi=300, bbox_inches='tight')

# 放电模式波形
fig2, axs2 = plt.subplots(4, 1, figsize=(12, 10), sharex=True)
# PWM_N
axs2[0].plot(t*1e6, pwm_n_boost*5, 'b', linewidth=2, label='PWM_N (Q1驱动，高电平导通)')
axs2[0].set_ylabel('电压 (V)')
axs2[0].set_title('放电模式（Boost升压，3.7V→5V）完整仿真波形')
axs2[0].legend()
axs2[0].grid(True)
# PWM_P
axs2[1].plot(t*1e6, pwm_p_boost*5, 'r', linewidth=2, label='PWM_P (Q2驱动，低电平导通)')
axs2[1].set_ylabel('电压 (V)')
axs2[1].legend()
axs2[1].grid(True)
# 电感电流
axs2[2].plot(t*1e6, I_L_boost, 'g', linewidth=2, label='电感电流 I_L')
axs2[2].set_ylabel('电流 (A)')
axs2[2].legend()
axs2[2].grid(True)
# 电感电压
axs2[3].plot(t*1e6, V_L_boost, 'm', linewidth=2, label='电感电压 V_L')
axs2[3].set_xlabel('时间 (μs)')
axs2[3].set_ylabel('电压 (V)')
axs2[3].legend()
axs2[3].grid(True)
plt.tight_layout()
plt.savefig('boost_mode_waveform.png', dpi=300, bbox_inches='tight')

plt.show()