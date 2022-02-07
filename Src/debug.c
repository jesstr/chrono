#include "debug.h"
#include "common.h"
#include "printf.h"
#include "cmsis_os.h"

#include "stm32l0xx.h"
#include "stm32l0xx_ll_usart.h"

#define DEBUG_USART             USART2
#define DEBUG_TX_TIMEOUT        200

extern osMessageQId uart2TxQueueHandle;

#define DEBUG_TX_QUEUE  uart2TxQueueHandle


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