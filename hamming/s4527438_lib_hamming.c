/** 
 **************************************************************
 * @file mylib/s4527438_hal_radio.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib radio driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "nrf24l01plus.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/**
  * Implement Hamming Code + parity checking
  * Hamming code is based on the following generator and parity check matrices
  * G = [ 1 1 1 | 1 0 0 0 ;
  *       1 0 1 | 0 1 0 0 ;
  *       1 1 0 | 0 0 1 0 ;
  *       0 1 1 | 0 0 0 1 ;
  *
  * hence H =
  * [ 1 0 0 | 1 1 1 0 ;
  *   0 1 0 | 1 0 1 1 ;
  *   0 0 1 | 1 1 0 1 ];
  *
  * y = x * G, syn = H * y'
  *
  *
  * NOTE: !! is used to get 1 out of non zeros
  */
static uint8_t hamming_hbyte_encoder(uint8_t in) {

    uint8_t d0, d1, d2, d3;
    uint8_t p0 = 0, h0, h1, h2;
    uint8_t z;
    uint8_t out;

    /* extract bits */
    d0 = !!(in & 0x1);
    d1 = !!(in & 0x2);
    d2 = !!(in & 0x4);
    d3 = !!(in & 0x8);

    /* calculate hamming parity bits */
    h0 = d0 ^ d1 ^ d2;
    h1 = d0 ^ d2 ^ d3;
    h2 = d0 ^ d1 ^ d3;

    /* generate out byte without parity bit P0 */
    out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
        (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

    /* calculate even parity bit */
    for (z = 1; z<8; z++)
        p0 = p0 ^ !!(out & (1 << z));

    out |= p0;

    return(out);
}

void s4527438_lib_hamming_byte_encoder(uint8_t input_byte,unsigned char *encoded_output) {
         uint16_t out;

     /* first encode D0..D3 (first 4 bits),
      * then D4..D7 (second 4 bits).
      */
    encoded_output[0] = hamming_hbyte_encoder(input & 0xF);
    encoded_output[1] = (hamming_hbyte_encoder(input >> 4) << 8);
}

/* xxxx 0111 */
#define H0_PATTERN      0x07    
/* xxxx 0010 */
#define IS_H0_ERROR_MASK      0x02

/* xxxx 1101 */
#define H1_PATTERN      0x0D
/* xxxx 0100 */
#define IS_H1_ERROR_MASK      0x04

/* xxxx 1011 */
#define H2_PATTERN      0x0B
/* xxxx 1000 */
#define IS_H2_ERROR_MASK      0x08

/* xxxx 1110 */
#define IS_ERROR_DETECT_MASK      0x0E

/* 1111 0000 */
/* EXTRA original 4 bits mask from byte */
#define EXTRACK_4_BITS_MASK      0xF0
uint8_t s4527438_lib_hamming_byte_decoder(uint8_t *input_byte) {
    uint8_t result = 0;

    /* First 4 bits : no error */
    if( !(input_byte[0] & IS_ERROR_DETECT_MASK) ) {
        result = (input_byte[0] & EXTRACK_4_BITS_MASK) >> 4;
    } else {
    }
}


