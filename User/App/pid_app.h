#ifndef __PID_APP_H__
#define __PID_APP_H__

#include "MyDefine.h"

// PID参数结构体
typedef struct
{
    float kp;          // 比例系数
    float ki;          // 积分系数
    float kd;          // 微分系数
    float out_min;     // 输出最小值
    float out_max;     // 输出最大值
} PidParams_t;

void PID_Init(void);
void PID_Task(void);
float Angle_Loop_Control(float target_angle, float measured_angle);
void pid_line_control(void);
void pid_angel_control(void);
float left_speed_filtered(void);
float right_speed_filtered(void);
void pid_dis_control(void);
#endif
