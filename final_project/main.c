/**
  ******************************************************************************
  * @file    stages/final_project/main.c
  * @author  KO-CHEN CHI
  * @date    27112018
  * @brief   Nucleo429ZI onboard LED flashing example.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_os_radio.h"
#include "s4527438_os_atimer.h"
#include "s4527438_cli_task.h"

#include "FreeRTOS.h"
#include "task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Hardware_init();

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {
    BRD_init();
    Hardware_init();

    s4527438_os_atimer_init();
    s4527438_os_radio_init();
    s4527438_cli_init();
    portENABLE_INTERRUPTS();
	
    vTaskStartScheduler();
    return 0;
}

void Hardware_init( void ) {

    portDISABLE_INTERRUPTS();   //Disable interrupts

    BRD_LEDInit();              //Initialise Blue LED
    BRD_LEDBlueOff();               //Turn off Blue LED
    BRD_LEDRedOff();               //Turn off Blue LED
    BRD_LEDGreenOff();               //Turn off Blue LED

    portENABLE_INTERRUPTS();    //Enable interrupts

}
