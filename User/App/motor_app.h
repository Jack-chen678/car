#ifndef __MOTOR_APP_H__
#define __MOTOR_APP_H__

#include "MyDefine.h"

#define MOTOR_PWM_MAX           80      /* TIM2 ARR值，对应100%占空比 */
#define MOTOR_ENCODER_PPR       11      /* 编码器线数(脉冲/转) */
#define MOTOR_ENCODER_RATIO     30      /* 减速比 */
#define MOTOR_TASK_PERIOD_MS    10      /* 速度计算周期(ms) */

#define Right 1
#define Left  0

/* 电机速度结构体 */
typedef struct
{
    float   speed_rpm;          /* 电机转速(RPM) */
    int16_t pwm_duty;          /* PWM占空比(-100~100) */
    uint8_t direction;     /* 电机方向 */
} Motor_t;

/* 初始化函数 */
void Motor_Init(void);

/* 控制函数 */
void SetMotorPwm(int16_t pwm_value, uint32_t channel_in1, uint32_t channel_in2);
void SetLeftMotorPwm(int16_t pwm_value);   /* 左电机PWM控制 */
void SetRightMotorPwm(int16_t pwm_value);  /* 右电机PWM控制 */
void Motor_Stop(void);                     /* 紧急停止 */


/* 速度计算函数(需定期调用，建议10ms) */
void Motor_Task(void);


#endif
