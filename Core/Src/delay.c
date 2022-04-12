#include <stdint.h>
#include "delay.h"
#include "stm32l0xx_ll_tim.h"


void delay_us(uint32_t us)
{
	uint32_t ts = LL_TIM_GetCounter(TIM6);
	while (LL_TIM_GetCounter(TIM6) - ts < us);
}