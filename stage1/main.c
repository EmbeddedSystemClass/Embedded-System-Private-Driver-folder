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
#define MAIN_LOOP_POLLING_DELAY         100 //500 ms
#define MPD_SPEED_COLLECTION_WINDOW_DELAY_COUNT     (500/MAIN_LOOP_POLLING_DELAY) //500 ms

#define PATTERN1_INDEX          0
#define PATTERN1_NUM            (0x0000U) //xx000000xxxx
#define PATTERN1_MASK           0x03F0 //xx000000xxxx
#define PATTERN1_SHIFT_BIT      4
#define PATTERN1_DELAY_COUNT    0 // stop

#define PATTERN2_INDEX          1
#define PATTERN2_NUM            0x0040 //xx0001xxxxxx
#define PATTERN2_MASK           0x03C0 //xx0001xxxxxx
#define PATTERN2_SHIFT_BIT      6
#define PATTERN2_DELAY_COUNT    (1000/MAIN_LOOP_POLLING_DELAY) // 1000ms

#define PATTERN3_INDEX          2
#define PATTERN3_NUM            0x0100 //xxx1xxxxxxxx
#define PATTERN3_MASK           0x0100 //xxx1xxxxxxxx
#define PATTERN3_SHIFT_BIT      8
#define PATTERN3_DELAY_COUNT    (2000/MAIN_LOOP_POLLING_DELAY) // 2000ms

#define PATTERN4_INDEX          3
#define PATTERN4_NUM            0x0280 //xx101xxxxxxx
#define PATTERN4_MASK           0x0380 //xx101xxxxxxx
#define PATTERN4_SHIFT_BIT      7
#define PATTERN4_DELAY_COUNT    (3000/MAIN_LOOP_POLLING_DELAY) // 3000ms

#define PATTERN5_INDEX          4
#define PATTERN5_NUM            0x0380 //xx111xxxxxxx
#define PATTERN5_MASK           0x0380 //xx111xxxxxxx
#define PATTERN5_SHIFT_BIT      7
#define PATTERN5_DELAY_COUNT    (4000/MAIN_LOOP_POLLING_DELAY) // 4000ms

#define PATTERN6_INDEX          5
#define PATTERN6_NUM            0x03F0 //xx111111xxxx
#define PATTERN6_MASK           0x03F0 //xx111111xxxx
#define PATTERN6_SHIFT_BIT      4
#define PATTERN6_DELAY_COUNT    (5000/MAIN_LOOP_POLLING_DELAY) // 5000ms

#define PATTERN_NUM             6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned short current_ledbar_value = LEDBAR_LED_ALL_ON_MASK;
static MPD_shift_pattern_TypeEnum current_ledbar_shift_pattern = L_TO_R;
static int pattern_count[PATTERN_NUM] = {0};

static int current_MPD_speed_count = 0;
static int current_MPD_speed_count_setting = 0;

static int current_MPD_speed_collection_delay_count = MPD_SPEED_COLLECTION_WINDOW_DELAY_COUNT;
/* Private function prototypes -----------------------------------------------*/

void Hardware_init(void);
static void ledbar_mpd_display_update(void);
static void mpd_count_reset(void);
static void mpd_count_polling(void);
static void mpd_speed_update(void);

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

    current_MPD_speed_count_setting = PATTERN3_DELAY_COUNT;
    current_MPD_speed_count = current_MPD_speed_count_setting;
    current_MPD_speed_collection_delay_count = MPD_SPEED_COLLECTION_WINDOW_DELAY_COUNT;

	/* Main processing loop */
    while (1) {

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

        if( current_MPD_speed_collection_delay_count == 0 ) {
            current_MPD_speed_collection_delay_count = MPD_SPEED_COLLECTION_WINDOW_DELAY_COUNT;
            mpd_speed_update();
            mpd_count_reset();
        } else {
            mpd_count_polling();
        }

        if( current_MPD_speed_count == 0 ) {
            ledbar_mpd_display_update();
            current_MPD_speed_count = current_MPD_speed_count_setting;
        }

		HAL_Delay(MAIN_LOOP_POLLING_DELAY);		//Delay for 2.5s
        current_MPD_speed_count--;
        current_MPD_speed_collection_delay_count--;
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
}

static void ledbar_mpd_display_update(void) {
    if( current_ledbar_shift_pattern == L_TO_R ) {
        if( current_ledbar_value & LEDBAR_MPD_SHIFT_L_TO_R_END ) {
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
	debug_printf("%x\n\r", current_ledbar_value);
    s4527438_hal_lta1000g_write(current_ledbar_value);
}

static void mpd_count_reset(void) {
        pattern_count[PATTERN1_INDEX] = 0;
        pattern_count[PATTERN2_INDEX] = 0;
        pattern_count[PATTERN3_INDEX] = 0;
        pattern_count[PATTERN4_INDEX] = 0;
        pattern_count[PATTERN5_INDEX] = 0;
        pattern_count[PATTERN6_INDEX] = 0;
}

static void mpd_count_polling(void) {
    int read_value;
    unsigned short ADC_value = 0; 

    read_value = s4527438_hal_joystick_x_read();
    ADC_value = ((unsigned short)read_value)&0x3FF;

    if( ((read_value&PATTERN1_MASK) == PATTERN1_NUM) ) {
        pattern_count[PATTERN1_INDEX]++;
    }
    if( ((read_value&PATTERN2_MASK) == PATTERN2_NUM) ) {
        pattern_count[PATTERN2_INDEX]++;
    }
    if( ((read_value&PATTERN3_MASK) == PATTERN3_NUM) ) {
        pattern_count[PATTERN3_INDEX]++;
    }
    if( ((read_value&PATTERN4_MASK) == PATTERN4_NUM) ) {
        pattern_count[PATTERN4_INDEX]++;
    }
    if( ((read_value&PATTERN5_MASK) == PATTERN5_NUM) ) {
        pattern_count[PATTERN5_INDEX]++;
    }
    if( ((read_value&PATTERN6_MASK) == PATTERN6_NUM) ) {
        pattern_count[PATTERN6_INDEX]++;
    }
}

static void mpd_speed_update(void) {
    int MPD_speed_count_setting = 0;

    MPD_speed_count_setting = PATTERN1_DELAY_COUNT;
    if( pattern_count[PATTERN2_INDEX] > pattern_count[PATTERN1_INDEX] ) {
        MPD_speed_count_setting = PATTERN2_DELAY_COUNT;
    }
    if( pattern_count[PATTERN3_INDEX] > pattern_count[PATTERN2_INDEX] ) {
        MPD_speed_count_setting = PATTERN3_DELAY_COUNT;
    }
    if( pattern_count[PATTERN4_INDEX] > pattern_count[PATTERN3_INDEX] ) {
        MPD_speed_count_setting = PATTERN4_DELAY_COUNT;
    }
    if( pattern_count[PATTERN5_INDEX] > pattern_count[PATTERN4_INDEX] ) {
        MPD_speed_count_setting = PATTERN5_DELAY_COUNT;
    }
    if( pattern_count[PATTERN6_INDEX] > pattern_count[PATTERN5_INDEX] ) {
        MPD_speed_count_setting = PATTERN6_DELAY_COUNT;
    }

    // Check is need to update
    if( current_MPD_speed_count_setting != MPD_speed_count_setting ) {
        current_MPD_speed_count_setting = MPD_speed_count_setting;
        current_MPD_speed_count = current_MPD_speed_count_setting;
    }
}

