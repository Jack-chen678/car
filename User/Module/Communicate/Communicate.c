#include "Communicate.h"
extern unsigned char uart2_data_buffer[BUFFER_SIZE];
extern unsigned char system_mode;
extern PID_T pid_speed_left;  // 左轮速度环 PID 控制器
extern PID_T pid_speed_right; // 右轮速度环 PID 控制器

extern bool pid_running;

void Change_()
{
    uint16_t uart2_data_len = rt_ringbuffer_data_len(&ring_buffer2);
    if(uart2_data_len > 0)
    {
        rt_ringbuffer_get(&ring_buffer2, uart2_data_buffer,uart2_data_len);
        if(strcmp((char*)uart2_data_buffer,"stop\r\n")==0)
        {
            pid_running = 0;
            Motor_Brake(&left_motor);
            Motor_Brake(&right_motor);
        }
        else if(strcmp((char*)uart2_data_buffer,"task1\r\n")==0)
        {
            pid_running=1; 
        }
        else if(strcmp((char*)uart2_data_buffer,"task2\r\n")==0)
        {

        }
        else if(strcmp((char*)uart2_data_buffer,"task3\r\n")==0)
        {


        }
        else if(strcmp((char*)uart2_data_buffer,"task4\r\n")==0)
        {

        }
        memset(uart2_data_buffer, 0, uart2_data_len);
    }
}
