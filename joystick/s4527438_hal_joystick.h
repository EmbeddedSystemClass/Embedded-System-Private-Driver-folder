/** 
 **************************************************************
 * @file mylib/joystick/s4527438_hal_joystick.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib joystick driver
 * REFERENCE: 
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
void s4527438_hal_joystick_switch_reset(void);
uint32_t s4527438_hal_joystick_is_switch_triggered(void);
#endif /* S4527438_HAL_JOYSTICK_H_ */
