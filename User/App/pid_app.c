#include "pid_app.h"

PID_TypeDef SpeedPID_Right;
PID_TypeDef SpeedPID_Left;
PID_TypeDef TrackPID;

void PID_Reset(PID_TypeDef *pid) 
{
	pid->target_val = 0;
    pid->error = 0.0f;
    pid->prevError = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->output = 0.0f;
} 


void PID_Init(void) {
    // 速度环PID初始化
    SpeedPID_Right.Kp =	40.0f;
    SpeedPID_Right.Ki =	4.5f;
    SpeedPID_Right.Kd =	5.0f;
    SpeedPID_Right.integralLimit = 400.0f;
    SpeedPID_Right.outputLimit = 1000.0f;
	SpeedPID_Right.Deadzone = 2;
    PID_Reset(&SpeedPID_Right);
	
	SpeedPID_Left.Kp = 40.0f;
    SpeedPID_Left.Ki = 4.5f;
    SpeedPID_Left.Kd = 5.0f;
    SpeedPID_Left.integralLimit = 400.0f;
    SpeedPID_Left.outputLimit = 1000.0f;
	SpeedPID_Left.Deadzone = 2;
    PID_Reset(&SpeedPID_Left);
    
    //	// 循迹PID初始化
//    TrackPID.Kp = 0.75f;
//    TrackPID.Ki = 0.0f;
//    TrackPID.Kd = 0.7f;
//    TrackPID.integralLimit = 0.0f;
//    TrackPID.outputLimit = 100.0f;
//	TrackPID.Deadzone = 2;
//    PID_Reset(&TrackPID);
}

float PID_Calculate(PID_TypeDef *pid,int32_t actual_val) 
{
    
    // 计算误差
    pid->error = pid->target_val - actual_val;
	
	// 计算微分项,误差变化率
    pid->derivative = pid->error - pid->prevError;
    pid->prevError = pid->error;
	
	// 临时积分项（先不更新到结构体）
	float temp_integral = pid->integral + pid->error; 
    
    // 积分限幅
	if (temp_integral > pid->integralLimit) {
		temp_integral = pid->integralLimit;
	} else if (temp_integral < -pid->integralLimit) {
		temp_integral = -pid->integralLimit;
	}

	if(pid->error<-pid->Deadzone||pid->error>pid->Deadzone)
    {
		// 计算PID输出
		pid->output = pid->Kp * pid->error +
					  pid->Ki * temp_integral +
					  pid->Kd * pid->derivative;
	}
    else pid->output = pid->Ki * temp_integral;


    // 根据抗积分饱和模式选择不同的处理方式
    float final_output = pid->output;

    if (pid->antiWindupMode == 0) {
        // 遇限停止积分法（原有方式）
        // 输出限幅
        if (pid->output > pid->outputLimit)
        {
            final_output = pid->outputLimit;
        } else if (pid->output < -pid->outputLimit)
        {
            final_output = -pid->outputLimit;
        } else {
            // 仅当输出未被限幅时，更新积分项
            pid->integral = temp_integral;
        }
    } 
    else 
    {
        // 遇限削弱积分法（新实现）
        // 输出限幅
        if (pid->output > pid->outputLimit)
        {
            final_output = pid->outputLimit;
            // 遇限削弱：输出上限时，只有负误差才允许积分（削弱积分）
            if (pid->error < 0) {
                pid->integral = temp_integral;
            }
            // 正误差时不更新积分，避免进一步饱和
        } 
        else if (pid->output < -pid->outputLimit)
        {
            final_output = -pid->outputLimit;
            // 遇限削弱：输出下限时，只有正误差才允许积分（削弱积分）
            if (pid->error > 0) {
                pid->integral = temp_integral;
            }
            // 负误差时不更新积分，避免进一步饱和
        } 
        else 
        {
            // 输出未饱和，正常更新积分项
            pid->integral = temp_integral;
        }
    }

    return final_output;
}

