#include <stdint.h>
#include <stdbool.h>
#include "u8g2.h"
#include "u8g2_cb.h"
#include "lcd.h"
#include "cmsis_os.h"
#include "printf.h"
#include "buttons.h"

#define OLED_ADDRESS        0x3C

static u8g2_t u8g2;


void Lcd_DrawMain(void)
{

static void DrawMain230V(void)
{
    char buf[16];
    const uint8_t *font = u8g2_font_9x15_tf;

    u8g2_SetFont(&u8g2, font);

    uint8_t y = 50;

    u8g2_ClearBuffer(&u8g2);

    u8g2_DrawUTF8Line(&u8g2, 0, y, u8g2_GetDisplayWidth(&u8g2), buf, 0, false);

    u8g2_SendBuffer(&u8g2);
}


void Lcd_Init(void) {
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(&u8g2,
        U8G2_R0,
        u8x8_byte_hw_i2c,
        psoc_gpio_and_delay_cb);

    u8g2_SetI2CAddress(&u8g2, OLED_ADDRESS * 2);

    osDelay(100);
    u8g2_InitDisplay(&u8g2);
    // u8g2_SetPowerSave(&u8g2, 0);
}