#include <stdint.h>
#include "main.h"
#include "cmsis_os.h"
#include "u8g2.h"
#include "lcd.h"
#include "buttons.h"

#define SCREEN_UPDATE_TIMEOUT   50


void displayTaskRoutine(void const * argument) {
    osEvent event;

    Lcd_Init();
    Buttons_Init();

    Lcd_DrawMain();

    enableIRIn();
    blink(true);

    while (1) {
        Lcd_DrawIR();

        /* Check pressed button */
        event = osMessageGet(buttonsQueueHandle, SCREEN_UPDATE_TIMEOUT);
        if (event.status == osEventMessage) {
            if (event.value.v == RELEASED(BTN_ENTER)) {
                ;
            }
        }
    }
}
