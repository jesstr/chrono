#include <stdint.h>
#include "cmsis_os.h"
#include "u8g2.h"
#include "u8x8.h"
#include "i2c.h"


uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;

    switch(msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while (arg_int > 0) {
                buffer[buf_idx++] = *data;
                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            I2C_Send(u8x8_GetI2CAddress(u8x8), (uint8_t *)buffer, buf_idx);
            break;
        default:
        return 0;
    }
    return 1;
}


uint8_t psoc_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg) {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
        /* only support for software I2C */
        break;

        case U8X8_MSG_DELAY_NANO:
        /* not required for SW I2C */
        break;

        case U8X8_MSG_DELAY_10MICRO:
        /* not used at the moment */
        break;

        case U8X8_MSG_DELAY_100NANO:
        /* not used at the moment */
        break;

        case U8X8_MSG_DELAY_MILLI:
        osDelay(arg_int);
        break;

        case U8X8_MSG_DELAY_I2C:
        /* not used at the moment */
        break;

        case U8X8_MSG_GPIO_I2C_CLOCK:
        /* not used at the moment */
        break;

        default:
        break;
    }
    return 1;
}