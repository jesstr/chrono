#include <stdint.h>
#include "cmsis_os.h"
#include "printf.h"
#include "debug.h"
#include "IRremote.h"

decode_results results;


void irTaskRoutine(void const * argument)
{
  static const char *decode_str[] = {
    "UNUSED", "UNKNOWN", "RC5", "RC6", "NEC", "SONY", "PANASONIC", "JVC",
    "SAMSUNG", "WHYNTER", "AIWA_RC_T501", "LG", "SANYO", "MITSUBISHI", "DISH",
    "SHARP", "DENON", "PRONTO"
  };

  my_enableIRIn(); // инициализация

  while (1)
  {
    ////////////////////////////////////////// приём-декодирование ///////////////////////////////////////////////////
    if(my_decode(&results))
    {
      ////////// вывод декодированых данных ///////////////
      printf("Code: %X | Type: %s | address: %X | Bits: %d\r\n",
        results.value, decode_str[results.decode_type + 1], results.address, results.bits);
      print_data("\traw:", results.rawbuf, results.rawlen);
      

      ////////// вывод данных в "сыром" виде ///////////////
      /*snprintf(trans_str, 96, "Raw: %d\n", results.rawlen);
      HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);

      for(int i = 1; i < results.rawlen; i++)
      {
        if(i & 1)
        {
          snprintf(trans_str, 96, "%d", results.rawbuf[i] * USECPERTICK);
          HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);
        }
        else
        {
          HAL_UART_Transmit(&huart1, (uint8_t*)"-", 1, 100);
          snprintf(trans_str, 96, "%lu", (uint32_t)results.rawbuf[i] * USECPERTICK);
          HAL_UART_Transmit(&huart1, (uint8_t*)trans_str, strlen(trans_str), 100);
        }

        HAL_UART_Transmit(&huart1, (uint8_t*)" ", 1, 100);
      }

      HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n-----------------\r\n", strlen("\r\n-----------------\r\n"), 100);*/
      ////////// конец вывода данных в "сыром" виде ///////////////

      osDelay(300); // небольшая пауза (типа от "дребезга"), а то будет ловить следующий сигнал
      my_resume(); // рестарт приёма (нужно обязательно)
    } else {
      // printf("%u\r\n", TIM2->CNT);
      // osDelay(100);
    }

    ////////////////////////////////////////// отправка декодированного сигнала ///////////////////////////////////////////////////
    // Cod: 0x707048b7 | Type: SAMSUNG | Bits: 32
    /*uint32_t cod_but = 0;
    cod_but = 0x707048b7; // код кнопки полученый при декодировании

    sendSAMSUNG(cod_but, 32); // протокол полученый при декодировании, код кнопки и длина пакета в битах (Cod: 0x707048b7 | Type: SAMSUNG | Bits: 32)
    my_enableIRIn(); // переинициализирование приёма (нужна обязательно)
    HAL_Delay(2000);*/

    /////////////////////////////////// отправка НЕ декодированного сигнала, в сыром виде /////////////////////////////////////////
    // Cod: 0xa32ab931 | Type: UNKNOWN | Bits: 32
    // Raw: 24
    // 1300 -400 1300 -400 450 -1200 1300 -400 1300 -400 450 -1200 500 -1200 450 -1250 450 -1200 500 -1200 450 -1250 1300

    /*uint8_t khz = 38; // частоту указать экспериментальным путём, основные используемые от 36 до 40 кГц
    unsigned int raw_signal[] = {1300, 400, 1300, 400, 450, 1200, 1300, 400, 1300, 400, 450, 1200, 500, 1200, 450, 1250, 450, 1200, 500, 1200, 450, 1250, 1300};

    sendRaw(raw_signal, sizeof(raw_signal) / sizeof(raw_signal[0]), khz);
    my_enableIRIn(); // переинициализирование приёма (нужна обязательно)
    HAL_Delay(2000);*/
  }
}