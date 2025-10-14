#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__

#include "MyDefine.h"

#define BUFFER_SIZE 128 

int Uart_Printf(UART_HandleTypeDef *huart, const char *format, ...);  
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
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
#endif

