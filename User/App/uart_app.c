#include "uart_app.h"

unsigned char uart_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区
unsigned char uart2_data_buffer[BUFFER_SIZE]; // 数据处理缓冲区

/* 全局变量 */
DebugMode_t g_debug_mode = DEBUG_MODE_STOP;

/******************************************************************************************/


void Uart1_Task(void)
{
    uint16_t data_len = rt_ringbuffer_data_len(&ring_buffer);
    if(data_len > 0)
    {
        // 读取数据
        rt_ringbuffer_get(&ring_buffer, uart_data_buffer, data_len);
        uart_data_buffer[data_len] = '\0';  // 添加字符串结束符

        // 查找@开头的命令
        char *cmd = strstr((char*)uart_data_buffer, "@");
        if(cmd != NULL)
        {
            // 查找\r\n结束符
            char *end = strstr(cmd, "\r\n");
            if(end != NULL)
            {
                *end = '\0';  // 截断命令
                ParseCommand(cmd);  // 解析命令
            }
        }

        // 清空缓冲区
        memset(uart_data_buffer, 0, BUFFER_SIZE);
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 1. 确认是目标串口 (USART1)
    if (huart->Instance == USART1)
    {
        // 2. 紧急停止当前的 DMA 传输 (如果还在进行中)
        //    因为空闲中断意味着发送方已经停止，防止 DMA 继续等待或出错
        HAL_UART_DMAStop(huart);

        // 3. 将 DMA 缓冲区中有效的数据 (Size 个字节) 复制到待处理缓冲区
        rt_ringbuffer_put(&ring_buffer, uart_rx_dma_buffer, Size);
        // 注意：这里使用了 Size，只复制实际接收到的数据

        // 4. 举起"到货通知旗"，告诉主循环有数据待处理

        // 5. 清空 DMA 接收缓冲区，为下次接收做准备
        //    虽然 memcpy 只复制了 Size 个，但清空整个缓冲区更保险
        memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));

        // 6. **关键：重新启动下一次 DMA 空闲接收**
        //    必须再次调用，否则只会接收这一次
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_rx_dma_buffer, sizeof(uart_rx_dma_buffer));

        // 7. 如果之前关闭了半满中断，可能需要在这里再次关闭 (根据需要)
         __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

/**
 * @brief  解析上位机命令
 * @param  cmd: 命令字符串，格式：@参数名=数值\r\n 或 @Stop\r\n
 */
void ParseCommand(char *cmd)
{
    float value;
    PID_TypeDef *pid = NULL;
    DebugMode_t mode = DEBUG_MODE_STOP;

    // 1. 判断是左轮还是右轮
    if(strstr(cmd, "_L") != NULL)
    {
        pid = &SpeedPID_Left;
        mode = DEBUG_MODE_LEFT;
    }
    else if(strstr(cmd, "_R") != NULL)
    {
        pid = &SpeedPID_Right;
        mode = DEBUG_MODE_RIGHT;
    }

    // 2. 解析参数
    if(pid != NULL && sscanf(cmd, "@%*[^=]=%f", &value) == 1)
    {
        if(strstr(cmd, "Kp")) {
            pid->Kp = value;
        }
        else if(strstr(cmd, "Ki")) {
            pid->Ki = value;
        }
        else if(strstr(cmd, "Kd")) {
            pid->Kd = value;
        }
        else if(strstr(cmd, "Aim")) {
            pid->target_val = (int32_t)value;
        }
        g_debug_mode = mode;
    }

    // 3. 停止命令
    if(strcmp(cmd, "@Stop") == 0)
    {
        Motor_Stop();
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
                (float)SpeedPID_Left.target_val,  // 显式转换为float
                left_encoder.speed_cm_s);
            break;

        case DEBUG_MODE_RIGHT:
            Uart_Printf(&huart1, "%f,%f\n",
                (float)SpeedPID_Right.target_val,  // 显式转换为float
                right_encoder.speed_cm_s);
            break;

        case DEBUG_MODE_STOP:
        default:
            // 不发送数据
            break;
    }
}
