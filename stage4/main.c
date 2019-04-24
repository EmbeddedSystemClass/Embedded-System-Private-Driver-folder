/**
  ******************************************************************************
  * @file    blink/main.c
  * @author  MDS
  * @date    27112018
  * @brief   Nucleo429ZI onboard LED flashing example.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_os_pantilt.h"
#include "s4527438_cli_task.h"

#include "FreeRTOS.h"
#include "task.h"

#ifdef S4527438_SYSMON_CONF_INC
#include "s4527438_hal_sysmon_config.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {
    s4527438_os_pantilt_init();
    s4527438_cli_init();
	
    vTaskStartScheduler();
    return 0;
}


