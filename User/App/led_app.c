#include "led_app.h"

uint8_t led_buf[4] = {0, 0, 0, 0};
extern void Led_Display(unsigned char* led_buf, bool enable);

void Led_Init(void)
{
    Led_Display(led_buf, 1);
}

void Led_Task(void)
{
    Led_Display(led_buf, 1);
}
