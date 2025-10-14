#include "No_MCU_Grayscale.h"
uint32_t dma_buff[91]={0};
uint32_t gAdcResult =0;

// 添加微秒延时函数（更精确）
static void delay_us(uint32_t us)
{
  uint32_t ticks = us * (SystemCoreClock / 1000000) / 5;  //  约5个时钟周期/循环

  while(ticks--) {
      __NOP();
  }
}
volatile bool gCheckADC=false;
/**
 * @brief  获取ADC采样值（DMA+中断方式，多采样求平均）
 * @param  number - 采样次数（此处实际使用90次，与DMA缓冲区长度匹配）
 * @return 采样计算后的平均值（超时返回0，代表采样失败）
 */
unsigned int adc_getValue(unsigned int number)
{
    // 清空采样完成标志，为新一轮ADC采集做准备
    gCheckADC = false;

    // 清空DMA接收缓冲区，避免旧采样数据干扰新数据
    memset((uint16_t*)dma_buff, 0, sizeof(dma_buff));

    // 启动ADC DMA采集（中断触发方式，采集完成后进入中断回调）
    HAL_ADC_Start_DMA(&hadc1, dma_buff, 90);

    // 等待中断回调设置gCheckADC标志（最多等待，防止程序死等）
    uint16_t timeout = 2000; 
    while(gCheckADC == false && timeout > 0)
    {
        timeout--;
    }

    // 判断是否超时：若超时，停止DMA并返回无效值0
    if(timeout == 0)
    {
        HAL_ADC_Stop_DMA(&hadc1);
        return 0;  // 超时返回0，标识采样失败
    }
    
    // 中断已完成（采样成功），清空标志确保下次采样正常，返回计算好的平均值
    gCheckADC = false;  // 重置采样完成标志，为下次采集做准备
    uint32_t sum = 0; 
    for (int i = 0; i < number; i++) 
    {
        sum += dma_buff[i];
    }
    gAdcResult = sum / number;
    return gAdcResult;
}

/**
 * @brief  采集8路通道的ADC原始模拟值，并按方向存储到数组
 * @param  result - 输出参数，用于存储8路通道的ADC原始值
 * @note   通过3位地址线（Address_0~2）控制多路选择器，实现8路通道切换；
 *         支持正序/逆序存储，由全局变量Direction控制（0=正序，1=逆序）
 */
void Get_Analog_value(unsigned short *result)
{
    unsigned char i;
    unsigned int Anolag = 0;  // 临时存储单路ADC采样结果

    // 循环8次，通过3位地址线（bit0~bit2）切换8路采集通道
    for(i = 0; i < 8; i++)
    {
        // 地址线电平控制：通过i的对应bit位反相，切换多路选择器的当前通道
        Switch_Address_0(!(i & 0x01));  // 地址线0 <-> i的bit0（控制第1位通道选择）
        Switch_Address_1(!(i & 0x02));  // 地址线1 <-> i的bit1（控制第2位通道选择）
        Switch_Address_2(!(i & 0x04));  // 地址线2 <-> i的bit2（控制第3位通道选择）

        // 关键延时：地址切换后，需等待多路选择器和ADC输入信号稳定（避免信号抖动导致采样误差）
        delay_us(200);  // ms延时确保信号稳定
        Anolag = adc_getValue(90);  // 采集当前通道的ADC值（90次采样求平均，提高精度）

        // 根据Direction控制存储顺序：正序（i→result[i]）或逆序（i→result[7-i]）
        if(!Direction)
            result[i] = Anolag;        // Direction=0：正序存储（通道0→result[0]，通道1→result[1]...）
        else
            result[7 - i] = Anolag;    // Direction=1：逆序存储（通道0→result[7]，通道1→result[6]...）

        Anolag = 0;  // 清空临时变量，避免干扰下一路采样
    }
}

/* 函数功能：将模拟值转换为数字信号（二值化处理）
   参数说明：
   adc_value - 原始ADC值数组
   Gray_white - 白色阈值数组
   Gray_black - 黑色阈值数组
   Digital - 输出的数字信号（按位表示） */
void convertAnalogToDigital(unsigned short *adc_value,unsigned short *Gray_white,unsigned short *Gray_black,unsigned char *Digital)
{
    
    for (int i = 0; i < 8; i++) {
        if (adc_value[i] > Gray_white[i]) {
            *Digital |= (1 << i);   // 超过白阈值置1（白色）
        } else if (adc_value[i] < Gray_black[i]) {
            *Digital &= ~(1 << i);  // 低于黑阈值置0（黑色）
        }
        // 中间灰度值保持原有状态
    }
}

/* 函数功能：归一化ADC值到指定范围
   参数说明：
   adc_value - 原始ADC值数组
   Normal_factor - 归一化系数数组
   Calibrated_black - 校准黑值数组
   result - 存储归一化结果的数组
   bits - ADC最大量程值（如255/1024等） */
void normalizeAnalogValues(unsigned short *adc_value,double *Normal_factor,unsigned short *Calibrated_black,unsigned short *result,double bits)
{
    for (int i = 0; i < 8; i++) {
        unsigned short n ;
        // 计算归一化值（减去黑电平后缩放）
        if(adc_value[i]<Calibrated_black[i]) n=0;  // 低于黑电平归零
        else n = (adc_value[i] - Calibrated_black[i]) * Normal_factor[i];

        // 限幅处理
        if (n > bits) {
            n = bits;
        }
        result[i]=n;
    }
}

/* 函数功能：传感器结构体初始化（首次初始化）
   参数说明：sensor - 传感器结构体指针 */
void No_MCU_Ganv_Sensor_Init_Frist(No_MCU_Sensor*sensor)
{
    // 清零所有校准数据和状态
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

/* 函数功能：传感器完整初始化（带校准参数）
   参数说明：
   sensor - 传感器结构体指针
   Calibrated_white - 校准白值数组
   Calibrated_black - 校准黑值数组 */
void No_MCU_Ganv_Sensor_Init(No_MCU_Sensor*sensor,unsigned short *Calibrated_white,unsigned short *Calibrated_black)
{
    No_MCU_Ganv_Sensor_Init_Frist(sensor);
    
    // 根据配置设置ADC量程
    if(Sensor_ADCbits==_8Bits)sensor->bits=255.0;
    else if(Sensor_ADCbits==_10Bits)sensor->bits=1024.0;
    else if(Sensor_ADCbits==_12Bits)sensor->bits=4096.0;
    else if(Sensor_ADCbits==_14Bits)sensor->bits=16384.0;

    // 设置采样超时时间
    if(Sensor_Edition==Class)sensor->Time_out=1;
    else sensor->Time_out=10;

    double Normal_Diff[8];
    unsigned short temp;
    
    for (int i = 0; i < 8; i++)
    {
        // 确保白值 > 黑值（必要时交换）
        if(Calibrated_black[i]>=Calibrated_white[i])
        {
            temp=Calibrated_white[i];
            Calibrated_white[i]=Calibrated_black[i];
            Calibrated_black[i]=temp;
        }

        // 计算灰度阈值（1:2和2:1分界点）
        sensor->Gray_white[i]=(Calibrated_white[i]*2+Calibrated_black[i])/3;
        sensor->Gray_black[i]=(Calibrated_white[i]+Calibrated_black[i]*2)/3;

        // 保存校准数据
        sensor->Calibrated_black[i]=Calibrated_black[i];
        sensor->Calibrated_white[i]=Calibrated_white[i];

        // 处理无效校准数据（全黑/全白/相等情况）
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

/* 函数功能：传感器主任务（无定时器版本）*/
void No_Mcu_Ganv_Sensor_Task_Without_tick(No_MCU_Sensor*sensor)
{
    Get_Analog_value(sensor->Analog_value);  // 采集数据
    convertAnalogToDigital(sensor->Analog_value, sensor->Gray_white,sensor->Gray_black,&sensor->Digtal);// 二值化处理
    normalizeAnalogValues(sensor->Analog_value,  sensor->Normal_factor,sensor->Calibrated_black,sensor->Normal_value,sensor->bits);// 归一化处理
}

/* 函数功能：传感器主任务（带定时器版本）*/
void No_Mcu_Ganv_Sensor_Task_With_tick(No_MCU_Sensor*sensor)
{
    if(sensor->Tick>=sensor->Time_out)  // 检查是否到达采样周期
    {
        // 执行数据采集和处理
        Get_Analog_value(sensor->Analog_value);
        convertAnalogToDigital(sensor->Analog_value,sensor->Gray_white,sensor->Gray_black,&sensor->Digtal);
        normalizeAnalogValues(sensor->Analog_value,sensor->Normal_factor,sensor->Calibrated_black,sensor->Normal_value,sensor->bits);  
        sensor->Tick=0;  // 重置定时器
    }
}

/* 函数功能：定时器tick递增 */
void Task_tick(No_MCU_Sensor*sensor)
{
    sensor->Tick++;
}

/* 函数功能：获取数字信号状态 */
unsigned char Get_Digtal_For_User(No_MCU_Sensor*sensor)
{
    return sensor->Digtal;  // 返回8位数字状态（每位对应一个传感器）
}

/* 函数功能：获取归一化后的数据
   返回值：1-成功 0-未初始化 */
unsigned char Get_Normalize_For_User(No_MCU_Sensor*sensor,unsigned short* result)
{
    if(!sensor->ok)return 0;
    else 
    {
        memcpy(result,sensor->Normal_value,16);  // 拷贝归一化数据
        return 1;     
    }
}

/* 函数功能：获取原始校准数据
   返回值：1-成功 0-未初始化 */
unsigned char Get_Anolog_Value(No_MCU_Sensor*sensor,unsigned short *result)
{   
    Get_Analog_value(sensor->Analog_value);  // 重新采集数据
    memcpy(result,sensor->Analog_value,16);
    if(!sensor->ok)return 0;
    else return 1;
}