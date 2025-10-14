#include "uart_app.h"
unsigned char uart_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区
unsigned char uart2_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区
unsigned char uart3_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区

/******************************************************************************************/


void Uart1_Task(void)
{
    uint16_t data_len = rt_ringbuffer_data_len(&ring_buffer2);
    if(data_len > 0)
    {
        rt_ringbuffer_get(&ring_buffer, uart_data_buffer, data_len);
        
        memset(uart_data_buffer, 0, data_len);
        
    }
}


void Uart2_Task(void)
{
//    uint16_t uart2_data_len = rt_ringbuffer_data_len(&ring_buffer2);
//    if(uart2_data_len > 0)
//    {
//        rt_ringbuffer_get(&ring_buffer2, uart2_data_buffer,uart2_data_len);
//        
//      Uart_Printf(&huart2,"%f,%f\r\n",left_encoder.speed_cm_s,pid_speed_left.target);
//        memset(uart2_data_buffer, 0, uart2_data_len);
//    }
    
}


void Uart3_Task(void)
{
//    uint16_t uart2_data_len = rt_ringbuffer_data_len(&ring_buffer2);
//    unsigned char res=0;
//  // 处理USART3数据
//    if(uart2_data_len > 0)
//    {
//    rt_ringbuffer_get(&ring_buffer2, uart2_data_buffer,uart2_data_len);
//    memset(uart2_data_buffer, 0, uart2_data_len);

//    }
      
}
