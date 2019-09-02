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
//#include "nrf24l01plus.h"

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
#if 0
    encoded_output[0] = hamming_hbyte_encoder(input_byte & 0xF);
    encoded_output[1] = hamming_hbyte_encoder(input_byte >> 4);
#endif
#if 1
    encoded_output[1] = hamming_hbyte_encoder(input_byte & 0xF);
    encoded_output[0] = hamming_hbyte_encoder(input_byte >> 4);
#endif
}

#define H00     0x01                    
#define H03     0x08 
#define H04     0x10 
#define H05     0x20 

#define H11     0x02 
#define H13     0x08 
#define H15     0x20 
#define H16     0x40 

#define H22     0x04
#define H23     0x08 
#define H24     0x10 
#define H26     0x40 
static uint8_t hamming_two_hbyte_decoder(uint8_t *encoded_input,uint8_t *output_char) {
    uint8_t s0, s1, s2, p_bit;
    uint8_t result = 0;
    uint8_t target_byte = 0;
    uint8_t error_digit_num = 0;
    uint8_t return_val = 0; //No Error need to report

    /* Extra high 4 bit from encoded_input[0] */
    target_byte = (encoded_input[0] >> 1);

    p_bit = (!!(encoded_input[0] & (1 << 1))) ^
            (!!(encoded_input[0] & (1 << 2))) ^
            (!!(encoded_input[0] & (1 << 3))) ^
            (!!(encoded_input[0] & (1 << 4))) ^
            (!!(encoded_input[0] & (1 << 5))) ^
            (!!(encoded_input[0] & (1 << 6))) ^
            (!!(encoded_input[0] & (1 << 7)));

    // Calculate H Row 0
    s0 = (!!(target_byte & H00)) ^ (!!(target_byte & H03)) ^ (!!(target_byte & H04)) ^ (!!(target_byte & H05));
    error_digit_num |= s0;

    // Calculate H Row 1
    s1 = (!!(target_byte & H11)) ^ (!!(target_byte & H13)) ^ (!!(target_byte & H15)) ^ (!!(target_byte & H16));
    error_digit_num |= (s1 << 1);

    // Calculate H Row 2
    s2 = (!!(target_byte & H22)) ^ (!!(target_byte & H23)) ^ (!!(target_byte & H24)) ^ (!!(target_byte & H26));
    error_digit_num |= (s2 << 2);

    if( (s0 | s1 | s2) == 0 ) {
        result  = result | (target_byte >> 3);
        result  =   result << 4;
    } else if( p_bit ) {
        // Correct error
        target_byte = target_byte ^ (1 << (error_digit_num - 1));
        result  = result | (target_byte >> 3);
        result  =   result << 4;
    } else { 
        return_val = 1;
        // More than one bit error
    }

    /* Extra low 4 bit from encoded_input[1] */
    target_byte = (encoded_input[1] >> 1);
    p_bit = (!!(encoded_input[1] & (1 << 1))) ^
            (!!(encoded_input[1] & (1 << 2))) ^
            (!!(encoded_input[1] & (1 << 3))) ^
            (!!(encoded_input[1] & (1 << 4))) ^
            (!!(encoded_input[1] & (1 << 5))) ^
            (!!(encoded_input[1] & (1 << 6))) ^
            (!!(encoded_input[1] & (1 << 7)));
    error_digit_num = 0;

    // Calculate H Row 0
    s0 = (!!(target_byte & H00)) ^ (!!(target_byte & H03)) ^ (!!(target_byte & H04)) ^ (!!(target_byte & H05));
    error_digit_num |= s0;

    // Calculate H Row 1
    s1 = (!!(target_byte & H11)) ^ (!!(target_byte & H13)) ^ (!!(target_byte & H15)) ^ (!!(target_byte & H16));
    error_digit_num |= (s1 << 1);

    // Calculate H Row 2
    s2 = (!!(target_byte & H22)) ^ (!!(target_byte & H23)) ^ (!!(target_byte & H24)) ^ (!!(target_byte & H26));
    error_digit_num |= (s2 << 2);

    if( (s0 | s1 | s2) == 0 ) {
        result  = result | (target_byte >> 3);
    } else if( p_bit ) {
        // Correct error
        target_byte = target_byte ^ (1 << (error_digit_num - 1));
        result  = result | (target_byte >> 3);
    } else {
        return_val = 1;
        // More than two bit error
    }

    if( !return_val ) {
        *output_char = result;
    }
    return return_val;
}

uint8_t s4527438_lib_hamming_byte_decoder(uint8_t *input_byte,uint8_t *decoded_byte_buffer) {
    return hamming_two_hbyte_decoder(input_byte,&(decoded_byte_buffer[0]));
}


