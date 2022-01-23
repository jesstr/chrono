#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "stm32l0xx.h"
#include "stm32l0xx_ll_gpio.h"

#define PRESSED(X)        X##_RPESSED
#define RELEASED(X)       X##_RELEASED
#define LONGPRESS(X)      X##_LONGPRESS

typedef enum {
    BTN_ENTER = 0,
    BTN_DOWN,
    BTN_UP,
    MAX_BUTTONS_NUM
} button_id_t;

typedef enum {
    PRESSED(BTN_ENTER) = 0,
    PRESSED(BTN_DOWN),
    PRESSED(BTN_UP),

    RELEASED(BTN_ENTER),
    RELEASED(BTN_DOWN),
    RELEASED(BTN_UP),

    LONGPRESS(BTN_ENTER),
    LONGPRESS(BTN_DOWN),
    LONGPRESS(BTN_UP),

    MAX_BUTTON_EVENTS_NUM
} button_event_t;


typedef struct {
    GPIO_TypeDef *port;
    uint32_t pin;
    uint32_t exti;
    uint32_t t_last;
    osTimerId timerId;
    void (*pressHandler)(button_id_t id, uint32_t lastcall);
    void (*releaseHandler)(button_id_t id, uint32_t lastcall);
    uint16_t period;
    button_id_t id;
    bool inverted;
    bool inlongpress;
} button_t;

extern osMessageQId buttonsQueueHandle;

void Buttons_Init(void);
void Buttons_IRQHandler(void);
bool Buttons_GetState(button_id_t id);


#endif /* BUTTONS_H */