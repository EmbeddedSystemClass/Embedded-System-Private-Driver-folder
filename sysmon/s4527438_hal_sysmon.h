/** 
 **************************************************************
 * @file mylib/s4527438_hal_atimer.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib atimer driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * s4527438_hal_atimer_init(void) - intialise mylib template driver
 * s4527438_hal_atimer_timer_getms(void) - set mylib template driver 
 *************************************************************** 
 */

#ifndef S4527438_HAL_SYSMON_H_
#define S4527438_HAL_SYSMON_H_

void s4527438_hal_sysmon_init(void);

#ifndef S4527438_SYSMON_CONF_INC
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
#endif /* S4527438_SYSMON_CONF_INC */

#define s4527438_hal_sysmon_chan0_clr()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN0PORT, S4527438_HAL_SYSMON_CHAN0PIN, 0x01^0x01)
#define s4527438_hal_sysmon_chan0_set()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN0PORT, S4527438_HAL_SYSMON_CHAN0PIN, 0x00^0x01)

#define s4527438_hal_sysmon_chan1_clr()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN1PORT, S4527438_HAL_SYSMON_CHAN1PIN, 0x01^0x01)
#define s4527438_hal_sysmon_chan1_set()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN1PORT, S4527438_HAL_SYSMON_CHAN1PIN, 0x00^0x01)

#define s4527438_hal_sysmon_chan2_clr()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN2PORT, S4527438_HAL_SYSMON_CHAN2PIN, 0x01^0x01)
#define s4527438_hal_sysmon_chan2_set()     HAL_GPIO_WritePin(S4527438_HAL_SYSMON_CHAN2PORT, S4527438_HAL_SYSMON_CHAN2PIN, 0x00^0x01)
#endif /* S4527438_HAL_SYSMON_H_ */
