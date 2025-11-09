#include "motor_app.h"


/* ==================== 初始化函数 ==================== */

/**
 * @brief  左电机初始化
 */
static void Motor_L_Init(void)
{
    /* 1. 启动PWM输出 (TIM2_CH2) */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    /* 2. 设置初始安全状态 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);  /* PWM占空比0% */
}

/**
 * @brief  右电机初始化
 */
static void Motor_R_Init(void)
{
    /* 1. 启动PWM输出 (TIM2_CH1) */
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    /* 2. 设置初始安全状态 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);  /* PWM占空比0% */
}

/**
 * @brief  电机初始化总函数
 */
void Motor_Init(void)
{
    Motor_L_Init();
    Motor_R_Init();
}


/**
 * @brief  设置电机PWM
 * @param  pwm_value: PWM值 (-100 ~ +100)，正值正转，负值反转
 * @param  pwm_channel: PWM通道 (如TIM_CHANNEL_1)
 * @param  dir_port: 方向控制GPIO端口 (如L_Dir_GPIO_Port)
 * @param  dir_pin: 方向控制GPIO引脚 (如L_Dir_Pin)
 * @note   正转: DIR=0, PWM=abs(pwm_value)
 *         反转: DIR=1, PWM=abs(pwm_value)
 */
void SetMotorPwm(int16_t pwm_value, uint32_t pwm_channel, GPIO_TypeDef *dir_port, uint16_t dir_pin)
{
    uint16_t abs_pwm;
    GPIO_PinState dir_level;

    /* 1. 边界限制 */
    if (pwm_value > MOTOR_PWM_MAX)  pwm_value = MOTOR_PWM_MAX;
    if (pwm_value < -MOTOR_PWM_MAX) pwm_value = -MOTOR_PWM_MAX;

    /* 2. 确定方向和PWM绝对值 */
    if (pwm_value >= 0)
    {
        dir_level = GPIO_PIN_RESET;  /* 正转，方向引脚为0 */
        abs_pwm = pwm_value;
    }
    else
    {
        dir_level = GPIO_PIN_SET;    /* 反转，方向引脚为1 */
        abs_pwm = -pwm_value;
    }

    /* 3. 设置PWM和方向 */
    __HAL_TIM_SET_COMPARE(&htim2, pwm_channel, abs_pwm);
    HAL_GPIO_WritePin(dir_port, dir_pin, dir_level);
}


/**
 * @brief  紧急停止
 */
void Motor_Stop(void)
{
    SetMotorPwm(0, TIM_CHANNEL_1, R_Dir_GPIO_Port, R_Dir_Pin);  /* 右电机停止 */
    SetMotorPwm(0, TIM_CHANNEL_2, L_Dir_GPIO_Port, L_Dir_Pin);  /* 左电机停止 */
}


