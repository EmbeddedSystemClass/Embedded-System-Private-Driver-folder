/** 
 **************************************************************
 * @file mylib/sxxxxxx_mylib_template.h
 * @author MyName - MyStudent ID
 * @date 22022018
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * sxxxxxx_mylib_template_init() - intialise mylib template driver
 * sxxxxxx_mylib_template_set() - set mylib template driver 
 *************************************************************** 
 */
#ifndef S4527438_HAL_JOYSTICK_H_
#define S4527438_HAL_JOYSTICK_H_

#include "board.h"
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef AdcHandle_y;
extern ADC_HandleTypeDef AdcHandle_x;

int joystick_readxy(ADC_HandleTypeDef* hadc);

#define s4527438_hal_joystick_x_read() joystick_readxy(&AdcHandle_x)
#define s4527438_hal_joystick_y_read() joystick_readxy(&AdcHandle_y)

void s4527438_hal_joystick_init(void);
#endif /* S4527438_HAL_JOYSTICK_H_ */
