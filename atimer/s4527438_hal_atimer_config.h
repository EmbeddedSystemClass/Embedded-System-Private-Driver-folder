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

#ifndef S4527438_HAL_ATIMER_CONFIG_H_
#define S4527438_HAL_ATIMER_CONFIG_H_

/* D0 : GPORT , 9 pin */
#define     S4527438_HAL_ATIMER_PIN         GPIO_PIN_9
#define     S4527438_HAL_ATIMER_PINPORT     GPIOG
#define     S4527438_HAL_ATIMER_PINCLK()    __GPIOG_CLK_ENABLE()
#define     S4527438_HAL_ATIMER_PERIOD      1000  //    1000ms
#define     S4527438_HAL_ATIMER_CLKSPEED    50000 //    50 kHz

#endif /* S4527438_HAL_ATIMER_CONFIG_H_ */
