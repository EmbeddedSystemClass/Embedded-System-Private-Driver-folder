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
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_lta1000g.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  GPIO_TypeDef  *GPIO_PORT;      
  uint16_t      GPIO_PIN;      
} GPIO_PIN_Info;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static GPIO_PIN_Info segment_to_gpio_pin_info[(LEDBAR_LED_END - LEDBAR_LED_START + 1)] = {0x00};

/* Private function prototypes -----------------------------------------------*/
static void lta1000g_seg_set(int segment,unsigned char segment_value);


void s4527438_hal_lta1000g_init(void) {
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* Enable the GPIO_LED Clock */
    __LEDBAR_0_LED_GPIO_CLK();
    __LEDBAR_1_LED_GPIO_CLK();
    __LEDBAR_2_LED_GPIO_CLK();
    __LEDBAR_3_LED_GPIO_CLK();
    __LEDBAR_4_LED_GPIO_CLK();
    __LEDBAR_5_LED_GPIO_CLK();
    __LEDBAR_6_LED_GPIO_CLK();
    __LEDBAR_7_LED_GPIO_CLK();
    __LEDBAR_8_LED_GPIO_CLK();
    __LEDBAR_9_LED_GPIO_CLK();

    /* Configure the GPIO_LED pin */
    GPIO_InitStructure.Pin = LEDBAR_0_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_0_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_0_LED].GPIO_PORT = LEDBAR_0_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_0_LED].GPIO_PIN = LEDBAR_0_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_1_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_1_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_1_LED].GPIO_PORT = LEDBAR_1_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_1_LED].GPIO_PIN = LEDBAR_1_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_2_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_2_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_2_LED].GPIO_PORT = LEDBAR_2_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_2_LED].GPIO_PIN = LEDBAR_2_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_3_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_3_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_3_LED].GPIO_PORT = LEDBAR_3_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_3_LED].GPIO_PIN = LEDBAR_3_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_4_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_4_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_4_LED].GPIO_PORT = LEDBAR_4_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_4_LED].GPIO_PIN = LEDBAR_4_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_5_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_5_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_5_LED].GPIO_PORT = LEDBAR_5_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_5_LED].GPIO_PIN = LEDBAR_5_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_6_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_6_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_6_LED].GPIO_PORT = LEDBAR_6_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_6_LED].GPIO_PIN = LEDBAR_6_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_7_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_7_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_7_LED].GPIO_PORT = LEDBAR_7_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_7_LED].GPIO_PIN = LEDBAR_7_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_8_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_8_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_8_LED].GPIO_PORT = LEDBAR_8_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_8_LED].GPIO_PIN = LEDBAR_8_LED_PIN;

    GPIO_InitStructure.Pin = LEDBAR_9_LED_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LEDBAR_9_LED_GPIO_PORT, &GPIO_InitStructure);
    segment_to_gpio_pin_info[LEDBAR_9_LED].GPIO_PORT = LEDBAR_9_LED_GPIO_PORT;
    segment_to_gpio_pin_info[LEDBAR_9_LED].GPIO_PIN = LEDBAR_9_LED_PIN;

}

static void lta1000g_seg_set(int segment,unsigned char segment_value) {

    GPIO_PIN_Info  *gpio_info = (&(segment_to_gpio_pin_info[segment]));
    GPIO_TypeDef   *gpio_port =  gpio_info->GPIO_PORT;      

    if( segment_value == 1 ) {
        gpio_port->BSRR |= gpio_info->GPIO_PIN;
    } else if( segment_value == 0 ) {
        gpio_port->BSRR |= (gpio_info->GPIO_PIN) << 16;
    }
}

void s4527438_hal_lta1000g_write(unsigned short value) {
    if ((value & LEDBAR_0_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_0_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_0_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_1_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_1_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_1_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_2_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_2_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_2_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_3_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_3_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_3_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_4_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_4_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_4_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_5_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_5_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_5_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_6_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_6_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_6_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_7_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_7_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_7_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_8_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_8_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_8_LED,LEDBAR_VALUE_OFF);
    }

    if ((value & LEDBAR_9_LEDMASK) != 0) {
        lta1000g_seg_set(LEDBAR_9_LED,LEDBAR_VALUE_ON);
    }else {
        lta1000g_seg_set(LEDBAR_9_LED,LEDBAR_VALUE_OFF);
    }
}
