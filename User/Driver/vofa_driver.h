#ifndef __VOFA_DRIVER_H__
#define __VOFA_DRIVER_H__

#include "MyDefine.h"

///* ========== VOFA+ 协议相关定义 ========== */

//// VOFA+ 参数类型枚举
//typedef enum {
//    VOFA_PARAM_P1 = 0,    // 左轮比例系数
//    VOFA_PARAM_P2,        // 右轮比例系数
//    VOFA_PARAM_I1,        // 左轮积分系数
//    VOFA_PARAM_I2,        // 右轮积分系数
//    VOFA_PARAM_D1,        // 左轮微分系数 (预留)
//    VOFA_PARAM_D2,        // 右轮微分系数 (预留)
//    VOFA_PARAM_MAX
//} VOFA_PARAM_TYPE_E;

//// VOFA+ 数据类型枚举
//typedef enum {
//    VOFA_DATA_PID_LEFT = 0,     // 左轮PID数据
//    VOFA_DATA_PID_RIGHT,        // 右轮PID数据
//    VOFA_DATA_ENCODER,          // 编码器数据
//    VOFA_DATA_MOTOR_OUTPUT,     // 电机输出数据
//    VOFA_DATA_SYSTEM_STATUS,    // 系统状态数据
//    VOFA_DATA_MAX
//} VOFA_DATA_TYPE_E;

//// VOFA+ 错误码枚举
//typedef enum {
//    VOFA_OK = 0,                    // 成功
//    VOFA_ERROR_INVALID_PARAM,       // 无效参数
//    VOFA_ERROR_PARSE_FAILED,        // 解析失败
//    VOFA_ERROR_OUT_OF_RANGE,        // 参数超出范围
//    VOFA_ERROR_UART_BUSY,           // 串口忙
//    VOFA_ERROR_BUFFER_FULL,         // 缓冲区满
//    VOFA_ERROR_NOT_INITIALIZED,     // 未初始化
//    VOFA_ERROR_TIMEOUT,             // 超时
//    VOFA_ERROR_MAX
//} VOFA_ErrorCode_E;

//// 参数映射结构体
//typedef struct {
//    VOFA_PARAM_TYPE_E type;     // 参数类型
//    float* target_ptr;          // 目标参数指针
//    const char* name;           // 参数名称
//} VOFA_ParamMap_t;

//// VOFA+ 配置结构体
//typedef struct {
//    VOFA_DATA_TYPE_E dataflag;  // 当前数据输出类型
//    UART_HandleTypeDef* huart;  // 串口句柄
//    uint32_t send_interval_ms;  // 发送间隔(毫秒)
//    uint32_t last_send_time;    // 上次发送时间
//    bool enabled;               // 模块使能状态
//} VOFA_Config_t;

///* ========== VOFA+ 协议常量定义 ========== */
//#define VOFA_MAX_CHANNELS          8       // 最大通道数
//#define VOFA_CMD_BUFFER_SIZE       32      // 命令缓冲区大小
//#define VOFA_DATA_BUFFER_SIZE      128     // 数据缓冲区大小
//#define VOFA_PARAM_NAME_MAX_LEN    8       // 参数名称最大长度


///* ========== VOFA+ 核心接口函数声明 ========== */


///* ========== 全局变量声明 ========== */
//extern VOFA_Config_t g_vofa_config;
//extern const VOFA_ParamMap_t g_vofa_param_map[];
//extern const uint8_t g_vofa_param_map_size;
//extern VOFA_DATA_TYPE_E vofa_dataflag;  // 当前数据输出类型标志位

#endif // __VOFA_DRIVER_H__
