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

#ifndef S4527438_HAL_ATIMER_H_
#define S4527438_HAL_ATIMER_H_

extern int atimerCounterVal;

void s4527438_hal_atimer_init(void);
int s4527438_hal_atimer_timer_getms(void);
void s4527438_hal_atimer_timer_reset(void);
void s4527438_hal_atimer_timer_pause(void);
void s4527438_hal_atimer_timer_resume(void);
void s4527438_hal_atimer_clkspeed_set(int frequency);
void s4527438_hal_atimer_period_set(int period);
void s4527438_hal_atimer_init_pin();

#define s4527438_hal_atimer_timer_read() {return atimerCounterVal;} 
#define S4527438_HAL_CLOCK_LOWER_BOUND                      123 // 8000000(system core clock/2) / 65535(16-bit bus)

#endif /* S4527438_HAL_ATIMER_H_ */
