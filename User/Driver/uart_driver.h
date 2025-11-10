#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#include "MyDefine.h"

#define BUFFER_SIZE 128

int Uart_Printf(UART_HandleTypeDef *huart, const char *format, ...);
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);  // DMA方式已禁用
void Uart_Init(void);

extern uint8_t uart2_flag;

extern uint8_t uart_rx_dma_buffer[128];
extern uint8_t uart2_rx_dma_buffer[128];
extern uint8_t uart3_rx_dma_buffer[128];

extern struct rt_ringbuffer ring_buffer; // 环形缓冲区
extern struct rt_ringbuffer ring_buffer2;
extern struct rt_ringbuffer ring_buffer3;

extern uint8_t ring_buffer_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区
extern uint8_t ring_buffer2_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区
extern uint8_t ring_buffer3_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区

// 普通中断接收方式的定义
#define USART_REC_LEN 128  // 接收缓冲区最大长度

extern uint16_t USART_RX_STA;              // 接收状态标志
                                           // bit15: 接收完成标志
                                           // bit14: 收到0x0d标志
                                           // bit13-0: 接收字节数
extern uint8_t USART_RX_BUF[USART_REC_LEN]; // 接收缓冲区

#endif

