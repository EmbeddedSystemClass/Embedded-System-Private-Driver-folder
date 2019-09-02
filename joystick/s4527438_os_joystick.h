/** 
 **************************************************************
 * @file mylib/joystick/s4527438_os_joystick.h
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

void s4527438_os_joystick_init(void);
void s4527438_os_joystick_switch_reset(void);
uint32_t s4527438_os_joystick_is_switch_triggered(void);
#endif /* S4527438_HAL_JOYSTICK_H_ */
