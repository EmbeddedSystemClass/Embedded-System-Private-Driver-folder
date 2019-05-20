/** 
 **************************************************************
 * @file mylib/s4527438_lib_hamming.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib hamming driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_LIB_HAMMING_H_
#define S4527438_LIB_HAMMING_H_

void s4527438_lib_hamming_byte_encoder(uint8_t input_byte,unsigned char *encoded_output);
uint8_t s4527438_lib_hamming_byte_decoder(uint8_t *input_byte);

#endif /* S4527438_LIB_HAMMING_H_ */
