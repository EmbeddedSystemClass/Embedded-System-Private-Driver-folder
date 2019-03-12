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
#include "s4527438_hal_lta1000g.h"
#include "s4527438_hal_joystick.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    L_TO_R = 0,
    R_TO_L
} MPD_shift_pattern_TypeEnum;
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned short current_ledbar_value = LEDBAR_LED_ALL_ON_MASK;
static MPD_shift_pattern_TypeEnum current_ledbar_shift_pattern = L_TO_R;

/* Private function prototypes -----------------------------------------------*/

void Hardware_init(void);
static void ledbar_mpd_display_update(void);

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {

    int read_value;
    unsigned short ADC_value = 0; 

	BRD_init();			//Initalise Board
	Hardware_init();	//Initalise hardware modules
	
    current_ledbar_value = LEDBAR_MPD_INIT_VALUE;
    s4527438_hal_lta1000g_write(current_ledbar_value);
    current_ledbar_shift_pattern = L_TO_R;

	/* Main processing loop */
    while (1) {

		debug_printf("LED Toggle time: %d\n\r", HAL_GetTick());	//Print debug message with system time (ms)
        
#if 0
        read_value = s4527438_hal_joystick_x_read();
        ADC_value = ((unsigned short)read_value)&0x3FF;
        s4527438_hal_lta1000g_write(ADC_value);
#endif

        /* Check is joystick triggered*/
        if( s4527438_hal_joystick_is_switch_triggered() ) {
            if( current_ledbar_shift_pattern == L_TO_R ) {
                current_ledbar_shift_pattern = R_TO_L;
            } else {
                current_ledbar_shift_pattern = L_TO_R;
            }
            s4527438_hal_joystick_switch_reset();
        }
        ledbar_mpd_display_update();
		HAL_Delay(2500);		//Delay for 2.5s
	}

    return 0;
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {

    s4527438_hal_lta1000g_init();
    s4527438_hal_joystick_init();

	BRD_LEDInit();		//Initialise LEDS

	/* Turn off LEDs */
	BRD_LEDRedOff();
	BRD_LEDGreenOff();
	BRD_LEDBlueOff();
}

static void ledbar_mpd_display_update(void) {
    if( current_ledbar_shift_pattern == L_TO_R ) {
        if( current_ledbar_value & LEDBAR_MPD_SHIFT_L_TO_R_END ) {
	        BRD_LEDGreenOff();
            LEDBAR_MPD_SHIFT_L_TO_R(current_ledbar_value);
            current_ledbar_value |= LEDBAR_MPD_SHIFT_L_TO_R_END_REPEAT;
        } else {
            LEDBAR_MPD_SHIFT_L_TO_R(current_ledbar_value);
        }
    } else {
        if( current_ledbar_value & LEDBAR_MPD_SHIFT_R_TO_L_END ) {
            LEDBAR_MPD_SHIFT_R_TO_L(current_ledbar_value);
            current_ledbar_value |= LEDBAR_MPD_SHIFT_R_TO_L_END_REPEAT;
        } else {
            LEDBAR_MPD_SHIFT_R_TO_L(current_ledbar_value);
        }
    }
    s4527438_hal_lta1000g_write(current_ledbar_value);
}

