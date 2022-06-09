#include <stdint.h>
#include "main.h"
#include "cmsis_os.h"
#include "u8g2.h"
#include "lcd.h"
#include "buttons.h"
#include "printf.h"
#include "IRremote.h"
// #define osObjectExternal
#include "cmsis_os.h"

// extern osSemaphoreDef(lcdSem);
// extern const osSemaphoreDef_t os_semaphore_def_lcdSem;



#define SCREEN_UPDATE_TIMEOUT   0


void displayTaskRoutine(void const * argument) {
    osEvent event;

    Lcd_Init();
    Buttons_Init();

    Lcd_DrawIntro();

    IR_enableIRIn();
    IR_blink(true);

    while (1) {

        if (osSemaphoreWait(lcdSemHandle, portMAX_DELAY) == osOK) {

            /* Draw received IR code if any */
            Lcd_DrawIR();

            /* Check pressed button */
            event = osMessageGet(buttonsQueueHandle, SCREEN_UPDATE_TIMEOUT);
            if (event.status == osEventMessage) {

                /* Debug output */
                char msg[8];
                static uint8_t n = 0;
                if (event.value.v == LONGPRESS(BTN_DOWN)) {
                    n++;
                } else {
                    n = 0;
                }
                snprintf(msg, sizeof(msg), "%d [%d]", event.value.v, n);
                Lcd_DrawMsg(msg);

                if (event.value.v == RELEASED(BTN_ENTER)) {
                    ;
                }
            }
        }
    }
}
