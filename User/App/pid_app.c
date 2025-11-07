#include "pid_app.h"

PID_TypeDef SpeedPID_Right;
PID_TypeDef SpeedPID_Left;
PID_TypeDef AnglePID;
PID_TypeDef TrackPID;

void PID_Reset(PID_TypeDef *pid) {
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

    // 角度环PID初始化
//    AnglePID.Kp = 2.0f;
//    AnglePID.Ki = 0.0f;
//    AnglePID.Kd = 1.2f;
//    AnglePID.integralLimit = 0.0f;
//    AnglePID.outputLimit = 100.0f;
//	AnglePID.Deadzone = 0;
//    PID_Reset(&AnglePID);
//	
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

	if(pid->error<-pid->Deadzone||pid->error>pid->Deadzone){
		// 计算PID输出
		pid->output = pid->Kp * pid->error + 
					  pid->Ki * temp_integral + 
					  pid->Kd * pid->derivative;
	}else pid->output = pid->Ki * temp_integral;
	
	
    // 输出限幅
    if (pid->output > pid->outputLimit) 
    {
		return pid->outputLimit;
    } else if (pid->output < -pid->outputLimit) 
    {
		return -pid->outputLimit;
    }
	// 仅当输出未被限幅时，更新积分项
	pid->integral = temp_integral;
    return pid->output;
}


//float PositionPID_Calculate(PID_TypeDef *pid,int32_t actual_val) {
//// 计算误差
//pid->error = pid->target_val - actual_val;

//// 计算微分项,误差变化率
//pid->derivative = pid->error - pid->prevError;
//pid->prevError = pid->error;

//// 临时积分项（先不更新到结构体）
//float temp_integral = pid->integral + pid->error; 

//// 积分限幅
//if (temp_integral > pid->integralLimit) {
//	temp_integral = pid->integralLimit;
//} else if (temp_integral < -pid->integralLimit) {
//	temp_integral = -pid->integralLimit;
//}

//if(pid->error<-pid->Deadzone||pid->error>pid->Deadzone){
//	// 计算PID输出
//	pid->output = pid->Kp * pid->error + 
//				  pid->Ki * temp_integral + 
//				  pid->Kd * pid->derivative;
//}else pid->output = pid->Ki * temp_integral;


//// 输出限幅
//if (pid->output > pid->outputLimit) {
//	return pid->outputLimit;
//} else if (pid->output < -pid->outputLimit) {
//	return -pid->outputLimit;
//}
//// 仅当输出未被限幅时，更新积分项
//pid->integral = temp_integral;
//return pid->output;
//}

















//float velocity_planner(float current_pos, float target_pos, 
//				   float current_speed, float max_speed, 
//				   float max_accel, float max_decel) {
//// 计算到目标的剩余距离和方向
//float distance = fabs(target_pos - current_pos);
//float direction = (target_pos > current_pos) ? 1.0f : -1.0f;

//// 计算安全减速所需的最大速度
//float safe_decel_speed = sqrtf(2.0f * max_decel * distance);

//// 计算考虑启动加速度的最大允许速度
//// 启动阶段：v = sqrt(2ad)，确保从0加速到v的距离不超过当前已行驶距离
//float distance_traveled = fabs(target_pos) - distance;  // 已行驶距离
//float safe_accel_speed = sqrtf(2.0f * max_accel * fmaxf(distance_traveled, 0.0f));

//// 综合考虑：取三者最小值（当前速度+加速度限制、减速限制、最大速度）
//float max_allowed_speed = fminf(
//	fminf(fabs(current_speed) + max_accel * 0.1f, safe_accel_speed),  // 启动加速度限制
//	fminf(safe_decel_speed, max_speed)                                   // 减速限制和最大速度
//);

//// 带上方向信息
//return max_allowed_speed * direction;
//}
//				   


//PositionPID Positionpid;
//// PID初始化
//void PositionPID_Init(PositionPID *pid, float kp, float ki, float kd, 
//		 float integral_limit, float output_limit,
//		 float max_accel, float max_decel) {
//pid->Kp = kp;
//pid->Ki = ki;
//pid->Kd = kd;
//pid->integral_limit = integral_limit;
//pid->output_limit = output_limit;
//pid->error = 0.0f;
//pid->prev_error = 0.0f;
//pid->integral = 0.0f;
//pid->output = 0.0f;
//pid->current_speed = 0.0f;
//pid->max_accel = max_accel;
//pid->max_decel = max_decel;
//}

//// PID计算（带完整速度规划）
//float PID_Compute(PositionPID *pid, float current_pos) {
//// 计算误差
//pid->error = pid->target_pos - current_pos;

//// 速度规划：结合启动加速度和减速限制
//float max_allowed_speed = velocity_planner(
//	current_pos, pid->target_pos, 
//	pid->current_speed, pid->output_limit,
//	pid->max_accel, pid->max_decel
//);

//// 计算PID各项
//float derivative = (pid->error - pid->prev_error) ;
//pid->integral += pid->error ;

//// 积分限幅
//if (pid->integral > pid->integral_limit) {
//	pid->integral = pid->integral_limit;
//} else if (pid->integral < -pid->integral_limit) {
//	pid->integral = -pid->integral_limit;
//}

//// 计算基础PID输出
//float base_output = pid->Kp * pid->error + 
//					pid->Ki * pid->integral + 
//					pid->Kd * derivative;

//// 速度限幅（结合规划的最大允许速度）
//float output = base_output;
//if (output > max_allowed_speed) {
//	output = max_allowed_speed;
//} else if (output < -max_allowed_speed) {
//	output = -max_allowed_speed;
//}

//// 保存本次误差用于下次计算
//pid->prev_error = pid->error;

//// 更新当前速度估计
//pid->current_speed = output;

//return output;
//}