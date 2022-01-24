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
    char buf[16];
    const uint8_t *font = u8g2_font_9x15_tf;

    for (uint8_t i = 0; i < 46; i++) {
        // u8g2_ClearBuffer(&u8g2);
        u8g2_FirstPage(&u8g2);
        do {
            u8g2_SetFont(&u8g2, font);
            u8g2_DrawStr(&u8g2, 10, i, "Hello World!");

        } while (u8g2_NextPage(&u8g2));
        // u8g2_SendBuffer(&u8g2);
    }

}


void Lcd_Init(void) {
    u8g2_Setup_ssd1306_i2c_128x32_univision_1(&u8g2,
        U8G2_R0,
        u8x8_byte_hw_i2c,
        psoc_gpio_and_delay_cb);

    u8g2_SetI2CAddress(&u8g2, OLED_ADDRESS << 1);

    osDelay(100);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}