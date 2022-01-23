/*

  u8g2_input_extended.c
  
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
*/

#include "u8g2.h"
#include "u8x8_addon.h"
#include "u8g2_addon.h"

#define ABS(X) (((X) < 0) ? -(X) : (X))


int16_t u8g2_GetPicklistIndex(picklist_t *picklist, uint8_t picklist_len, int16_t value)
{
  uint8_t i = 0;

  if (value < 0) {
    return -1;
  }
  do {
    if (picklist[i].value == value) {
      return i;
    }
  } while (i++ < picklist_len);

  return -1;
}

/*
  return:
    0: value is not changed (HOME/Break Button pressed)
    1: value has been updated
*/
uint8_t u8g2_UserInterfaceInputExtended(u8g2_t *u8g2, const char *title, const char *pre,
  int16_t *value, int16_t lo, int16_t hi, uint8_t digits, uint8_t delim,
  const char *post, const uint8_t *symbols_font, picklist_t *picklist)
{
  uint8_t line_height;
  uint8_t height;
  u8g2_uint_t pixel_height;
  u8g2_uint_t  y, yy;
  u8g2_uint_t  pixel_width;
  u8g2_uint_t  x, xx;
  u8g2_uint_t  neg_offs;
  const uint8_t *title_font = u8g2->font;

  int16_t local_value = *value;
  uint8_t event;

  if (delim == 0) delim = 1;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* force baseline position */
  u8g2_SetFontPosBaseline(u8g2);

  /* calculate line height */
  line_height = u8g2_GetAscent(u8g2);
  line_height -= u8g2_GetDescent(u8g2);


  /* calculate overall height of the input value box */
  height = 1;	/* value input line */
  height += u8x8_GetStringLineCnt(title);

  /* calculate the height in pixel */
  pixel_height = height;
  pixel_height *= line_height;


  /* calculate offset from top */
  y = 0;
  if ( pixel_height < u8g2_GetDisplayHeight(u8g2)  )
  {
    y = u8g2_GetDisplayHeight(u8g2);
    y -= pixel_height;
    y /= 2;
  }
  
  /* calculate offset from left for the label */
  x = 0;
  neg_offs = 0;
  
  if (symbols_font != NULL) {
    u8g2_SetFont(u8g2, symbols_font);
  }
  pixel_width = u8g2_GetUTF8Width(u8g2, pre);
  if (local_value < 0) neg_offs = u8g2_GetUTF8Width(u8g2, "-");
  if (delim > 1) pixel_width += u8g2_GetUTF8Width(u8g2, ".");
  pixel_width += u8g2_GetUTF8Width(u8g2, "0") * digits;
  pixel_width += u8g2_GetUTF8Width(u8g2, post);
  pixel_width += neg_offs;
  if ( pixel_width < u8g2_GetDisplayWidth(u8g2) )
  {
    x = u8g2_GetDisplayWidth(u8g2);
    x -= pixel_width;
    x /= 2;
  }

  /* event loop */
  for(;;)
  {
    u8g2_FirstPage(u8g2);
    do
    {
      /* render */
      yy = y;
      u8g2_SetFont(u8g2, title_font);
      yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
      xx = x;
      if (symbols_font != NULL) {
        u8g2_SetFont(u8g2, symbols_font);
      }
      if (picklist == NULL) {
        if (local_value < 0 && neg_offs == 0) xx -= u8g2_GetUTF8Width(u8g2, "-") / 2;
        if (local_value >= 0 && neg_offs != 0) xx += u8g2_GetUTF8Width(u8g2, "-") / 2;
        xx += u8g2_DrawUTF8(u8g2, xx, yy, pre);
        if (delim > 1) {
          if ((local_value < 0) && (local_value / delim == 0)) {
            xx += u8g2_DrawUTF8(u8g2, xx, yy, "-");
          }
          xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_i16toa(local_value / delim, digits - 1));
          xx += u8g2_DrawUTF8(u8g2, xx, yy, ".");
          xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_i16toa(ABS(local_value % delim), 1));
        } else {
          xx += u8g2_DrawUTF8(u8g2, xx, yy, u8x8_i16toa(local_value, digits));
        }
        u8g2_DrawUTF8(u8g2, xx, yy, post);
      } else {
        if (picklist && picklist[local_value].item) {
          x = u8g2_GetDisplayWidth(u8g2);
          x -= u8g2_GetUTF8Width(u8g2, picklist[local_value].item);;
          x /= 2;
          u8g2_DrawUTF8(u8g2, x, yy, picklist[local_value].item);
        }
      }
    } while( u8g2_NextPage(u8g2) );

#ifdef U8G2_REF_MAN_PIC
      return 0;
#endif

    for(;;)
    {
      event = u8x8_GetMenuEvent(u8g2_GetU8x8(u8g2));
      if ( event == U8X8_MSG_GPIO_MENU_SELECT )
      {
        *value = local_value;
        return 1;
      }
      else if ( event == U8X8_MSG_GPIO_MENU_HOME )
      {
        return 0;
      }
      else if ( event == U8X8_MSG_GPIO_MENU_PREV || event == U8X8_MSG_GPIO_MENU_UP )
      {
        if (picklist != NULL) {
          uint8_t i = hi > lo ? hi - lo : 0;
          do {
            if ( local_value >= hi ) {
              local_value = lo;
            } else { 
              local_value++;
            }
          } while (picklist[local_value].item == NULL && i--);
        } else {
          if ( local_value >= hi )
            local_value = lo;
          else {
            local_value = (event == U8X8_MSG_GPIO_MENU_UP) ? local_value + 10 : local_value + 1;
            local_value = (local_value > hi) ? lo : local_value;
          }
        }
        break;
      }
      else if ( event == U8X8_MSG_GPIO_MENU_NEXT || event == U8X8_MSG_GPIO_MENU_DOWN )
      {
        if (picklist != NULL) {
          uint8_t i = hi > lo ? hi - lo : 0;
          do {
            if ( local_value <= lo ) {
              local_value = hi;
            } else {
              local_value--;
            }
          } while (picklist[local_value].item == NULL && i--);
        } else {
          if ( local_value <= lo )
            local_value = hi;
          else {
            local_value = (event == U8X8_MSG_GPIO_MENU_DOWN) ? local_value - 10 : local_value - 1;
            local_value = (local_value < lo) ? hi : local_value;
          }
        }
        break;
      }
    }
  }
}