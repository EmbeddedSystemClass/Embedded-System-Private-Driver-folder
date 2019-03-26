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
#include "s4527438_hal_atimer.h"
#include "s4527438_hal_pantilt.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
    ANGLE_INCREASE = 0,
    ANGLE_DECREASE 
} METRONOME_shift_pattern_TypeEnum;
/* Private define ------------------------------------------------------------*/

#define METRONOME_MODE                  1
#define NORMAL_MODE                     0 

#define METRONOME_PERIOD_LOWER          2000 // 2 s
#define METRONOME_PERIOD_UPPER          20000 // 20 s

#define METRONOME_COUNT_PER_METRONOME_INTERVAL                      10   // 10
#define METRONOME_CHANGE_TICK_COUNT_THRESHOLD                       10   // 10
#define METRONOME_CHANGE_TICK_COUNT_THRESHOLD_ANGLE                 (80/METRONOME_COUNT_PER_METRONOME_INTERVAL)   // 10

#define METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD          (METRONOME_CHANGE_TICK_COUNT_THRESHOLD*METRONOME_COUNT_PER_METRONOME_INTERVAL)
#define METRONOME_FREQ_LOWER_BOUND                                  (10*METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD*1000/METRONOME_PERIOD_LOWER) // We set clk freq by times 10 

#define METRONOME_ANGLE_LOWER          (-40)    // -40 degree
#define METRONOME_ANGLE_MIDDLE         (0)      // 0 degree
#define METRONOME_ANGLE_UPPER          (40)     // 40 degree

#define MAIN_LOOP_POLLING_DELAY        (METRONOME_PERIOD_LOWER/(METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD*5)) //100 ms

#define METRONOME_LED_SLOT0_ANGLE_BEGIN      0
#define METRONOME_LED_SLOT1_ANGLE_BEGIN      8
#define METRONOME_LED_SLOT2_ANGLE_BEGIN      16
#define METRONOME_LED_SLOT3_ANGLE_BEGIN      24
#define METRONOME_LED_SLOT4_ANGLE_BEGIN      32
#define METRONOME_LED_SLOT5_ANGLE_BEGIN      40
#define METRONOME_LED_SLOT6_ANGLE_BEGIN      48
#define METRONOME_LED_SLOT7_ANGLE_BEGIN      56
#define METRONOME_LED_SLOT8_ANGLE_BEGIN      64
#define METRONOME_LED_SLOT9_ANGLE_BEGIN      72

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int atimer_period_ms = S4527438_HAL_ATIMER_PERIOD;
static int atimer_clk = S4527438_HAL_ATIMER_CLKSPEED;
static int cur_mode = NORMAL_MODE;
static int metronome_cur_period_ms = 0;
static int metronome_cur_angle = METRONOME_ANGLE_MIDDLE;
static int metronome_cur_change_tick_count = 0;
static METRONOME_shift_pattern_TypeEnum metronome_shift_pattern = ANGLE_INCREASE;

/* Private function prototypes -----------------------------------------------*/
static void handle_normal_mode(int cmd);
static void handle_metronome_mode(int cmd);

static void metronome_mode_init(void);
static void normal_mode_init(void);
static void metronome_polling_update_angle(void);
static void metronome_polling_update_LEDBAR(int angle);

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

    // set period init value

	/* Main processing loop */
    while (1) {
        RxChar = debug_getc();

        if( RxChar != '\0' ) {
            switch(RxChar) {
                case 'm':
                    cur_mode = METRONOME_MODE;

                    metronome_mode_init();
                    break;
                case 'n':
                    cur_mode = NORMAL_MODE;
                    normal_mode_init();
                    break;
            }
#ifdef DEBUG
            debug_printf(" cur_mode = %s\n", (cur_mode == NORMAL_MODE)?"Normal":"Metronome");
#endif
            if( cur_mode == NORMAL_MODE ) {
                handle_normal_mode(RxChar);
            } else {
                handle_metronome_mode(RxChar);
            }
        }

        if( cur_mode == METRONOME_MODE ) {
            if( ( s4527438_hal_atimer_timer_read() - metronome_cur_change_tick_count) >= METRONOME_CHANGE_TICK_COUNT_THRESHOLD ) {
                metronome_polling_update_angle();
                metronome_polling_update_LEDBAR(metronome_cur_angle);

                metronome_cur_change_tick_count = s4527438_hal_atimer_timer_read();
            }
        }
        HAL_Delay(MAIN_LOOP_POLLING_DELAY);
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
#ifdef DEBUG
            metronome_polling_update_LEDBAR(cur_angle);
            debug_printf(" cur_angle = %d\n", cur_angle);
#endif
            break;
        case 'n':
            cur_angle = s4527438_hal_pantilt_pan_read();
            s4527438_hal_pantilt_pan_write(--cur_angle);
#ifdef DEBUG
            metronome_polling_update_LEDBAR(cur_angle);
            debug_printf(" cur_angle = %d\n", cur_angle);
#endif
            break;
    }

}

static void handle_metronome_mode(int cmd) {
    int cur_angle = 0;
    int atimer_counter = 0;
    int atimer_counter_ms = 0;

    switch(cmd) {
        case '+':
            metronome_cur_period_ms += 1000;
            s4527438_hal_atimer_period_set(metronome_cur_period_ms/METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD);
            break;
        case '-':
            metronome_cur_period_ms -= 1000;
            s4527438_hal_atimer_period_set(metronome_cur_period_ms/METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD);
            break;
    }
}

static void normal_mode_init(void) {
    int cur_angle = 0;

    s4527438_hal_atimer_clkspeed_set(atimer_clk);
    s4527438_hal_atimer_period_set(atimer_period_ms);
    metronome_polling_update_LEDBAR(LEDBAR_LED_ALL_ON_MASK^LEDBAR_LED_ALL_ON_MASK);

    cur_angle = s4527438_hal_pantilt_pan_read();
    s4527438_hal_pantilt_pan_write(cur_angle);
}

static void metronome_mode_init(void) {
    // Init setting
    metronome_shift_pattern = ANGLE_INCREASE;
    metronome_cur_period_ms = METRONOME_PERIOD_LOWER;
    metronome_cur_change_tick_count = 0;
    metronome_cur_angle = METRONOME_ANGLE_MIDDLE;

    s4527438_hal_atimer_clkspeed_set(METRONOME_FREQ_LOWER_BOUND);
    s4527438_hal_atimer_period_set(metronome_cur_period_ms/METRONOME_CHANGE_TOTAL_TICK_COUNT_PER_TIMER_PERIOD);
    metronome_polling_update_LEDBAR(metronome_cur_angle);
    s4527438_hal_pantilt_pan_write(metronome_cur_angle);
}

static void metronome_polling_update_angle(void) {
    if( metronome_cur_angle <= METRONOME_ANGLE_LOWER ) {
        metronome_shift_pattern = ANGLE_INCREASE;
    } else if( metronome_cur_angle >= METRONOME_ANGLE_UPPER ) {
        metronome_shift_pattern = ANGLE_DECREASE;
    }

    if( metronome_shift_pattern == ANGLE_INCREASE ) {
        metronome_cur_angle += METRONOME_CHANGE_TICK_COUNT_THRESHOLD_ANGLE;
        s4527438_hal_pantilt_pan_write(metronome_cur_angle);
    } else {
        metronome_cur_angle -= METRONOME_CHANGE_TICK_COUNT_THRESHOLD_ANGLE;
        s4527438_hal_pantilt_pan_write(metronome_cur_angle);
    }
}

static void metronome_polling_update_LEDBAR(int target_angle) {
    if( target_angle < METRONOME_LED_SLOT1_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_0_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT1_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT1_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_1_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT2_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT3_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_2_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT3_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT4_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_3_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT4_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT5_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_4_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT5_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT6_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_5_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT6_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT7_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_6_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT7_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT8_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_7_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT8_ANGLE_BEGIN && target_angle < METRONOME_LED_SLOT9_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_8_LEDMASK);
    } else if( target_angle >= METRONOME_LED_SLOT9_ANGLE_BEGIN ) {
        s4527438_hal_lta1000g_write(LEDBAR_9_LEDMASK);
    }
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {
    s4527438_hal_lta1000g_init();
    s4527438_hal_atimer_init_pin();
    s4527438_hal_atimer_init();
    s4527438_hal_pantilt_init();
}

