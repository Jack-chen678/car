#include "uart_app.h"

unsigned char uart_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区
unsigned char uart2_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区

/* 全局变量 */
DebugMode_t g_debug_mode = DEBUG_MODE_STOP;

/******************************************************************************************/


void Uart1_Task(void)
{
    // 检查接收完成标志（bit15=1表示接收到完整的\r\n结尾数据）
    if(USART_RX_STA & 0x8000)
    {
        // 查找@开头的命令
        char *cmd = strstr((char*)USART_RX_BUF, "@");
        if(cmd != NULL)
        {
            // 中断处理已经添加了'\0'结束符，直接解析命令
            ParseCommand(cmd);
        }

        // 清空接收状态和缓冲区
        USART_RX_STA = 0;
        memset(USART_RX_BUF, 0, USART_REC_LEN);
    }
}


// DMA方式空闲中断回调函数（已禁用，保留代码）
/*
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 1. 确认是目标串口 (USART1)
    if (huart->Instance == USART1)
    {
        // 将 DMA 缓冲区中有效的数据 (Size 个字节) 复制到待处理缓冲区
        rt_ringbuffer_put(&ring_buffer, uart_rx_dma_buffer, Size);
        // 注意：这里使用了 Size，只复制实际接收到的数据

        // 举起"到货通知旗"，告诉主循环有数据待处理

        // 清空 DMA 接收缓冲区，为下次接收做准备
        memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));

        // **关键：重新启动下一次 DMA 空闲接收**
        //    必须再次调用，否则只会接收这一次
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_rx_dma_buffer, sizeof(uart_rx_dma_buffer));
    }
}
*/

/**
 * @brief  解析上位机命令
 * @param  cmd: 命令字符串，格式：@参数名=数值\r\n 或 @Stop\r\n
 */
void ParseCommand(char *cmd)
{
    float value;

      // 左轮PID参数
      if(sscanf(cmd, "@Kp_L=%f", &value) == 1) {
          SpeedPID_Left.Kp = value;
          g_debug_mode = DEBUG_MODE_LEFT;
      }
      else if(sscanf(cmd, "@Ki_L=%f", &value) == 1) {
          SpeedPID_Left.Ki = value;
          g_debug_mode = DEBUG_MODE_LEFT;
      }
      else if(sscanf(cmd, "@Kd_L=%f", &value) == 1) {
          SpeedPID_Left.Kd = value;
          g_debug_mode = DEBUG_MODE_LEFT;
      }
      else if(sscanf(cmd, "@Aim_L=%f", &value) == 1) {
          SpeedPID_Left.target_val = value;
          g_debug_mode = DEBUG_MODE_LEFT;
      }
    
      // 右轮PID参数
      else if(sscanf(cmd, "@Kp_R=%f", &value) == 1) {
          SpeedPID_Right.Kp = value;
          g_debug_mode = DEBUG_MODE_RIGHT;
      }
      else if(sscanf(cmd, "@Ki_R=%f", &value) == 1) {
          SpeedPID_Right.Ki = value;
          g_debug_mode = DEBUG_MODE_RIGHT;
      }
      else if(sscanf(cmd, "@Kd_R=%f", &value) == 1) {
          SpeedPID_Right.Kd = value;
          g_debug_mode = DEBUG_MODE_RIGHT;
      }
      else if(sscanf(cmd, "@Aim_R=%f", &value) == 1) {
          SpeedPID_Right.target_val = value;
          g_debug_mode = DEBUG_MODE_RIGHT;
      }
    
      // 停止命令
      else if(strcmp(cmd, "@Stop") == 0) {
          Motor_Stop();
          PID_Reset(&SpeedPID_Right);
          PID_Reset(&SpeedPID_Left);
          g_debug_mode = DEBUG_MODE_STOP;
      }

  }

/**
 * @brief  发送调试数据到FireWater上位机
 * @note   数据格式：目标值,实际值\n
 */
void SendDebugData(void)
{
    switch(g_debug_mode)
    {
        case DEBUG_MODE_LEFT:
            Uart_Printf(&huart1, "%f,%f\n",
                SpeedPID_Left.target_val,  // 显式转换为float
                left_encoder.speed_cm_s);
            break;

        case DEBUG_MODE_RIGHT:
            Uart_Printf(&huart1, "%f,%f\n",
                SpeedPID_Right.target_val,  // 显式转换为float
                right_encoder.speed_cm_s);
            break;

        case DEBUG_MODE_STOP:
        default:
            // 不发送数据
            break;
    }
}
