#include <stdint.h>
#include "printf.h"
#include "main.h"
#include "cmsis_os.h"


void buzzer_beep(uint16_t ms)
{
    LL_GPIO_SetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
    osDelay(ms);
    LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
}