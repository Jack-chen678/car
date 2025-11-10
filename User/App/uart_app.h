#ifndef __UART_APP_H__
#define __UART_APP_H__

#include "MyDefine.h"

/* 调试模式枚举 */
typedef enum {
    DEBUG_MODE_STOP = 0,  // 停止发送数据
    DEBUG_MODE_LEFT,      // 发送左轮数据
    DEBUG_MODE_RIGHT      // 发送右轮数据
} DebugMode_t;

/* 全局变量声明 */
extern DebugMode_t g_debug_mode;

/* 函数声明 */
void Uart_Task(void);
void Uart1_Task(void);
void Uart2_Task(void);
void Uart3_Task(void);
void SendDebugData(void);
void ParseCommand(char *cmd);

#endif
