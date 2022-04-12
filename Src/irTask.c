#include <stdint.h>
#include "cmsis_os.h"
#include "printf.h"
#include "debug.h"
#include "IRremote.h"

decode_results results;


void irTaskRoutine(void const * argument)
{
  enableIRIn();
  blink(true);

  printf("Waiting IR signal...\r\n");

  while(1) {
    if (decode()) {
      printf("Code: 0x%08X | Type: %s | Address: 0x%02X\r\n",
        results.value, getProtocolString(), results.address);

#if 0
      printf("Raw: %d\r\n", results.rawlen - 1);
      printf("-----------------\r\n");
      for (uint16_t i = 1; i < results.rawlen; i++) {
        if (i > 1 && (i - 1) % 8 == 0) printf("\r\n");
        printf(i & 1 ? "+%04u " : "-%04u ", results.rawbuf[i] * MICROS_PER_TICK);
      }
      printf("\r\n-----------------\r\n");
#endif

      resume();
    } else {
      osDelay(100);
    }
  }
}