#include "uart_driver.h"


uint8_t uart_rx_dma_buffer[BUFFER_SIZE]; // DMA 接收缓冲区
uint8_t uart2_rx_dma_buffer[BUFFER_SIZE];
uint8_t uart3_rx_dma_buffer[BUFFER_SIZE];

uint8_t uart2_flag=0;

//// USART1 USART2 USART3 缓冲区
uint8_t ring_buffer_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区
uint8_t ring_buffer2_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区
uint8_t ring_buffer3_input[BUFFER_SIZE]; // 环形缓冲区对应的输入缓冲区

// USART1和USART2和USART3的环形缓冲区声明
struct rt_ringbuffer ring_buffer; 
struct rt_ringbuffer ring_buffer2;
struct rt_ringbuffer ring_buffer3;

// 普通中断接收方式的变量定义（仅用于USART1）
uint16_t USART_RX_STA = 0;              // 接收状态标志
uint8_t USART_RX_BUF[USART_REC_LEN] = {0}; // 接收缓冲区

void Uart_Init(void)
{
  // 初始化USART1 - DMA方式（已禁用，保留代码）
  // rt_ringbuffer_init(&ring_buffer, ring_buffer_input, BUFFER_SIZE);
  // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_rx_dma_buffer, sizeof(uart_rx_dma_buffer));
  // __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

  // 初始化USART1 - 普通中断接收方式
  USART_RX_STA = 0;
  memset(USART_RX_BUF, 0, USART_REC_LEN);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);  // 使能接收中断
}

int Uart_Printf(UART_HandleTypeDef *huart, const char *format, ...)
{
	char buffer[512]; // 临时存储格式化后的字符串
	va_list arg;      // 处理可变参数
	int len;          // 最终字符串长度

	va_start(arg, format);
	// 安全地格式化字符串到 buffer
	len = vsnprintf(buffer, sizeof(buffer), format, arg);
	va_end(arg);

	// 通过 HAL 库发送 buffer 中的内容
	HAL_UART_Transmit(huart, (uint8_t *)buffer, (uint16_t)len, 0xFF);
	return len;
}



