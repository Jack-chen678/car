#include "pid_app.h"
#define base_speed 50
float left_base_speed=0;
float right_base_speed=0;

float left_current;
float right_current;

extern Encoder left_encoder;
extern Encoder right_encoder;

extern MOTOR left_motor;
extern MOTOR right_motor;

extern float g_line_position_error; // 循迹误差值（循迹环当前值）
int pid_angle_output=0;
bool track_enable=0;

// 低通滤波器系数 (Low-pass filter coefficient 'alpha')
// alpha 越小, 滤波效果越强, 但延迟越大。建议从 0.1 到 0.5 之间开始尝试。
#define SPEED_FILTER_ALPHA_LEFT  0.4f 
#define SPEED_FILTER_ALPHA_RIGHT 0.4f 

/* PID 控制器实例 */
PID_T pid_speed_left;  // 左轮速度环
PID_T pid_speed_right; // 右轮速度环
PID_T pid_line;        // 循迹环
PID_T pid_dis;        // 距离环

float pid_line_output=0;
/* PID 参数定义 */
PidParams_t pid_params_left = {
    .kp = 500.5f,        
    .ki = 20.500f,      
    .kd = 0.00f,      
    .out_min = -16700.0f,
    .out_max = 16700.0f,
};

PidParams_t pid_params_right = {
    .kp = 1000.0f,        
    .ki = 15.500f,      
    .kd = 0.00f,      
    .out_min = -16700.0f,
    .out_max = 16700.0f,
};

PidParams_t pid_params_line = {
    .kp = 6.50f,        
    .ki = 0.001f,      
    .kd = 12.60f,      
    .out_min = -100.0f,
    .out_max = 100.0f,
};


PidParams_t pid_params_dis= {
    .kp = 1.00f,        
    .ki = 0.00f,      
    .kd = 0.5f,      
    .out_min = -5.0f,
    .out_max = 5.0f,
};
void PID_Init(void)
{
  pid_init(&pid_speed_left,
           pid_params_left.kp, pid_params_left.ki, pid_params_left.kd,
           0.0f, pid_params_left.out_max);
  
  pid_init(&pid_speed_right,
           pid_params_right.kp, pid_params_right.ki, pid_params_right.kd,
           0.0f, pid_params_right.out_max);
  
  pid_init(&pid_dis,
           pid_params_dis.kp, pid_params_dis.ki, pid_params_dis.kd,
           0.0f, pid_params_dis.out_max);
  
  pid_init(&pid_line,
           pid_params_line.kp, pid_params_line.ki, pid_params_line.kd,
           0.0f, pid_params_line.out_max);
//  
  pid_init(&pid_speed_right,
           pid_params_right.kp, pid_params_right.ki, pid_params_right.kd,
           0.0f, pid_params_right.out_max);
    
  pid_set_target(&pid_speed_left, right_base_speed);
  pid_set_target(&pid_speed_right, left_base_speed);
  pid_set_target(&pid_line, 0);
  pid_set_target(&pid_dis,20);
}

bool pid_running = false; // PID 控制使能开关

int output_left = 0, output_right = 0;
int line_output = 0;

//循迹环PID控制
void pid_line_control(void)
{
//    pid_line_output=0;
    pid_line_output=pid_calculate_positional(&pid_line, g_line_position_error);

    //输出限幅
    pid_line_output = pid_constrain(pid_line_output,pid_params_line.out_min,pid_params_line.out_max);
//    
    pid_line.integral=0;
    pid_line.integral=0;

}

//距离环PID控制
void pid_dis_control(void)
{
    float pid_dis_output=0;
    pid_dis_output=pid_calculate_positional(&pid_dis, ultrasonic.last_distance);
    
    pid_dis_output = pid_constrain(pid_dis_output,pid_params_dis.out_min,pid_params_dis.out_max);
    left_base_speed=base_speed-pid_dis_output;
    right_base_speed=base_speed-pid_dis_output;
}

/**********************************滤波************************************************/
float left_speed_filtered(void)
{
    // 用于存储滤波后速度的变量
    static float filtered_speed_left = 0.0f;
    filtered_speed_left = SPEED_FILTER_ALPHA_LEFT * left_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_LEFT) * filtered_speed_left;
    return filtered_speed_left;
}

float right_speed_filtered(void)
{
    // 用于存储滤波后速度的变量
    static float filtered_speed_right = 0.0f;
    filtered_speed_right = SPEED_FILTER_ALPHA_RIGHT * right_encoder.speed_cm_s + (1.0f - SPEED_FILTER_ALPHA_RIGHT) * filtered_speed_right;
    return filtered_speed_right;
}
/******************************************************************************************/

void PID_Task(void)
{
    if(pid_running == false) 
    {
        return;
    }
    
    if(track_enable)
    {
        pid_dis_control();
    }
    
    pid_line_control();
    
    pid_speed_left.target=left_base_speed-pid_line_output;
    pid_speed_right.target=left_base_speed+pid_line_output;
    
    left_current =left_speed_filtered();
    right_current = right_speed_filtered();
    
    output_left = pid_calculate_positional(&pid_speed_left,left_current);
    output_right = pid_calculate_positional(&pid_speed_right,right_current);
    
    // 输出限幅
    output_left = pid_constrain(output_left, pid_params_left.out_min, pid_params_left.out_max);
    output_right = pid_constrain(output_right, pid_params_right.out_min, pid_params_right.out_max);
    
    // 设置电机速度
    Motor_Set_Speed(&left_motor, output_left);
    Motor_Set_Speed(&right_motor, output_right);
    
    //Uart_Printf(&huart3, "Left:%.2fcm/s  Right:%.2fcm/s\r\n", left_encoder.speed_cm_s, right_encoder.speed_cm_s);
    //Uart_Printf(&huart3, "output_left:%d  output_right:%d\r\n",output_left,output_right);
    //Uart_Printf(&huart3,"%f,%f\r\n",right_encoder.speed_cm_s,pid_speed_right.target);
    //Uart_Printf(&huart3,"%f\r\n", output_left);
}


