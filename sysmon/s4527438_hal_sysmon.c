/** 
 **************************************************************
 * @file mylib/s4527438_hal_atimer.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib atimer driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_atimer.h"

#ifdef S4527438_SYSMON_CONF_INC
#include "s4527438_hal_sysmon_config.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void s4527438_hal_sysmon_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    /* Channel 0 */
    S4527438_HAL_SYSMON_CHAN0CLK();
    GPIO_InitStructure.Pin = S4527438_HAL_SYSMON_CHAN0PIN;                //Pin
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;       //Set mode to be output alternate
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;          // Default is pull down , since timer event will send high 
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    HAL_GPIO_Init(S4527438_HAL_SYSMON_CHAN0PORT, &GPIO_InitStructure);   //Initialise Pin

    /* Channel 1 */
    S4527438_HAL_SYSMON_CHAN1CLK();
    GPIO_InitStructure.Pin = S4527438_HAL_SYSMON_CHAN1PIN;                //Pin
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;       //Set mode to be output alternate
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;          // Default is pull down , since timer event will send high 
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    HAL_GPIO_Init(S4527438_HAL_SYSMON_CHAN1PORT, &GPIO_InitStructure);   //Initialise Pin

    /* Channel 2 */
    S4527438_HAL_SYSMON_CHAN2CLK();
    GPIO_InitStructure.Pin = S4527438_HAL_SYSMON_CHAN2PIN;                //Pin
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;       //Set mode to be output alternate
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;          // Default is pull down , since timer event will send high 
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    HAL_GPIO_Init(S4527438_HAL_SYSMON_CHAN2PORT, &GPIO_InitStructure);   //Initialise Pin
}


