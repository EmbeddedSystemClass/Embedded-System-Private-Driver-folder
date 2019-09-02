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

#ifndef S4527438_HAL_ATIMER_H_
#define S4527438_HAL_ATIMER_H_

extern int atimerCounterVal;
extern int32_t atimerCounterValMax;

void s4527438_hal_atimer_init(void);
int s4527438_hal_atimer_timer_getms(void);
void s4527438_hal_atimer_timer_reset(void);
void s4527438_hal_atimer_timer_pause(void);
void s4527438_hal_atimer_timer_resume(void);
void s4527438_hal_atimer_clkspeed_set(int frequency);
void s4527438_hal_atimer_period_set(int period);
void s4527438_hal_atimer_init_pin();

#define s4527438_hal_atimer_timer_read()        (atimerCounterVal)
#define s4527438_hal_atimer_timer_max_value()   (atimerCounterValMax)
#define S4527438_HAL_CLOCK_LOWER_BOUND                      123 // 8000000(system core clock/2) / 65535(16-bit bus)

/* D10 : DPORT , 14 pin */
#define     S4527438_HAL_ATIMER_PIN         GPIO_PIN_14
#define     S4527438_HAL_ATIMER_PINPORT     GPIOD
#define     S4527438_HAL_ATIMER_PINCLK()    __GPIOD_CLK_ENABLE()
#define     S4527438_HAL_ATIMER_PERIOD      2  //    2ms
#define     S4527438_HAL_ATIMER_CLKSPEED    25000 //    25 kHz

#endif /* S4527438_HAL_ATIMER_H_ */
