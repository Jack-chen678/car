/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MyDefine.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CALIBRATION_MODE 0  // 0=正常模式, 1=校准模式
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned short white[8] = {2397,2519,2452,2786,2417,3071,2829,3043}; //白色基准值
unsigned short black[8] = {143,146,150,157,164,171,171,164};    // 黑色基准值
unsigned short Normal[8]={0};
No_MCU_Sensor sensor;               // 初始化传感器结构体
int32_t offset=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int32_t CalculateNormalizedValue(unsigned short normal[8],uint8_t field);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  #if CALIBRATION_MODE
    // ========== 校准模式 ==========
    // 用途：获取黑白线的ADC校准值
    // 使用步骤：
    // 1. 将传感器放在白色区域，上电查看串口打印的ADC值
    // 2. 记录8个通道的值，更新white数组
    // 3. 将传感器放在黑色区域，查看串口打印的ADC值
    // 4. 记录8个通道的值，更新black数组
    // 5. 将CALIBRATION_MODE改为0，重新编译运行

    No_MCU_Ganv_Sensor_Init_Frist(&sensor);  // 首次初始化（不带校准值）

  #else
    // ========== 正常运行模式 ==========
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);  // 使用校准值初始化
    HAL_Delay(10);
  #endif

  Uart_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
  #if CALIBRATION_MODE
    uint8_t Digtal=0;
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Digtal = Get_Digtal_For_User(&sensor);
    // 校准模式：每500ms打印一次原始ADC值
//    Uart_Printf(&huart1, "ADC: %d,%d,%d,%d,%d,%d,%d,%d\r\n",
//                sensor.Analog_value[0], sensor.Analog_value[1],
//                sensor.Analog_value[2], sensor.Analog_value[3],
//                sensor.Analog_value[4], sensor.Analog_value[5],
//                sensor.Analog_value[6], sensor.Analog_value[7]);
      
    
    //HAL_Delay(500);
    #else
    //
//    uint8_t Digtal=0;
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
//    Digtal = ~Get_Digtal_For_User(&sensor);
//    Uart_Printf(&huart1, "Digital: %d-%d-%d-%d-%d-%d-%d-%d\r\n",
//                (Digtal>>0)&0x01, (Digtal>>1)&0x01,
//                (Digtal>>2)&0x01, (Digtal>>3)&0x01,
//                (Digtal>>4)&0x01, (Digtal>>5)&0x01,
//                (Digtal>>6)&0x01, (Digtal>>7)&0x01);
  #endif
    Get_Normalize_For_User(&sensor, Normal);
    offset=CalculateNormalizedValue(Normal,1);
    Uart_Printf(&huart1,"%d\r\n",offset);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int32_t CalculateNormalizedValue(unsigned short normal[8],uint8_t field)
{
    // 定义权值数组，从左到右对应-7,-5,-3,-1,1,3,5,7
    const short weights[8] = {-7, -5, -3, -1, 1, 3, 5, 7};
    
    float weighted_sum = 0.0f;          // 加权和
    float original_sum = 0.0f; // 原始数据和
    static float last_value = 0.0f;   // 上一次的值
    static float filter_value = 0.0f;  // 滤波后的值
    const float filter_alpha = 0.3f;   // 滤波系数 (0.1-0.5，越小越平滑)
    
    // 计算加权和和原始数据和
    for (int i = 0; i < 8; i++) {
        float value = field ? (4096.0f - normal[i]) : normal[i];
        weighted_sum += value * weights[i];  // 每个数据乘以对应权值
        original_sum += value;               // 累加原始数据
    }
    
    // 计算并返回归一化值
    if (original_sum != 0.0f) { // 避免除以0的情况
        last_value = weighted_sum / original_sum;

        // 一阶低通滤波，减少数据波动
        filter_value = filter_alpha * last_value + (1.0f - filter_alpha) * filter_value;

        return (int32_t)(filter_value * 1000); // 放大1000倍返回，保持精度
    } else {
        return (int32_t)(filter_value * 1000); // 如果原始数据和为0，返回滤波后的值
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
