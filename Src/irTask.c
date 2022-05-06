#include <stdint.h>
#include "cmsis_os.h"


void irTaskRoutine(void const * argument)
{

  while(1) {

      osDelay(100);
    }
}