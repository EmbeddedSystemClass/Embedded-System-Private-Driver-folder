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

#ifndef S4527438_HAL_LTA1000G_H_
#define S4527438_HAL_LTA1000G_H_

#define LEDBAR_VALUE_ON             0
#define LEDBAR_VALUE_OFF            1

/**********************  BOARD LEDs  *****************************/

//LED definitions (Left ~ Right : D0 ~ D9)
#define LEDBAR_0_LED                        0   //D0 : PG9
#define LEDBAR_0_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_0_LED_PIN                    GPIO_PIN_9
#define LEDBAR_0_LED_GPIO_PORT              GPIOG
#define __LEDBAR_0_LED_GPIO_CLK()           __GPIOG_CLK_ENABLE()

#define LEDBAR_1_LED                        1  //D1 : PG14
#define LEDBAR_1_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_1_LED_PIN                    GPIO_PIN_14
#define LEDBAR_1_LED_GPIO_PORT              GPIOG
#define __LEDBAR_1_LED_GPIO_CLK()           __GPIOG_CLK_ENABLE()

#define LEDBAR_2_LED                        2   //D2 : PF15
#define LEDBAR_2_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_2_LED_PIN                    GPIO_PIN_15
#define LEDBAR_2_LED_GPIO_PORT              GPIOF
#define __LEDBAR_2_LED_GPIO_CLK()           __GPIOF_CLK_ENABLE()

#define LEDBAR_3_LED                        3   //D3 : PE13
#define LEDBAR_3_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_3_LED_PIN                    GPIO_PIN_13
#define LEDBAR_3_LED_GPIO_PORT              GPIOE
#define __LEDBAR_3_LED_GPIO_CLK()           __GPIOE_CLK_ENABLE()

#define LEDBAR_4_LED                        4   //D4 : PF14
#define LEDBAR_4_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_4_LED_PIN                    GPIO_PIN_14
#define LEDBAR_4_LED_GPIO_PORT              GPIOF
#define __LEDBAR_4_LED_GPIO_CLK()           __GPIOF_CLK_ENABLE()

#define LEDBAR_5_LED                        5   //D5 : PE11
#define LEDBAR_5_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_5_LED_PIN                    GPIO_PIN_11
#define LEDBAR_5_LED_GPIO_PORT              GPIOE
#define __LEDBAR_5_LED_GPIO_CLK()           __GPIOE_CLK_ENABLE()

#define LEDBAR_6_LED                        6   //D6 : PE9
#define LEDBAR_6_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_6_LED_PIN                    GPIO_PIN_9
#define LEDBAR_6_LED_GPIO_PORT              GPIOE
#define __LEDBAR_6_LED_GPIO_CLK()           __GPIOE_CLK_ENABLE()

#define LEDBAR_7_LED                        7   //D7 : PF13
#define LEDBAR_7_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_7_LED_PIN                    GPIO_PIN_13
#define LEDBAR_7_LED_GPIO_PORT              GPIOF
#define __LEDBAR_7_LED_GPIO_CLK()           __GPIOF_CLK_ENABLE()

#define LEDBAR_8_LED                        8   //D8 : PF12
#define LEDBAR_8_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_8_LED_PIN                    GPIO_PIN_12
#define LEDBAR_8_LED_GPIO_PORT              GPIOF
#define __LEDBAR_8_LED_GPIO_CLK()           __GPIOF_CLK_ENABLE()

#define LEDBAR_9_LED                        9   //D9 : PD15
#define LEDBAR_9_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_9_LED_PIN                    GPIO_PIN_15
#define LEDBAR_9_LED_GPIO_PORT              GPIOD
#define __LEDBAR_9_LED_GPIO_CLK()           __GPIOD_CLK_ENABLE()

#define LEDBAR_LED_START                    LEDBAR_0_LED
#define LEDBAR_LED_END                      LEDBAR_9_LED

#define LEDBAR_LEDMASK_START                LEDBAR_0_LEDMASK
#define LEDBAR_LEDMASK_END                  LEDBAR_9_LEDMASK

#define LEDBAR_LED_ALL_ON_MASK              (   LEDBAR_0_LEDMASK \
                                                | LEDBAR_1_LEDMASK\
                                                | LEDBAR_2_LEDMASK\
                                                | LEDBAR_3_LEDMASK\
                                                | LEDBAR_4_LEDMASK\
                                                | LEDBAR_5_LEDMASK\
                                                | LEDBAR_6_LEDMASK\
                                                | LEDBAR_7_LEDMASK\
                                                | LEDBAR_8_LEDMASK\
                                                | LEDBAR_9_LEDMASK\
                                            )

void s4527438_hal_lta1000g_init();
void s4527438_hal_lta1000g_write(unsigned short value);

#endif /* S4527438_HAL_LTA1000G_H_ */
