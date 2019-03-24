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
#define MAIN_LOOP_POLLING_DELAY         125 //500 ms
#define METRONOME_MODE                  1
#define NORMAL_MODE                     0 


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int atimer_period_ms = S4527438_HAL_ATIMER_PERIOD;
static int atimer_clk = S4527438_HAL_ATIMER_CLKSPEED;
static int cur_mode = NORMAL_MODE;
/* Private function prototypes -----------------------------------------------*/
static void handle_normal_mode(int cmd);
static void handle_metronome_mode(int cmd);

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

	/* Main processing loop */
    while (1) {
        RxChar = debug_getc();

        if( RxChar != '\0' ) {
            switch(RxChar) {
                case 'm':
                    cur_mode = METRONOME_MODE;
                    break;
                case 'n':
                    cur_mode = NORMAL_MODE;
                    break;
                default:
                    if( cur_mode == NORMAL_MODE ) {
                        handle_normal_mode(RxChar);
                    } else {
                        handle_metronome_mode(RxChar);
                    }
                    break;
            }

		    HAL_Delay(MAIN_LOOP_POLLING_DELAY);		//Delay for 2.5s
        }
        HAL_Delay(125);
	}

    return 0;
}

static void handle_normal_mode(int cmd) {
    int cur_angle = 0;
    int atimer_counter = 0;
    int atimer_counter_ms = 0;

    switch(cmd) {
        case 'f':
            s4527438_hal_atimer_timer_pause();
            break;
        case 'r':
            s4527438_hal_atimer_timer_resume();
            break;
        case 'z':
            s4527438_hal_atimer_timer_reset();
            break;
        case 'c':
            atimer_counter = s4527438_hal_atimer_timer_read();
            debug_printf("atimer_counter = %d\n", atimer_counter);
            break;
        case 't':
            atimer_counter_ms = s4527438_hal_atimer_timer_getms();
            debug_printf(" atimer_counter_ms = %d\n", atimer_counter_ms);
            break;

                
        case '+':
            atimer_period_ms += 10;
            s4527438_hal_atimer_period_set(atimer_period_ms);
            break;
        case '-':
            atimer_period_ms -= 10;
            s4527438_hal_atimer_period_set(atimer_period_ms);
            break;
        case 'i':
            atimer_clk += 1000;
            s4527438_hal_atimer_clkspeed_set(atimer_clk);
            break;
        case 'd':
            atimer_clk -= 1000;
            s4527438_hal_atimer_clkspeed_set(atimer_clk);
            break;

        case 'p':
            cur_angle = s4527438_hal_pantilt_pan_read();
            s4527438_hal_pantilt_pan_write(++cur_angle);
            break;
        case 'n':
            cur_angle = s4527438_hal_pantilt_pan_read();
            s4527438_hal_pantilt_pan_write(--cur_angle);
            break;
    }

}

static void handle_metronome_mode(int cmd) {
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

