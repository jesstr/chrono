#include <stdint.h>
#include <stdbool.h>
#include "stm32l0xx.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_exti.h"
#include "cmsis_os.h"
#include "buttons.h"
#include "printf.h"
#include "main.h"

#define DEBOUNCE_TIMEOUT            50
#define BUTTONS_QUEUE_SIZE          8
#define LONGPRESS_LONG_DELAY        1000
#define LONGPRESS_SHORT_DELAY       (LONGPRESS_LONG_DELAY / 8)

static button_t buttons[MAX_BUTTONS_NUM];

osMessageQId buttonsQueueHandle;
uint8_t buttonsQueueBuffer[BUTTONS_QUEUE_SIZE];
osStaticMessageQDef_t buttonsQueueControlBlock;

osStaticTimerDef_t timerButtonEnterControlBlock;
osStaticTimerDef_t timerButtonDownControlBlock;
osStaticTimerDef_t timerButtonUpControlBlock;

static void Buttons_PressHandler(button_id_t id, uint32_t lastcall);
static void Buttons_ReleaseHandler(button_id_t id, uint32_t lastcall);
static void LongPressHandler(void const *tmr);


void Buttons_Init(void)
{
    osMessageQStaticDef(buttonsQueue, BUTTONS_QUEUE_SIZE,
        uint8_t, buttonsQueueBuffer, &buttonsQueueControlBlock);
    buttonsQueueHandle = osMessageCreate(osMessageQ(buttonsQueue), NULL);

    osTimerStaticDef(timerButtonEnter, LongPressHandler, &timerButtonEnterControlBlock);
    osTimerStaticDef(timerButtonDown, LongPressHandler, &timerButtonDownControlBlock);
    osTimerStaticDef(timerButtonUp, LongPressHandler, &timerButtonUpControlBlock);

    for (button_id_t id = 0; id < MAX_BUTTONS_NUM; id++) {
        buttons[id].id = id;
        buttons[id].port = GPIOB;
        buttons[id].pin = 0;
        buttons[id].exti = 0;
        buttons[id].inlongpress = false;
        buttons[id].inverted = true;
        buttons[id].timerId = NULL;
        buttons[id].period = LONGPRESS_LONG_DELAY;
        buttons[id].pressHandler = Buttons_PressHandler;
        buttons[id].releaseHandler = Buttons_ReleaseHandler;
        buttons[id].t_last = 0;
    }

    buttons[BTN_ENTER].pin = LL_GPIO_PIN_4;
    buttons[BTN_ENTER].exti = LL_EXTI_LINE_4;
    buttons[BTN_ENTER].timerId = osTimerCreate(osTimer(timerButtonEnter), osTimerPeriodic, NULL);

    buttons[BTN_DOWN].pin = LL_GPIO_PIN_5;
    buttons[BTN_DOWN].exti = LL_EXTI_LINE_5;
    buttons[BTN_DOWN].timerId = osTimerCreate(osTimer(timerButtonDown), osTimerPeriodic, NULL);

    buttons[BTN_UP].pin = LL_GPIO_PIN_10;
    buttons[BTN_UP].exti = LL_EXTI_LINE_10;
    buttons[BTN_UP].timerId = osTimerCreate(osTimer(timerButtonUp), osTimerPeriodic, NULL);

    NVIC_SetPriority(EXTI4_15_IRQn, 3);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}


void Buttons_IRQHandler(void)
{
    for (button_id_t id = 0; id < MAX_BUTTONS_NUM; id++) {
        uint32_t exti = buttons[id].exti;
        if (LL_EXTI_IsActiveFlag_0_31(exti)) {
            
            /* Clear flag */
            LL_EXTI_ClearFlag_0_31(exti);

            uint32_t ticks = osKernelSysTick();
            if (ticks - buttons[id].t_last >= DEBOUNCE_TIMEOUT) {
                if (LL_EXTI_IsEnabledFallingTrig_0_31(exti)) {
                    LL_EXTI_DisableFallingTrig_0_31(exti);
                    LL_EXTI_EnableRisingTrig_0_31(exti);
                    if (buttons[id].pressHandler != NULL) {
                        buttons[id].pressHandler(id, buttons[id].t_last);
                    }
                } else if (LL_EXTI_IsEnabledRisingTrig_0_31(exti)) {
                    LL_EXTI_DisableRisingTrig_0_31(exti);
                    LL_EXTI_EnableFallingTrig_0_31(exti);
                    if (buttons[id].releaseHandler != NULL) {
                        buttons[id].releaseHandler(id, buttons[id].t_last);
                    }
                }
                buttons[id].t_last = ticks;
            }
            break;
        }
    }
}


bool Buttons_GetState(button_id_t id)
{
    if (id >= MAX_BUTTONS_NUM) return false;
    return (bool)LL_GPIO_IsInputPinSet(buttons[id].port, buttons[id].pin) ^ 
        buttons[id].inverted;
}


static void LongPressHandler(void const *arg)
{
    osTimerId timer = (osTimerId)arg;
    button_event_t event;
    button_id_t id;

    if (timer == buttons[BTN_ENTER].timerId) {
        event = LONGPRESS(BTN_ENTER);
        id = BTN_ENTER;
    } else if (timer == buttons[BTN_DOWN].timerId) {
        event = LONGPRESS(BTN_DOWN);
        id = BTN_DOWN;
    } else if (timer == buttons[BTN_UP].timerId) {
        event = LONGPRESS(BTN_UP);
        id = BTN_UP;
    } else {
        return;
    }
    buttons[id].inlongpress = true;
    osMessagePut(buttonsQueueHandle, event, 0);
    osSemaphoreRelease(lcdSemHandle);

    if (buttons[id].period > LONGPRESS_SHORT_DELAY) {
        /* Accelerate repeat events speed for all buttons, except ENTER */
        buttons[id].period = (id == BTN_ENTER) ?
            LONGPRESS_LONG_DELAY / 2 : buttons[id].period / 2;
        xTimerChangePeriod(timer, buttons[id].period, 0);
    }
}


static void Buttons_PressHandler(button_id_t id, uint32_t lastcall)
{
    button_event_t event;
    (void)lastcall;

    switch (id) {
        case BTN_ENTER:
            event = PRESSED(BTN_ENTER);
            break;
        case BTN_DOWN:
            event = PRESSED(BTN_DOWN);
            break;
        case BTN_UP:
            event = PRESSED(BTN_UP);
            break;
        default:
            return;
    }
    buttons[id].inlongpress = false;
    osMessagePut(buttonsQueueHandle, event, 0);

    buttons[id].period = LONGPRESS_LONG_DELAY;
    osTimerStart(buttons[id].timerId, buttons[id].period);
}


static void Buttons_ReleaseHandler(button_id_t id, uint32_t lastcall)
{
    button_event_t event;
    (void)lastcall;

    switch (id) {
        case BTN_ENTER:
            event = RELEASED(BTN_ENTER);
            break;
        case BTN_DOWN:
            event = RELEASED(BTN_DOWN);
            break;
        case BTN_UP:
            event = RELEASED(BTN_UP);
            break;
        default:
            return;
    }
    osTimerStop(buttons[id].timerId);

    /* Skip RELEASE event for a longpressed button */
    if (!buttons[id].inlongpress) {
        osMessagePut(buttonsQueueHandle, event, 0);
    }
}