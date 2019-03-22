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
#include "s4527438_hal_atimer.h"
#include "s4527438_hal_pantilt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MAIN_LOOP_POLLING_DELAY         100 //500 ms


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {
	BRD_init();			//Initalise Board
	Hardware_init();	//Initalise hardware modules
	
	/* Main processing loop */
    while (1) {
		//HAL_Delay(MAIN_LOOP_POLLING_DELAY);		//Delay for 2.5s
	}

    return 0;
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {
    //s4527438_hal_atimer_init_pin();
    //s4527438_hal_atimer_init();
    s4527438_hal_pantilt_init();
}

