/*

  u8x8_i16toa.c

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

#include "u8x8.h"

/* Note: -32768 is always rounded to -32767 */
const char *u8x8_i16toap(char * dest, int16_t v)
{
  uint8_t pos = 5;
  char * pdest = dest;
  uint8_t d;
  int16_t c;
  c = 10000;
  if (v == INT16_MIN) {
    /* Special case: round to the
      max opposite positive  */
    v = -INT16_MAX;
  }
  if (v < 0) {
    *dest++ = '-';
    v = -v;
  }
  while(pos--) {
      d = '0';
      while(v >= c) {
        v -= c;
        d++;
      }
      *dest++ = d;
      c /= 10;
  }
  *dest = '\0';
  return pdest;
}

/* v = value, d = number of digits */
const char *u8x8_i16toa(int16_t v, uint8_t d)
{
  static char buf[7];
  d = 5-d;
  char *ret = (char *)u8x8_i16toap(buf, v) + d;
  if (v < 0)
    *ret = '-';
  return ret;
}

const char *u8x8_itoa(int16_t v)
{
  char *s = (char *)u8x8_i16toa(v, 5);
  while(*s == '-' && *(s + 1) == '0') {
    s++;
    *s = '-';
  }
  while(*s == '0')
    s++;
  if (*s == '\0')
    s--;
  return s;
}