#include "No_MCU_Grayscale.h"
uint16_t dma_buff[91]={0};
uint32_t gAdcResult =0;

// 微秒级延时函数，确保时序准确
static void delay_us(uint32_t us)
{
  uint32_t ticks = us * (SystemCoreClock / 1000000) / 5;  //  约5个时钟周期/循环

  while(ticks--) {
      __NOP();
  }
}
volatile bool gCheckADC=false;
/**
 * @brief  获取ADC采样值（DMA+中断方式），计算平均值
 * @param  number - 采样数量（最多90次）
 * @return 采样平均值（超时返回0，表示采样失败）
 */
unsigned int adc_getValue(unsigned int number)
{
    if(number>=90)  number=90;
    
    // 清除采样完成标志，为下一次ADC采集做准备
    gCheckADC = false;

    // 清空DMA接收缓冲区，清除残留数据，确保采样准确
    memset((uint16_t*)dma_buff, 0, sizeof(dma_buff));

    // 启动ADC DMA采集，采集完成后触发中断回调
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)dma_buff, number);

    // 等待中断回调函数设置gCheckADC标志（减少超时时间，提高响应）
    uint16_t timeout = 1000;  // 减少超时等待
    while(gCheckADC == false && timeout > 0)
    {
        timeout--;
    }

    // 判断是否超时，如果超时则停止DMA，返回有效值0
    if(timeout == 0)
    {
        HAL_ADC_Stop_DMA(&hadc1);
        Uart_Printf(&huart1,"time out!\r\n");
        return 0;  // 超时返回0，标识采集失败
    }

    uint32_t sum = 0;
    for (int i = 0; i < number; i++)
    {
        sum += dma_buff[i];
    }
    gAdcResult = sum / number;
    return gAdcResult;

}

/**
 * @brief  采集8通道ADC原始模拟值，按序存储到数组
 * @param  result - 结果数组，用于存储8通道的ADC原始值
 * @note   通过3位地址线，Address_0~2控制多路选择器，实现8通道切换
 *         支持正向/反向存储，全局变量Direction控制，0=正向，1=反向
 */
void Get_Analog_value(unsigned short *result)
{
    unsigned char i;
    unsigned int Anolag = 0;  // 临时存储单路ADC采样值

    // 循环8次，通过3位地址线，bit0~bit2切换8路采集通道
    for(i = 0; i < 8; i++)
    {
        // 地址线的电平控制，通过i的对应bit位取反，切换多路选择器的当前通道
        Switch_Address_0(!(i & 0x01));  // 地址线0 <-> i的bit0，控制的1位通道选择
        Switch_Address_1(!(i & 0x02));  // 地址线1 <-> i的bit1，控制的2位通道选择
        Switch_Address_2(!(i & 0x04));  // 地址线2 <-> i的bit2，控制的3位通道选择

        // 关键延时：地址切换后，等待多路选择器ADC输入信号稳定（避免信号抖动造成采样误差）
        delay_us(5);  // 减少延时，提高响应速度
        Anolag = adc_getValue(60);  //

        // 根据Direction控制存储顺序：正向i->result[i]，反向i->result[7-i]
        if(!Direction)
            result[i] = Anolag;        // Direction=0，正向存储（通道0->result[0]，通道1->result[1]...）
        else
            result[7 - i] = Anolag;    // Direction=1，反向存储（通道0->result[7]，通道1->result[6]...）

        Anolag = 0;  // 清空临时变量，为下一路做准备
    }
}

/* 功能说明：模拟值转换为数字信号（阈值比较）
   参数说明：
   adc_value - 原始ADC值数组
   Gray_white - 白色阈值数组
   Gray_black - 黑色阈值数组
   Digital - 输出数字信号（8位显示） */
void convertAnalogToDigital(unsigned short *adc_value,unsigned short *Gray_white,unsigned short *Gray_black,unsigned char *Digital)
{
    
    for (int i = 0; i < 8; i++) {
        if (adc_value[i] > Gray_white[i]) {
            *Digital |= (1 << i);   // 大于白色阈值输出1（白色线）
        } else if (adc_value[i] < Gray_black[i]) {
            *Digital &= ~(1 << i);  // 小于黑色阈值输出0（黑色线）
        }
        // 中间灰度值保持原状态
    }
}

/* 功能说明：归一化ADC值到指定范围
   参数说明：
   adc_value - 原始ADC值数组
   Normal_factor - 归一化系数数组
   Calibrated_black - 校准黑色值数组
   result - 存储归一化结果数组
   bits - ADC分辨率（如255/1024等） */
void normalizeAnalogValues(unsigned short *adc_value,double *Normal_factor,unsigned short *Calibrated_black,unsigned short *result,double bits)
{
    for (int i = 0; i < 8; i++) {
        unsigned short n ;
        // 计算归一化值（减去黑基准值，乘以系数）
        if(adc_value[i]<Calibrated_black[i]) n=0;  // 低于黑基准值
        else n = (adc_value[i] - Calibrated_black[i]) * Normal_factor[i];

        // 限幅处理
        if (n > bits) {
            n = bits;
        }
        result[i]=n;
    }
}

float last_offset=0;

/* 函数功能：将模拟值转换为偏移信号
   参数说明：
   adc_value - 原始ADC值数组
   Gray_white - 白色数组
   Gray_black - 黑色数组
   Offset - 输出的偏移量 */
int8_t convertAnalogToOffset(unsigned short *Normal_value, float filter_factor)
{
       // 输入验证
    if (Normal_value == NULL) {
        return last_offset;  // 返回上一次值，保持连续性
    }
    
    // 限制滤波系数范围
    if (filter_factor < 0.0f) filter_factor = 0.0f;
    if (filter_factor > 1.0f) filter_factor = 1.0f;
    
    float Weight[8] = {0};//看到"黑线的程度",0表示传感器完全在白色区域上，1表示完全在黑色上
    float total_weight = 0.0f;
    float offset = 0.0f;
    
    // 计算每个传感器的权重
    for (uint8_t i = 0; i < 8; i++) 
    {
        // 计算归一化权重，范围[0,1]
        // Normal_value范围：0(黑色) 到 4096(白色)
        // 权重：黑色=1，白色=0
        float normalized_ratio = (float)Normal_value[i] / 4096.0f;
        Weight[i] = 1.0f - normalized_ratio;
        total_weight += Weight[i];
		
		
    }
    
    // 计算加权平均偏移量（物理偏移：-3.5 ~ 3.5）
    for (uint8_t i = 0; i < 8; i++) 
    {
        offset += (i - 3.5f) * Weight[i];
    }
    
    // 防止除零：如果total_weight为0，说明没有检测到黑线，返回上次值
    if (total_weight < 0.01f) 
    {
        return (int8_t)last_offset;
    }
    offset /= total_weight;
    
    // ========== 关键修改部分 ==========
    // 将物理偏移映射到 -100 ~ 100 范围
    const float PHYSICAL_RANGE = 3.5f;   // 物理偏移的最大绝对值
    const float TARGET_RANGE = 100.0f;   // 目标范围的最大值
    const float SCALE_FACTOR = TARGET_RANGE / PHYSICAL_RANGE;
    
    float mapped_offset = offset * SCALE_FACTOR;
    
    // 限制范围到 -100 ~ 100
    if (mapped_offset < -100.0f) mapped_offset = -100.0f;
    if (mapped_offset > 100.0f) mapped_offset = 100.0f;
    // ==================================
    
    // 一阶低通滤波
    float filtered_offset = filter_factor * mapped_offset + 
                          (1.0f - filter_factor) * last_offset;
    
    last_offset=filtered_offset;//更新上一次滤波后的输出
    
    // 四舍五入转换为整数
    return (int8_t)(filtered_offset + 0.5f);
}


/* 功能说明：灰度传感器结构体初始化（首次初始化）
   参数说明：sensor - 传感器结构体指针 */
void No_MCU_Ganv_Sensor_Init_Frist(No_MCU_Sensor*sensor)
{
    // 清除传感器校准数据和状态
    memset(sensor->Calibrated_black,0,16);
    memset(sensor->Calibrated_white,0,16);
    memset(sensor->Normal_value,0,16);
    memset(sensor->Analog_value,0,16);

    // 初始化归一化系数
    for(int i = 0; i < 8; i++)
    {
        sensor->Normal_factor[i]=0.0;
    }

    // 初始化状态变量
    sensor->Digtal=0;
    sensor->Time_out=0;
    sensor->Tick=0;
    sensor->ok=0;  // 标记未完成校准
}

/* 功能说明：灰度传感器初始化（带校准参数）
   参数说明：
   sensor - 传感器结构体指针
   Calibrated_white - 校准白色值数组
   Calibrated_black - 校准黑色值数组 */
void No_MCU_Ganv_Sensor_Init(No_MCU_Sensor*sensor,unsigned short *Calibrated_white,unsigned short *Calibrated_black)
{
    No_MCU_Ganv_Sensor_Init_Frist(sensor);
    
    // 设置传感器ADC位数
    if(Sensor_ADCbits==_8Bits)sensor->bits=255.0;
    else if(Sensor_ADCbits==_10Bits)sensor->bits=1024.0;
    else if(Sensor_ADCbits==_12Bits)sensor->bits=4096.0;
    else if(Sensor_ADCbits==_14Bits)sensor->bits=16384.0;

    // 设置传感器超时时间
    if(Sensor_Edition==Class)sensor->Time_out=1;
    else sensor->Time_out=10;

    double Normal_Diff[8];
    unsigned short temp;
    
    for (int i = 0; i < 8; i++)
    {
        // 确保白值 > 黑值（重要：需要时交换）
        if(Calibrated_black[i]>=Calibrated_white[i])
        {
            temp=Calibrated_white[i];
            Calibrated_white[i]=Calibrated_black[i];
            Calibrated_black[i]=temp;
        }

        // 计算灰度阈值（1:2和2:1分割点）
        sensor->Gray_white[i]=(Calibrated_white[i]*2+Calibrated_black[i])/3;
        sensor->Gray_black[i]=(Calibrated_white[i]+Calibrated_black[i]*2)/3;

        // 保存校准参数
        sensor->Calibrated_black[i]=Calibrated_black[i];
        sensor->Calibrated_white[i]=Calibrated_white[i];

        // 检查无效校准数据（全白/全黑/相同值）
        if ((Calibrated_white[i] == 0 && Calibrated_black[i] == 0)||
            (Calibrated_white[i]==Calibrated_black[i]))
        {
            sensor->Normal_factor[i] = 0.0;  // 无效通道
            continue;
        }

        // 计算归一化系数
        Normal_Diff[i] = (double)Calibrated_white[i] - (double)Calibrated_black[i];
        sensor->Normal_factor[i] = sensor->bits / Normal_Diff[i];
    }
    sensor->ok=1;  // 标记初始化完成
}

/* 功能说明：灰度传感器任务（无定时器版本）*/
void No_Mcu_Ganv_Sensor_Task_Without_tick(No_MCU_Sensor*sensor)
{
    Get_Analog_value(sensor->Analog_value);  // 采集数据
    convertAnalogToDigital(sensor->Analog_value, sensor->Gray_white,sensor->Gray_black,&sensor->Digtal);// 阈值比较
    normalizeAnalogValues(sensor->Analog_value,  sensor->Normal_factor,sensor->Calibrated_black,sensor->Normal_value,sensor->bits);// 归一化处理
}

/* 功能说明：灰度传感器任务（带定时器版本）*/
void No_Mcu_Ganv_Sensor_Task_With_tick(No_MCU_Sensor*sensor)
{
    if(sensor->Tick>=sensor->Time_out)  // 检查是否达到采样周期
    {
        // 执行数据采集和处理
        Get_Analog_value(sensor->Analog_value);
        convertAnalogToDigital(sensor->Analog_value,sensor->Gray_white,sensor->Gray_black,&sensor->Digtal);
        normalizeAnalogValues(sensor->Analog_value,sensor->Normal_factor,sensor->Calibrated_black,sensor->Normal_value,sensor->bits);
        sensor->Tick=0;  // 重置定时器
    }
}

/* 功能说明：定时器tick计数 */
void Task_tick(No_MCU_Sensor*sensor)
{
    sensor->Tick++;
}

/* 功能说明：获取数字信号状态 */
unsigned char Get_Digtal_For_User(No_MCU_Sensor*sensor)
{
    return sensor->Digtal;  // 返回8位数据状态，每位对应一个传感器
}

/* 功能说明：获取归一化数据
   返回值：1-成功，0-未初始化 */
unsigned char Get_Normalize_For_User(No_MCU_Sensor*sensor,unsigned short* result)
{
    if(!sensor->ok)return 0;
    else
    {
        memcpy(result,sensor->Normal_value,16);  // 拷贝归一化数据
        return 1;
    }
}

/* 功能说明：获取原始模拟数据
   返回值：1-成功，0-未初始化 */
unsigned char Get_Anolog_Value(No_MCU_Sensor*sensor,unsigned short *result)
{
    Get_Analog_value(sensor->Analog_value);  // 重新采集数据
    memcpy(result,sensor->Analog_value,16);
    if(!sensor->ok)return 0;
    else return 1;
}
