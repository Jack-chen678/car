#include "oled_app.h"

extern Encoder left_encoder;
extern Encoder right_encoder;
extern int base2_target_speed;
extern int base_target_speed;
extern MOTOR left_motor;
extern MOTOR right_motor;
extern float g_line_position_error;
extern PID_T pid_speed_left;  // 左轮速度环 PID 控制器
extern PID_T pid_speed_right; // 右轮速度环 PID 控制器
extern PID_T pid_line; 
extern unsigned char Digtal;
extern unsigned char system_mode;
extern bool pid_running;
extern PID_T pid_dis; 
float dis_out;
extern float left_current;
extern float left_base_speed;

extern uint8_t cnt;

//extern float pid_line_output;
void Oled_Init(void)
{
  OLED_Init();
  OLED_Clear();
}

void Oled_Task(void)
{
//    Oled_Printf(40, 0, 12, 0, "mode:%d", system_mode);
//    Oled_Printf(0, 1, 12, 0, "%d-%d-%d-%d-%d-%d-%d",L3,L2,L1,M,R1,R2,R3);
//   Oled_Printf(0, 2, 12, 0, "dis=%.2f      ",ultrasonic.last_distance );
//      Oled_Printf(0, 2, 12, 0, "run=%d      ",pid_running); 
//    Oled_Printf(0, 5, 12, 0, "base:%.2f        ",left_base_speed);
   Oled_Printf(0, 2, 12, 0, "cnt:%d    ",cnt);
//    Oled_Printf(0, 3, 12, 0, "laim:%.2f    ",pid_speed_left.target);
//    Oled_Printf(0, 4, 12, 0, "right:%.2fcm/s  ", right_encoder.speed_cm_s);
//    Oled_Printf(0, 4, 12, 0, "left:%.2fcm/s  ",left_encoder.speed_cm_s);
///    Oled_Printf(0, 6, 12, 0, "line_err:%f  ", pid_line.error);
  
//    Oled_Printf(0, 2, 12, 0, "Roll:%f  ", Roll);
//    Oled_Printf(0, 4, 12, 0, "Pitch:%f  ", Pitch);
//    Uart_Printf(&huart1, "%f,%f\r\n", pid_line_right.target, g_line_position_error); // 测试循迹环

//    Uart_Printf(&huart1, "%f,%f,%f,%f\r\n", pid_speed_left.target, left_encoder.speed_cm_s, pid_speed_right.target, right_encoder.speed_cm_s); // 测试速度环
}
