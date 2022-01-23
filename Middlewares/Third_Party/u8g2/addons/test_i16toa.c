/*
Test routine for u8x8_i16toa() function.
To build and run the test, run folowing command:
"gcc -DTEST_I16TOA test_i16toa.c u8x8_i16toa.c -o test.o && ./test.o"
*/
#ifdef TEST_I16TOA
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern const char *u8x8_itoa(int16_t v);
extern const char *u8x8_i16toa(int16_t v, uint8_t d);


int main(char *argv, int argc)
{
    char ref[8];
    for (int16_t i = INT16_MIN + 1; i < INT16_MAX; i++) {
        sprintf(ref, "%d", i);
        if (strcmp(ref, u8x8_itoa(i)) != 0) {
            printf("FAILED at i=%i\r\n", i);
            return 1;
        }
    }

    /* Special case -32768 -> -32767 */
    sprintf(ref, "%d", INT16_MIN);
    if (strcmp(ref, u8x8_itoa(INT16_MIN)) == 5) {
        printf("FAILED at i=%i\r\n", INT16_MIN);
        return 1;
    }

    printf("PASSED\r\n");
}

#endif /* TEST_I16TOA */