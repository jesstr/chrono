#include <stdbool.h>
#include "u8g2.h"

#define LCD_WAIT_SETTINGS   0

#define STATIC_BIT      0
#define SENSORS_BIT     1
#define LOADS_BIT       2
#define PHASES_BIT      3

extern uint8_t data_req_flags;

void Lcd_Init(void);
void Lcd_DrawMain(void);
void Lcd_DrawMenu(void);