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

#define LEDBAR_VALUE_ON             1
#define LEDBAR_VALUE_OFF            0

/**********************  BOARD LEDs  *****************************/

//LED definitions (Left ~ Right : D25 ~ D16)
#define LEDBAR_0_LED                        0   //D16 : PC6
#define LEDBAR_0_LEDMASK                    1 << LEDBAR_0_LED
#define LEDBAR_0_LED_PIN                    GPIO_PIN_6
#define LEDBAR_0_LED_GPIO_PORT              GPIOC
#define __LEDBAR_0_LED_GPIO_CLK()           __GPIOC_CLK_ENABLE()

#define LEDBAR_1_LED                        1  //D17 : PB15
#define LEDBAR_1_LEDMASK                    1 << LEDBAR_1_LED
#define LEDBAR_1_LED_PIN                    GPIO_PIN_15
#define LEDBAR_1_LED_GPIO_PORT              GPIOB
#define __LEDBAR_1_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

#define LEDBAR_2_LED                        2   //D18 : PB13
#define LEDBAR_2_LEDMASK                    1 << LEDBAR_2_LED
#define LEDBAR_2_LED_PIN                    GPIO_PIN_13
#define LEDBAR_2_LED_GPIO_PORT              GPIOB
#define __LEDBAR_2_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

#define LEDBAR_3_LED                        3   //D19 : PB12
#define LEDBAR_3_LEDMASK                    1 << LEDBAR_3_LED
#define LEDBAR_3_LED_PIN                    GPIO_PIN_12
#define LEDBAR_3_LED_GPIO_PORT              GPIOB
#define __LEDBAR_3_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

#define LEDBAR_4_LED                        4   //D20 : PA15
#define LEDBAR_4_LEDMASK                    1 << LEDBAR_4_LED
#define LEDBAR_4_LED_PIN                    GPIO_PIN_15
#define LEDBAR_4_LED_GPIO_PORT              GPIOA
#define __LEDBAR_4_LED_GPIO_CLK()           __GPIOA_CLK_ENABLE()

#define LEDBAR_5_LED                        5   //D21 : PC7
#define LEDBAR_5_LEDMASK                    1 << LEDBAR_5_LED
#define LEDBAR_5_LED_PIN                    GPIO_PIN_7
#define LEDBAR_5_LED_GPIO_PORT              GPIOC
#define __LEDBAR_5_LED_GPIO_CLK()           __GPIOC_CLK_ENABLE()

#define LEDBAR_6_LED                        6   //D22 : PB5
#define LEDBAR_6_LEDMASK                    1 << LEDBAR_6_LED
#define LEDBAR_6_LED_PIN                    GPIO_PIN_5
#define LEDBAR_6_LED_GPIO_PORT              GPIOB
#define __LEDBAR_6_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

#define LEDBAR_7_LED                        7   //D23 : PB3
#define LEDBAR_7_LEDMASK                    1 << LEDBAR_7_LED
#define LEDBAR_7_LED_PIN                    GPIO_PIN_3
#define LEDBAR_7_LED_GPIO_PORT              GPIOB
#define __LEDBAR_7_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

#define LEDBAR_8_LED                        8   //D24 : PA4
#define LEDBAR_8_LEDMASK                    1 << LEDBAR_8_LED
#define LEDBAR_8_LED_PIN                    GPIO_PIN_4
#define LEDBAR_8_LED_GPIO_PORT              GPIOA
#define __LEDBAR_8_LED_GPIO_CLK()           __GPIOA_CLK_ENABLE()

#define LEDBAR_9_LED                        9   //D25 : PB4
#define LEDBAR_9_LEDMASK                    1 << LEDBAR_9_LED
#define LEDBAR_9_LED_PIN                    GPIO_PIN_4
#define LEDBAR_9_LED_GPIO_PORT              GPIOB
#define __LEDBAR_9_LED_GPIO_CLK()           __GPIOB_CLK_ENABLE()

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

#define LEDBAR_MPD_INIT_VALUE                (0x0003)
#define LEDBAR_MPD_SHIFT_L_TO_R(input)      (input = input << 1)
#define LEDBAR_MPD_SHIFT_L_TO_R_END         LEDBAR_9_LEDMASK
#define LEDBAR_MPD_SHIFT_L_TO_R_END_REPEAT  LEDBAR_0_LEDMASK

#define LEDBAR_MPD_SHIFT_R_TO_L(input)      (input = input >> 1)
#define LEDBAR_MPD_SHIFT_R_TO_L_END         LEDBAR_0_LEDMASK
#define LEDBAR_MPD_SHIFT_R_TO_L_END_REPEAT  LEDBAR_9_LEDMASK

#endif /* S4527438_HAL_LTA1000G_H_ */
