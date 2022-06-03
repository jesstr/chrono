#include <stdint.h>
#include <stdbool.h>
#include "u8g2.h"
#include "u8g2_cb.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "printf.h"
#include "buttons.h"
#include "buzzer.h"
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


void Lcd_DrawIR(void)
{
    char line[24];

    if (IR_decode(&ir_results)) {
        sprintf(line, "%s%s\n0x%08X", IR_getProtocolString(&ir_results),
            ir_results.isRepeat ? "(R)" : "", ir_results.value);

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
        u8g2_DrawUTF8Lines(&u8g2, 10, 10, u8g2_GetDisplayWidth(&u8g2), 16, msg);
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