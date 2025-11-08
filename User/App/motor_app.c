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


void SetMotorPwm(int16_t pwm_value, uint8_t position)
{
    uint16_t abs_pwm;
    bool dir_level;

    /* 1. 确定方向和PWM绝对值 */
    if (pwm_value >= 0)
    {
      dir_level = GPIO_PIN_RESET;  // 正转，方向引脚电平保持为0
      abs_pwm = pwm_value;
    }
    else
    {
      dir_level = GPIO_PIN_SET;    // 反转,方向引脚电平保持为1
      abs_pwm = -pwm_value;        // 取绝对值
    }

    /* 2. 边界限制 */
    if (abs_pwm > MOTOR_PWM_MAX)    abs_pwm = MOTOR_PWM_MAX;

    /* 3. 根据dir参数设置对应电机 */
    if (position == Right)
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, abs_pwm);
      HAL_GPIO_WritePin(R_Dir_GPIO_Port, R_Dir_Pin, dir_level);
    }
    else  // Left_Motor
    {
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, abs_pwm);
      HAL_GPIO_WritePin(L_Dir_GPIO_Port, L_Dir_Pin, dir_level);
    }
}


/**
 * @brief  紧急停止
 */
void Motor_Stop(void)
{
    SetMotorPwm(0,Right);
    SetMotorPwm(0,Left);
}


