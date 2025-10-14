#include "motor_app.h"



void Motor_Init(void)
{
    Motor_Config_Init(&left_motor, &htim1, TIM_CHANNEL_1, AIN1_GPIO_Port, AIN1_Pin, AIN2_GPIO_Port, AIN2_Pin, 0, 2000);
    Motor_Config_Init(&right_motor, &htim1, TIM_CHANNEL_2, BIN1_GPIO_Port, BIN1_Pin, BIN2_GPIO_Port, BIN2_Pin, 0, 2000);
}

void Motor_Task(void)
{
    
}
