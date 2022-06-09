#include <stdint.h>
#include <stdbool.h>
#include "u8g2.h"
#include "u8g2_cb.h"
#include "lcd.h"
#include "pics.h"
#include "cmsis_os.h"
#include "printf.h"
#include "buttons.h"
#include "buzzer.h"
#include "ts.h"
#include "tim.h"
#include "IRremote.h"

#define OLED_ADDRESS        0x3C

static u8g2_t u8g2;
static ir_decode_results ir_results;


void Lcd_DrawMain(void)
{
    const uint8_t *font = u8g2_font_9x15_tf;
    u8g2_SetFont(&u8g2, font);

    u8g2_FirstPage(&u8g2);
    do {
        u8g2_DrawUTF8Lines(&u8g2, 10, 10, u8g2_GetDisplayWidth(&u8g2), 16, "Ready!");
    } while (u8g2_NextPage(&u8g2));
}


void Lcd_DrawIntro(void)
{
    const uint8_t *font = u8g2_font_9x15_tf;
    u8g2_SetFont(&u8g2, font);

    u8g2_uint_t y_offset = 5;
    u8g2_uint_t line_y = u8g2.height / 2 + bike_height / 2 + y_offset;
    u8g2_uint_t bike_y = u8g2.height / 2 - bike_height / 2 + y_offset;

    for (uint8_t i = 0; i < u8g2.width - bike_width; i += 3) {
        u8g2_FirstPage(&u8g2);
        do {
            u8g2_DrawLine(&u8g2, 0, line_y, i, line_y);
            u8g2_DrawXBM(&u8g2, i, bike_y, bike_width, bike_height, bike_bits);
            if (i + 3 >= u8g2.width - bike_width) {
                u8g2_DrawUTF8Line(&u8g2, 5, line_y - y_offset, u8g2.width - bike_width, "Ready?", 0, 0);
            }
        } while (u8g2_NextPage(&u8g2));
    }
}


void Lcd_DrawIR(void)
{
    char line[24];

    if (IR_decode(&ir_results)) {

        if (ir_results.decode_type != UNKNOWN) {
            /* Check received code */
            if (ir_results.value == 0x00FF38C7) {
                /* Clear timestamp */
                ts.m = 0;
                ts.ms = 0;
                LL_TIM_SetCounter(TIM21, 0);
            }
            sprintf(line, "%s%s\n%u:%u.%03u", IR_getProtocolString(&ir_results),
                ir_results.isRepeat ? "(R)" : "", ts.m, ts.ms / 1000, ts.ms % 1000);
            printf("%u:%u.%03u\r\n", ts.m, ts.ms / 1000, ts.ms % 1000);
        } else {
            sprintf(line, "%s%s\n0x%08X", IR_getProtocolString(&ir_results),
                ir_results.isRepeat ? "(R)" : "", ir_results.value);
        }

        u8g2_FirstPage(&u8g2);
        do {
            u8g2_DrawUTF8Lines(&u8g2, 10, 10, u8g2_GetDisplayWidth(&u8g2), 16, line);
        } while (u8g2_NextPage(&u8g2));

        if (ir_results.decode_type != UNKNOWN) {
            buzzer_beep(10);
        }
        printf("Code: 0x%08X | Type: %s | Address: 0x%02X\r\n",
            ir_results.value, IR_getProtocolString(&ir_results), ir_results.address);

        IR_resume();
    }
}


void Lcd_DrawMsg(char *msg)
{
    u8g2_FirstPage(&u8g2);
    do {
        u8g2_DrawUTF8Lines(&u8g2, 10, 12, u8g2_GetDisplayWidth(&u8g2), 16, msg);
    } while (u8g2_NextPage(&u8g2));
}


void Lcd_Init(void) {
    u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2,
        U8G2_R0,
        u8x8_byte_hw_i2c,
        psoc_gpio_and_delay_cb);

    u8g2_SetI2CAddress(&u8g2, OLED_ADDRESS);

    osDelay(100);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}