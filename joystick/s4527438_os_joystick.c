 /** 
 **************************************************************
 * @file mylib/sxxxxxx_mylib_template.c
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
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_joystick.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*********************  JOYSTICK SWITCH BUTTON  *****************************/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void s4527438_os_joystick_init(void) {
    s4527438_hal_joystick_init();
}

void s4527438_os_joystick_switch_reset(void) {
    s4527438_hal_joystick_switch_reset();
}

uint32_t s4527438_os_joystick_is_switch_triggered(void) {
    return s4527438_os_joystick_is_switch_triggered();
}

