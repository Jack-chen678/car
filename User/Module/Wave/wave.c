#include "wave.h"
//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)

uint8_t     TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
uint32_t TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
ultrasonic_data_t ultrasonic = {0};
float dis=0;
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	
	if(htim->Instance == TIM5)
	{
		if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获
	{
		if(TIM5CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
                TIM5CH1_CAPTURE_VAL=HAL_TIM_ReadCapturedValue(&htim5,TIM_CHANNEL_1);//获取当前的捕获值.
                TIM_RESET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
                TIM_SET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);//配置TIM5通道1上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA=0;			//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				__HAL_TIM_DISABLE(&htim5);        //关闭定时器5
				__HAL_TIM_SET_COUNTER(&htim5,0);
				TIM_RESET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1);   //一定要先清除原来的设置！！
				TIM_SET_CAPTUREPOLARITY(&htim5,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);//定时器5通道1设置为下降沿捕获
				__HAL_TIM_ENABLE(&htim5);//使能定时器5
			}		    
	}	
	
	}
	

}

/**
 * @brief 超声波测量状态机
 */
void Ultrasonic_StateMachine(void)
{
    uint32_t current_time = HAL_GetTick();
    
    switch(ultrasonic.state) {
       case ULTRASONIC_IDLE:
            // 清除上次的捕获状态
            TIM5CH1_CAPTURE_STA = 0;
            
            HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_SET);
            delay_us(15);
            HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_RESET);
       
            // 确保初始设置为上升沿捕获
            __HAL_TIM_SET_CAPTUREPOLARITY(&htim5, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
       
            // 直接进入等待捕获状态
            ultrasonic.wait_start_time = current_time;
            ultrasonic.state = ULTRASONIC_WAIT_CAPTURE;
            break;
        case ULTRASONIC_WAIT_CAPTURE:
            // 检查是否捕获完成
            if(TIM5CH1_CAPTURE_STA & 0X80) 
            {
                ultrasonic.state = ULTRASONIC_PROCESS_DATA;
            } 
            else if (HAL_GetTick() - ultrasonic.wait_start_time > 200) {  // 超时200ms
                ultrasonic.last_distance = -1.0f;  // 或-1表示错误
                TIM5CH1_CAPTURE_STA = 0;
                ultrasonic.state = ULTRASONIC_IDLE;
                //ultrasonic.retry_count++;  // 可用于重试逻辑
//                if (ultrasonic.retry_count >= 3) {  // 可选：重试3次后警报
//                Uart_Printf(&huart2,"Ultrasonic timeout error!\r\n");
//            }
            }
            break;
            
        case ULTRASONIC_PROCESS_DATA:
            {
                // 计算距离（使用你原来的计算方法）
                uint32_t temp = TIM5CH1_CAPTURE_STA & 0X3F;
                temp *= 0xFFFFFFFF;
                temp += TIM5CH1_CAPTURE_VAL;
                
                float distance = 2*(temp / 58.0f);
                ultrasonic.last_distance = distance;
                
                // 重置状态
                TIM5CH1_CAPTURE_STA = 0;
                ultrasonic.state = ULTRASONIC_IDLE;
                ultrasonic.retry_count = 0;
            }
            break;
    }
}

///**
// * @brief 获取距离值
// */
//float Get_Ultrasonic_Distance(void)
//{
//    return ultrasonic.last_distance;
//}

///**
// * @brief 检查是否有新数据
// */
//bool Is_New_Distance_Ready(void)
//{
//    if(ultrasonic.new_data_ready) {
//        ultrasonic.new_data_ready = false;
//        return true;
//        
//    }
//    return false;
//}

///**
// * @brief 距离显示任务
// */
//void Distance_Display_Task(void)
//{
//    if(Is_New_Distance_Ready()) {
//        dis = Get_Ultrasonic_Distance();
//    }
//}
