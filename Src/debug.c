#include "debug.h"
#include "common.h"
#include "printf.h"
#include "cmsis_os.h"

#include "stm32l0xx.h"
#include "stm32l0xx_ll_usart.h"

#define DEBUG_USART             USART2
#define DEBUG_TX_TIMEOUT        200

#define MAX_PRINT_DATA_LEN      16

#define DEBUG_TX_QUEUE          uart2TxQueueHandle

extern osMessageQId DEBUG_TX_QUEUE;


void print_data(char *name, uint8_t *data, uint8_t len)
{
    printf("%s (%d) [", name, len);
    for (uint8_t i = 0; i < (len > MAX_PRINT_DATA_LEN ? MAX_PRINT_DATA_LEN - 4 : len); i++) {
        if (i > 0) printf(" ");
        printf("%02x", data[i]);
    }
    if (len > MAX_PRINT_DATA_LEN) {
        printf(" ...");
        for (uint8_t i = len - 4; i < len; i++) {
            printf(" %02x", data[i]);
        }
    }
    printf("]\r\n");
}


void debug_tx_callback(void)
{
    osEvent event = osMessageGet(DEBUG_TX_QUEUE, 0);

    if (event.status == osEventMessage) {
        LL_USART_TransmitData8(DEBUG_USART, (uint8_t)(event.value.v & 0xFF));
    } else {
        LL_USART_DisableIT_TXE(DEBUG_USART);
    }
}


static inline void debug_putc(char c)
{
    osMessagePut(DEBUG_TX_QUEUE, c, DEBUG_TX_TIMEOUT);
    LL_USART_EnableIT_TXE(DEBUG_USART);
}


void _putchar(char character)
{
    debug_putc(character);
}