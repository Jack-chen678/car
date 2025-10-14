#include "key_app.h"

extern PID_T pid_speed_left;  // 左轮速度环 PID 控制器
extern PID_T pid_speed_right; // 右轮速度环 PID 控制器
extern unsigned char system_mode;

extern bool pid_running;
uint8_t run_flag=0;
void Key_Init(void)
{
    Ebtn_Init();
}

void Key_Task(void)
{
    ebtn_process(HAL_GetTick());
}

/* 处理按键事件的回调函数 */
// 函数原型: void (*ebtn_evt_fn)(struct ebtn_btn *btn, ebtn_evt_t evt);
void my_handle_key_event(struct ebtn_btn *btn, ebtn_evt_t evt) 
{
    uint16_t key_id = btn->key_id;                 // 获取触发事件的按键 ID
    uint16_t click_cnt = ebtn_click_get_count(btn); // 获取连击次数 (仅在 ONCLICK 事件时有意义)
    switch (evt) {
        case EBTN_EVT_ONPRESS: // 按下事件 (消抖成功后触发一次)
//            if(key_id==1)
//            led_buf[0] ^= 1;
            break;
        case EBTN_EVT_ONRELEASE: // 释放事件 (消抖成功后触发一次)
            break;
        case EBTN_EVT_ONCLICK: // 单击/连击事件 (在释放后，或达到最大连击数，或超时后触发)
            switch (key_id) 
            {
              //按键1：模式切换键
              case 1: // 开启 PID 控制
                if(++system_mode >= 5) system_mode = 0;
                break;
              
              //按键2按一次确认，连续按两次停止
              case 2:
                if(click_cnt==1)
                {
                    pid_running = 1;
                    run_flag=1;
                }
                else if(click_cnt==2)
                {
                    pid_running = 0;
                    Motor_Brake(&left_motor);
                    Motor_Brake(&right_motor);
                    click_cnt=0;
                }
                
                break;
            }
            break;
        case EBTN_EVT_KEEPALIVE: // 保持活动/长按事件 (按下持续时间超过阈值后，按周期触发)
            break;
        default: // 未知事件 (理论上不应发生)
            break;
    }
}

