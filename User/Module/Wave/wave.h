#ifndef _WAVE_H
#define _WAVE_H
#include "Mydefine.h"
float get_Ultrasonic_distance(void);
// 超声波状态定义
typedef enum {
    ULTRASONIC_IDLE = 0,
    ULTRASONIC_WAIT_CAPTURE,
    ULTRASONIC_PROCESS_DATA
} ultrasonic_state_t;

// 超声波数据结构
typedef struct {
    ultrasonic_state_t state;
    uint32_t trigger_time;
    uint32_t wait_start_time;
    uint8_t retry_count;
    float last_distance;
    bool new_data_ready;
} ultrasonic_data_t;


void Ultrasonic_Init(void);
void Ultrasonic_StateMachine(void);
float Get_Ultrasonic_Distance_NonBlocking(void);
bool Is_New_Distance_Available(void);
void Distance_Display_Task(void);
extern ultrasonic_data_t ultrasonic;
extern float dis;
#endif

