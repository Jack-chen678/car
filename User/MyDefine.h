#ifndef __MYDEFINE_H__
#define __MYDEFINE_H__

/* ========== HAL ��ͷ�ļ� ========== */
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "usart.h"
#include "adc.h"
#include "tim.h"

/* ========== C ����ͷ�ļ� ========== */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== �����ͷ�ļ� ========== */
#include "uart_driver.h"
#include "ringbuffer.h"
#include "encoder_driver.h"
// #include "WouoUI.h"
// #include "WouoUI_user.h"

/* ========== ������ͷ�ļ� ========== */


/* ========== Ӧ�ò�ͷ�ļ� ========== */
#include "uart_app.h"
#include "gray_app.h"
#include "No_MCU_Grayscale.h"
#include "motor_app.h"
#include "encoder_app.h"

/* ========== ���ĵ�����ͷ�ļ� ========== */
#include "Scheduler.h"
extern DMA_HandleTypeDef hdma_usart1_rx;
extern ADC_HandleTypeDef hadc1;
#endif
