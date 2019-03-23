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
    BRD_LEDInit();
	Hardware_init();	//Initalise hardware modules
	
    char RxChar;
    int cur_angle = 0;

	/* Main processing loop */
    while (1) {
        RxChar = debug_getc();
		//HAL_Delay(MAIN_LOOP_POLLING_DELAY);		//Delay for 2.5s

        if( RxChar != '\0' ) {
            //debug_printf("%c\n", RxChar);

            cur_angle = s4527438_hal_pantilt_pan_read();
            if( RxChar == '+' ) {
                s4527438_hal_pantilt_pan_write(++cur_angle);
            } else if( RxChar == '-' ) {
                s4527438_hal_pantilt_pan_write(--cur_angle);
            }
            //debug_printf("%d\n", cur_angle);
        }
        HAL_Delay(125);
	}

    return 0;
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {
    s4527438_hal_atimer_init_pin();
    s4527438_hal_atimer_init();
    s4527438_hal_pantilt_init();
}

