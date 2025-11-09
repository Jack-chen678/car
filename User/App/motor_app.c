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
 * @param  channel_in1: IN1控制通道 (如TIM_CHANNEL_1)
 * @param  channel_in2: IN2控制通道 (如TIM_CHANNEL_2)
 * @note   正转: IN1=0, IN2=PWM
 *         反转: IN1=PWM, IN2=0
 */
void SetMotorPwm(int16_t pwm_value, uint32_t channel_in1, uint32_t channel_in2)
{
    /* 1. 边界限制 */
    if (pwm_value > MOTOR_PWM_MAX)  pwm_value = MOTOR_PWM_MAX;
    if (pwm_value < -MOTOR_PWM_MAX) pwm_value = -MOTOR_PWM_MAX;

    /* 2. 根据正负值设置PWM */
    if (pwm_value >= 0)  /* 正转 */
    {
        __HAL_TIM_SET_COMPARE(&htim2, channel_in1, 0);
        __HAL_TIM_SET_COMPARE(&htim2, channel_in2, pwm_value);
    }
    else  /* 反转 */
    {
        __HAL_TIM_SET_COMPARE(&htim2, channel_in1, -pwm_value);
        __HAL_TIM_SET_COMPARE(&htim2, channel_in2, 0);
    }
}


/**
 * @brief  紧急停止
 */
void Motor_Stop(void)
{
    SetMotorPwm(0, TIM_CHANNEL_1, TIM_CHANNEL_2);  /* 左电机停止 */
    /* TODO: 右电机停止 - 需要配置其他定时器通道后添加 */
}

