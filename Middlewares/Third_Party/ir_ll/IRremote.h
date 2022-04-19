/**
 * @file IRremote.h
 * @brief Public API to the library.
 */

//******************************************************************************
// IRremote
// Version 2.0.1 June, 2015
// Initially coded 2009 Ken Shirriff http://www.righto.com
// Edited by Mitra to add new controller SANYO
//
// Interrupt code based on NECIRrcv by Joe Knapp
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556
// Also influenced by http://zovirl.com/2008/11/12/building-a-universal-remote-with-an-arduino/
//
// JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
// LG added by Darryl Smith (based on the JVC protocol)
// Whynter A/C ARC-110WD added by Francesco Meschia
// MagiQuest added by E. Stuart Hicks (based on code by mpflaga - https://github.com/mpflaga/Arduino-IRremote/)
//******************************************************************************
#ifndef IRremote_h
#define IRremote_h

//------------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "private/IRremoteInt.h"
#include "printf.h"

/****************************************************
 *                     PROTOCOLS
 ****************************************************/
//------------------------------------------------------------------------------
// Supported IR protocols
// Each protocol you include costs memory and, during decode, costs time
// Disable (set to 0) all the protocols you do not need/want!
//
#define DECODE_BOSEWAVE      1
#define SEND_BOSEWAVE        1

#define DECODE_DENON         1
#define SEND_DENON           1

#define DECODE_DISH          0 // NOT WRITTEN
#define SEND_DISH            1

#define DECODE_JVC           1
#define SEND_JVC             1

#define DECODE_LEGO_PF       1
#define SEND_LEGO_PF         1

#define DECODE_LG            1
#define SEND_LG              1

#define DECODE_MAGIQUEST     1
#define SEND_MAGIQUEST       1

//#define USE_NEC_STANDARD // remove comment to have the standard NEC decoding (LSB first) available.
#if defined(USE_NEC_STANDARD)
#define DECODE_NEC_STANDARD  1
#define DECODE_NEC           0
#define LSB_FIRST_REQUIRED
#else
#define DECODE_NEC_STANDARD  0
#define DECODE_NEC           1
#endif
#define SEND_NEC             1
#define SEND_NEC_STANDARD    1

#define DECODE_PANASONIC     1
#define SEND_PANASONIC       1

#define DECODE_RC5           1
#define SEND_RC5             1

#define DECODE_RC6           1
#define SEND_RC6             1

#define DECODE_SAMSUNG       1
#define SEND_SAMSUNG         1

#define DECODE_SANYO         1
#define SEND_SANYO           0 // NOT WRITTEN

#define DECODE_SHARP         1
#define SEND_SHARP           1

#define DECODE_SHARP_ALT     1
#define SEND_SHARP_ALT       1
#if SEND_SHARP_ALT
#define LSB_FIRST_REQUIRED
#endif

#define DECODE_SONY          1
#define SEND_SONY            1

#define DECODE_WHYNTER       1
#define SEND_WHYNTER         1

#define DECODE_HASH          1 // special decoder for all protocols

/**
 * An enum consisting of all supported formats.
 * You do NOT need to remove entries from this list when disabling protocols!
 */
typedef enum {
    UNKNOWN = -1,
    UNUSED = 0,
    BOSEWAVE,
    DENON,
    DISH,
    JVC,
    LEGO_PF,
    LG,
    MAGIQUEST,
    NEC_STANDARD,
    NEC,
    PANASONIC,
    RC5,
    RC6,
    SAMSUNG,
    SANYO,
    SHARP,
    SHARP_ALT,
    SONY,
    WHYNTER,
} decode_type_t;

/**
 * Comment this out for lots of lovely debug output.
 */
// #define DEBUG 1
//------------------------------------------------------------------------------
// Debug directives
//
#ifdef DEBUG
#  define DBG_PRINT(...)    printf(__VA_ARGS__)
#  define DBG_PRINTLN(...)  printf(__VA_ARGS__);printf("\r\n")
#else
/**
 * If DEBUG, print the arguments, otherwise do nothing.
 */
#  define DBG_PRINT(...)
/**
 * If DEBUG, print the arguments as a line, otherwise do nothing.
 */
#  define DBG_PRINTLN(...)
#endif

//------------------------------------------------------------------------------
// Helper macro for getting a macro definition as string
//
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

//------------------------------------------------------------------------------
// Mark & Space matching functions
//
int MATCH(unsigned int measured, unsigned int desired);
int MATCH_MARK(uint16_t measured_ticks, unsigned int desired_us);
int MATCH_SPACE(uint16_t measured_ticks, unsigned int desired_us);

/****************************************************
 *                     RECEIVING
 ****************************************************/
/**
 * Results returned from the decoder
 */
typedef struct {
    decode_type_t decode_type;  ///< UNKNOWN, NEC, SONY, RC5, ...
    uint16_t address;           ///< Used by Panasonic & Sharp6 NEC_standard [16-bits]
    uint32_t value;             ///< Decoded value / command [max 32-bits]
    uint16_t bits;              ///< Number of bits in decoded value
    uint16_t magnitude;         ///< Used by MagiQuest [16-bits]
    bool isRepeat;              ///< True if repeat of value is detected

    // next 3 values are copies of irparams values - see IRremoteint.h
    uint16_t *rawbuf;           ///< Raw intervals in 50uS ticks
    uint16_t rawlen;            ///< Number of records in rawbuf
    bool overflow;              ///< true if IR raw code too long
} decode_results;

/**
 * DEPRECATED
 * Decoded value for NEC and others when a repeat code is received
 * Use Flag decode_results.isRepeat (see above) instead
 */
#define REPEAT 0xFFFFFFFF


/**
 * Attempt to decode the recently receive IR signal
 * @param results decode_results instance returning the decode, if any.
 * @return success of operation.
 */
bool decode();

/**
 * Enable IR reception.
 */
void enableIRIn();

/**
 * Disable IR reception.
 */
void disableIRIn();

/**
 * Returns status of reception
 * @return true if no reception is on-going.
 */
bool isIdle();

/**
 * Returns status of reception and copies IR-data to decode_results buffer if true.
 * @return true if data is available.
 */
bool available();

/**
 * Called to re-enable IR reception.
 */
void resume();

const char* getProtocolString();
void printResultShort(void);
void printIRResultRaw(void);
void printIRResultRawFormatted(void);
void printIRResultAsCArray(void);
void printIRResultAsCVariables(void);

/**
 * Print the result (second argument) as Pronto Hex on the Stream supplied as argument.
 * @param stream The Stream on which to write, often Serial
 * @param results the decode_results as delivered from irrecv.decode.
 * @param frequency Modulation frequency in Hz. Often 38000Hz.
 */
void dumpPronto(unsigned int frequency);
void printIRResultAsPronto(unsigned int frequency);

bool decodePulseDistanceData(uint8_t aNumberOfBits, uint8_t aStartOffset, unsigned int aBitMarkMicros,
        unsigned int aOneSpaceMicros, unsigned int aZeroSpaceMicros, bool aMSBfirst);

extern decode_results results; // the instance for decoding

//......................................................................
#if DECODE_RC5
/**
 * Try to decode the recently received IR signal as an RC5 signal-
 * @param results decode_results instance returning the decode, if any.
 * @return Success of the operation.
 */
bool decodeRC5();
#endif
#if DECODE_RC6
bool decodeRC6();
#endif
//......................................................................
#if DECODE_NEC
bool decodeNEC();
#endif
#if DECODE_NEC_STANDARD
bool decodeNECStandard();
#endif

//......................................................................
#if DECODE_SONY
bool decodeSony();
#endif
//......................................................................
#if DECODE_PANASONIC
bool decodePanasonic();
#endif
//......................................................................
#if DECODE_JVC
bool decodeJVC();
#endif
//......................................................................
#if DECODE_SAMSUNG
bool decodeSAMSUNG();
#endif
//......................................................................
#if DECODE_WHYNTER
bool decodeWhynter();
#endif
//......................................................................
#if DECODE_LG
bool decodeLG();
#endif
//......................................................................
#if DECODE_SANYO
bool decodeSanyo();
#endif
//......................................................................
#if DECODE_DISH
    bool  decodeDish () ; // NOT WRITTEN
#endif
//......................................................................
#if DECODE_SHARP
bool decodeSharp();
#endif
#if DECODE_SHARP_ALT
bool decodeSharpAlt();
#endif
//......................................................................
#if DECODE_DENON
bool decodeDenon();
#endif
//......................................................................
#if DECODE_LEGO_PF
    bool  decodeLegoPowerFunctions () ;
#endif
//......................................................................
#if DECODE_BOSEWAVE
bool decodeBoseWave(void);
#endif
//......................................................................
#if DECODE_MAGIQUEST
bool decodeMagiQuest();
#endif

/****************************************************
 *                     SENDING
 ****************************************************/
/**
 * Define to use no carrier PWM, just simulate an active low receiver signal.
 */
//#define USE_NO_SEND_PWM
/**
 * Define to use carrier PWM generation in software, instead of hardware PWM.
 */
//#define USE_SOFT_SEND_PWM
/**
 * If USE_SOFT_SEND_PWM, this amount is subtracted from the on-time of the pulses.
 */
#ifndef PULSE_CORRECTION_MICROS
#define PULSE_CORRECTION_MICROS 3
#endif
/**
 * If USE_SOFT_SEND_PWM, use spin wait instead of delayMicros().
 */
//#define USE_SPIN_WAIT

void enableIROut(int khz);
void sendPulseDistanceWidthData(unsigned int aOneMarkMicros, unsigned int aOneSpaceMicros, unsigned int aZeroMarkMicros,
        unsigned int aZeroSpaceMicros, unsigned long aData, uint8_t aNumberOfBits, bool aMSBfirst);
void mark(uint16_t timeMicros);
void mark_long(uint32_t timeMicros);
void space(uint16_t timeMicros);
void space_long(uint32_t timeMicros);
void sendRaw(const unsigned int buf[], unsigned int len, unsigned int hz);
void sendRaw_P(const unsigned int buf[], unsigned int len, unsigned int hz);

//......................................................................
#if SEND_RC5
void sendRC5(uint32_t data, uint8_t nbits);
void sendRC5ext(uint8_t addr, uint8_t cmd, bool toggle);
#endif
#if SEND_RC6
void sendRC6(uint32_t data, uint8_t nbits);
#endif
//......................................................................
#if SEND_NEC || SEND_NEC_STANDARD
void sendNECRepeat();
#endif
#if SEND_NEC
void sendNEC(uint32_t data, uint8_t nbits, bool repeat);
#endif
#if SEND_NEC_STANDARD
void sendNECStandard(uint16_t aAddress, uint8_t aCommand, uint8_t aNumberOfRepeats);
#endif
//......................................................................
#if SEND_SONY
void sendSony(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_PANASONIC
void sendPanasonic(unsigned int address, unsigned long data);
#endif
//......................................................................
#if SEND_JVC
// JVC does NOT repeat by sending a separate code (like NEC does).
// The JVC protocol repeats by skipping the header.
// To send a JVC repeat signal, send the original code value
//   and set 'repeat' to true
void sendJVC(unsigned long data, int nbits, bool repeat);
#endif
//......................................................................
#if SEND_SAMSUNG
void sendSAMSUNG(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_WHYNTER
void sendWhynter(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_LG
void sendLG(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_SANYO
    void  sendSanyo      ( ) ; // NOT WRITTEN
#endif
//......................................................................
#if SEND_DISH
void sendDISH(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_SHARP
void sendSharpRaw(unsigned long data, int nbits);
void sendSharp(unsigned int address, unsigned int command);
#endif
#if SEND_SHARP_ALT
void sendSharpAltRaw(unsigned int data, int nbits);
void sendSharpAlt(uint8_t address, uint8_t command);
#endif
//......................................................................
#if SEND_DENON
void sendDenon(unsigned long data, int nbits);
#endif
//......................................................................
#if SEND_LEGO_PF
void sendLegoPowerFunctions(uint16_t data, bool repeat);
#endif
//......................................................................
#if SEND_BOSEWAVE
void sendBoseWave(unsigned char code);
#endif
//......................................................................
#if SEND_MAGIQUEST
void sendMagiQuest(unsigned long wand_id, unsigned int magnitude);
#endif

/**
 * Parse the string given as Pronto Hex, and send it a number of times given
 * as the second argument. Thereby the division of the Pronto Hex into
 * an intro-sequence and a repeat sequence is taken into account:
 * First the intro sequence is sent, then the repeat sequence is sent times-1 times.
 * However, if the intro sequence is empty, the repeat sequence is sent times times.
 * <a href="http://www.harctoolbox.org/Glossary.html#ProntoSemantics">Reference</a>.
 *
 * Note: Using this function is very wasteful for the memory consumption on
 * a small board.
 * Normally it is a much better ide to use a tool like e.g. IrScrutinizer
 * to transform Pronto type signals offline
 * to a more memory efficient format.
 *
 * @param str C type string (null terminated) containing a Pronto Hex representation.
 * @param times Number of times to send the signal.
 */
void sendProntoStr(const char* str, unsigned int times);

void sendPronto(const uint16_t* data, unsigned int length, unsigned int times);

#if defined(USE_SOFT_SEND_PWM) || defined(USE_NO_SEND_PWM)

#  if defined(USE_SOFT_SEND_PWM)
unsigned int periodTimeMicros;
unsigned int periodOnTimeMicros;

void sleepMicros(unsigned long us);
void sleepUntilMicros(unsigned long targetTime);
#  endif

#endif

#endif // IRremote_h