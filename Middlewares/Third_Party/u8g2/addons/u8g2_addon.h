#include "u8g2.h"
#include <stdint.h>

typedef struct {
    int16_t value;
    const char *item;
} picklist_t;

int16_t u8g2_GetPicklistIndex(picklist_t *picklist, uint8_t picklist_len, int16_t value);
uint8_t u8g2_UserInterfaceInputExtended(u8g2_t *u8g2, const char *title, const char *pre,
    int16_t *value, int16_t lo, int16_t hi, uint8_t digits, uint8_t delim,
    const char *post, const uint8_t *symbol_font, picklist_t *picklist);