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
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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

		debug_printf("LED Toggle time: %d\n\r", HAL_GetTick());	//Print debug message with system time (ms)
        
		/* Toggle all LEDs */
		BRD_LEDRedToggle();
		BRD_LEDGreenToggle();
		BRD_LEDBlueToggle();

		HAL_Delay(1000);		//Delay for 1s

	}

    return 0;
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {

	BRD_LEDInit();		//Initialise LEDS

	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();
}
