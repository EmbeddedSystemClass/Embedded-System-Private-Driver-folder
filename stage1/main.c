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
typedef struct
{
  uint32_t                  tick_count;
  uint32_t                  triggered;
} JOYSTICK_Debounce_TypeDef;

/* Private define ------------------------------------------------------------*/
/*********************  JOYSTICK SWITCH BUTTON  *****************************/
#define JOYSTICK_BUTTON_PIN GPIO_PIN_7     // D51 : PD7
#define JOYSTICK_BUTTON_GPIO_PORT GPIOD
#define JOYSTICK_BUTTON_GPIO_CLK_ENABLE() __GPIOD_CLK_ENABLE()
#define JOYSTICK_BUTTON_EXTI_LINE GPIO_PIN_7
#define JOYSTICK_BUTTON_EXTI_IRQn EXTI9_5_IRQn

#define JOYSTICK_DEBOUNCE_TIME_LOWER_BOUND          (150U) // 100 ms
#define JOYSTICK_DEBOUNCE_TIME_UPPER_BOUND          (2000U) // 2 s
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned short current_ledbar_status = LEDBAR_LED_ALL_ON_MASK;
static JOYSTICK_Debounce_TypeDef joystick_info = {0};

/* Private function prototypes -----------------------------------------------*/
static void joystick_switch_init(void);
static void joystick_switch_debounce_reset(void);
static uint32_t joystick_switch_debounce_is_triggered(void);
static void joystick_switch_debounce_update(void);

void EXTI15_10_IRQHandler(void);
void Hardware_init(void);

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
	
	/* Main processing loop */
    while (1) {

		debug_printf("LED Toggle time: %d\n\r", HAL_GetTick());	//Print debug message with system time (ms)
        
		/* Toggle all LEDs */
		//BRD_LEDRedToggle();
		//BRD_LEDGreenToggle();
		//BRD_LEDBlueToggle();

        read_value = s4527438_hal_joystick_x_read();
        ADC_value = ((unsigned short)read_value)&0x3FF;
        s4527438_hal_lta1000g_write(ADC_value);
		//HAL_Delay(1000);		//Delay for 1s
		HAL_Delay(100);		//Delay for 1s

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

    current_ledbar_status = LEDBAR_LED_ALL_ON_MASK;
    s4527438_hal_lta1000g_write(current_ledbar_status);

    /* Init Joystick IRQ */
    joystick_switch_init();
}

static void joystick_switch_init(void) {

    joystick_switch_debounce_reset();

    GPIO_InitTypeDef GPIO_InitStructure;

    JOYSTICK_BUTTON_GPIO_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    //GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    //GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin  = JOYSTICK_BUTTON_PIN;
    //GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    HAL_GPIO_Init(JOYSTICK_BUTTON_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(JOYSTICK_BUTTON_EXTI_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(JOYSTICK_BUTTON_EXTI_IRQn);
}

static void joystick_switch_debounce_reset(void) {
    joystick_info.tick_count = 0U;
    joystick_info.triggered = 0;
}

static uint32_t joystick_switch_debounce_is_triggered(void) {
    uint32_t    return_value = joystick_info.triggered;
    return return_value;
}

static void joystick_switch_debounce_update(void) {
    uint32_t cur_tick = HAL_GetTick();
    
    if( joystick_info.tick_count == 0 ) {
        joystick_info.tick_count = cur_tick;
        return;
    }

    if( (cur_tick - joystick_info.tick_count) >= JOYSTICK_DEBOUNCE_TIME_LOWER_BOUND  ) {
        if( (cur_tick - joystick_info.tick_count) < JOYSTICK_DEBOUNCE_TIME_UPPER_BOUND  ) {
            joystick_info.triggered = 1;
        } else {
            joystick_switch_debounce_reset();
        }
    }
    joystick_info.tick_count = cur_tick;
}

/**
 * @brief EXTI line detection callback
 * @param GPIO_Pin: Specifies the pins connected EXTI line
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    int read_value;

    if (GPIO_Pin == JOYSTICK_BUTTON_PIN)
    {
		BRD_LEDRedToggle();
        joystick_switch_debounce_update();

        if( joystick_switch_debounce_is_triggered() ) {
            joystick_switch_debounce_reset();
        }
    }
}


//Override default mapping of this handler to Default_Handler
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(JOYSTICK_BUTTON_PIN);
}

