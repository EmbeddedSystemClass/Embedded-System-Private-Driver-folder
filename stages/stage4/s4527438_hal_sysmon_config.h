/** 
 **************************************************************
 * @file mylib/s4527438_hal_sysmon_config.h
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

#ifndef S4527438_HAL_SYSMON_CONFIG_H_
#define S4527438_HAL_SYSMON_CONFIG_H_

/* D43 : CPORT , 8 pin */
#define S4527438_HAL_SYSMON_CHAN0PIN        GPIO_PIN_8
#define S4527438_HAL_SYSMON_CHAN0PORT       GPIOC
#define S4527438_HAL_SYSMON_CHAN0CLK()      __GPIOC_CLK_ENABLE()

/* D44 : CPORT , 9 pin */
#define S4527438_HAL_SYSMON_CHAN1PIN        GPIO_PIN_9
#define S4527438_HAL_SYSMON_CHAN1PORT       GPIOC
#define S4527438_HAL_SYSMON_CHAN1CLK()      __GPIOC_CLK_ENABLE()

/* D45 : CPORT , 10 pin */
#define S4527438_HAL_SYSMON_CHAN2PIN        GPIO_PIN_10
#define S4527438_HAL_SYSMON_CHAN2PORT       GPIOC
#define S4527438_HAL_SYSMON_CHAN2CLK()      __GPIOC_CLK_ENABLE()

#endif /* S4527438_HAL_SYSMON_CONFIG_H_ */
