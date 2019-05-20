/** 
 **************************************************************
 * @file mylib/s4527438_hal_pantilt.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib pantilt driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_irremote.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum{
    DETECT_RISING_EDGE,
    DETECT_FALLING_EDGE
}Edge_Detect_TypeEnum;

typedef struct{
    int last_tick_falling_detected;
    int current_tick_falling_detected;

    int last_tick_rising_detected;
    int current_tick_rising_detected;

    unsigned char is_ever_receive_event_leading;
}Wave_Recording_TypeStruct;

/* Private define ------------------------------------------------------------*/
#define RADIO_UPDATE_FREQ       17778
#define RADIO_CLOCK_SPEED       16000000
#define RADIO_PERIOD_TICK       (uint32_t)((RADIO_CLOCK_SPEED/RADIO_UPDATE_FREQ) - 1)

#define IRREMOTE_GPIO_PIN       BRD_D38_PIN
#define IRREMOTE_GPIO_PORT      BRD_D38_GPIO_PORT
#define IRREMOTE_GPIO_CLK()     __BRD_D38_GPIO_CLK()

#define IRREMOTE_TIM_CLOCK_ENB()    __TIM1_CLK_ENABLE()

#define WAVE_RECORDING_INIT(handle)     {\
                                            (handle)->last_tick_falling_detected = 0;\
                                            (handle)->current_tick_falling_detected = 0;\
                                            \
                                            (handle)->last_tick_rising_detected = 0;\
                                            (handle)->current_tick_rising_detected = 0;\
                                            (handle)->is_ever_receive_event_leading = 0;\
                                        }

#define WAVE_RECORDING_SET_CURRENT_RISING_TICK(handle,value)     {\
                                            (handle)->last_tick_rising_detected = (handle)->current_tick_rising_detected;\
                                            (handle)->current_tick_rising_detected = value;\
                                        }
#define WAVE_RECORDING_SET_CURRENT_FALLING_TICK(handle,value)     {\
                                            (handle)->last_tick_falling_detected = (handle)->current_tick_falling_detected;\
                                            (handle)->current_tick_falling_detected = value;\
                                        }
/* Definition for TIMx clock resources */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TIM_HandleTypeDef TIM_Init;
static Edge_Detect_TypeEnum current_edge_detect_state = DETECT_RISING_EDGE;
static int read_gpio_value = 0;
static Wave_Recording_TypeStruct wave_recording_handle;


/* Private function prototypes -----------------------------------------------*/
void s4527438_hal_irremote_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_IC_InitTypeDef  TIM_ICInitStructure;
    uint16_t PrescalerValue = 0;

    BRD_LEDInit();      //Initialise Blue LED
    BRD_LEDRedOff();        //Turn off Blue LED

    /* Timer 1 clock enable */
    IRREMOTE_TIM_CLOCK_ENB();

    /* Enable the D0 Clock */
    IRREMOTE_GPIO_CLK();

    /* Configure the D0 pin with TIM1 input capture */
    GPIO_InitStructure.Pin = IRREMOTE_GPIO_PIN;                //Pin
    GPIO_InitStructure.Mode =GPIO_MODE_AF_PP;       //Set mode to be output alternate
    //GPIO_InitStructure.Pull = GPIO_NOPULL;          //Enable Pull up, down or no pull resister
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;          //Enable Pull up, down or no pull resister
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM1;   //Set alternate function to be timer 1
    HAL_GPIO_Init(IRREMOTE_GPIO_PORT, &GPIO_InitStructure);   //Initialise Pin

    /* Compute the prescaler value. TIM1 system clock = 16MHz */
    PrescalerValue = (uint16_t) ((SystemCoreClock) / RADIO_CLOCK_SPEED) - 1;

    /* Configure Timer 1 settings */
    TIM_Init.Instance = TIM1;                   //Enable Timer 3
    TIM_Init.Init.Period = RADIO_PERIOD_TICK;          //Set for 100ms (10Hz) period
    TIM_Init.Init.Prescaler = PrescalerValue;   //Set presale value
    TIM_Init.Init.ClockDivision = 0;            //Set clock division
    TIM_Init.Init.RepetitionCounter = 0;        // Set Reload Value
    TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP; //Set timer to count up.

    /* Configure TIM1 Input capture */
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;         //Set to trigger on rising edge
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0;
    current_edge_detect_state = DETECT_RISING_EDGE;

    // Check current state
    read_gpio_value = HAL_GPIO_ReadPin(IRREMOTE_GPIO_PORT, IRREMOTE_GPIO_PIN);
    read_gpio_value = !!read_gpio_value;
    if( read_gpio_value ) {
        current_edge_detect_state = DETECT_FALLING_EDGE;
    } else {
        current_edge_detect_state = DETECT_RISING_EDGE;
    }
    WAVE_RECORDING_INIT(&wave_recording_handle);

    /* Set priority of Timer 3 Interrupt [0 (HIGH priority) to 15(LOW priority)] */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 10, 0); //Set Main priority ot 10 and sub-priority ot 0.

    /* Enable Timer 3 interrupt and interrupt vector */
    NVIC_EnableIRQ(TIM1_CC_IRQn);

    /* Enable input capture for Timer 3, channel 4 */
    HAL_TIM_IC_Init(&TIM_Init);
    HAL_TIM_IC_ConfigChannel(&TIM_Init, &TIM_ICInitStructure, TIM_CHANNEL_4);


    /* Start Input Capture */
    HAL_TIM_IC_Start_IT(&TIM_Init, TIM_CHANNEL_4);
}

void s4527438_hal_irremote_recv(void) {
    /* Check is current tick  */
    if(  )
}

/**
  * @brief  Timer 3 Input Capture Interrupt handler
  * @param  None.
  * @retval None
  */
void HAL_TIM1_IRQHandler (TIM_HandleTypeDef *htim) {

    unsigned int input_capture_value;

    /* Clear Input Capture Flag */
    __HAL_TIM_CLEAR_IT(&TIM_Init, TIM_IT_TRIGGER);

    /* Toggle LED */
    BRD_LEDRedToggle();

    /* Read and display the Input Capture value of Timer 1, channel 4 */
    input_capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    if( current_edge_detect_state == DETECT_RISING_EDGE ){
        WAVE_RECORDING_SET_CURRENT_RISING_TICK(&wave_recording_handle,input_capture_value);
    } else {
        WAVE_RECORDING_SET_CURRENT_FALLING_TICK(&wave_recording_handle,input_capture_value);
    }

    s4527438_hal_irremote_recv();

    current_edge_detect_state = (current_edge_detect_state == DETECT_RISING_EDGE)?DETECT_FALLING_EDGE:DETECT_RISING_EDGE;
    debug_printf("Input capture: %d\n\r", input_capture_value);

}

/**
  * @brief  Timer 1 Input Capture Interrupt handler
  * Override default mapping of this handler to Default_Handler
  * @param  None.
  * @retval None
  */
#if 0
void TIM1_IRQHandler(void) {
#endif
void TIM1_CC_IRQHandler(void) {
    HAL_TIM1_IRQHandler(&TIM_Init);
    BRD_LEDRedToggle();
}



