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

/* Private define ------------------------------------------------------------*/
#define RADIO_UPDATE_FREQ       17778
#define RADIO_CLOCK_SPEED       16000000
#define RADIO_PERIOD_TICK       (uint32_t)(RADIO_CLOCK_SPEED/RADIO_UPDATE_FREQ) - 1)

/* Definition for TIMx clock resources */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int halRadioFsmcurrentstate;

/* Private function prototypes -----------------------------------------------*/
void s4527438_hal_irremote_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_IC_InitTypeDef  TIM_ICInitStructure;
    uint16_t PrescalerValue = 0;

    BRD_LEDInit();      //Initialise Blue LED
    BRD_LEDRedOff();        //Turn off Blue LED

    /* Timer 1 clock enable */
    __TIM1_CLK_ENABLE();

    /* Enable the D0 Clock */
    __BRD_D38_GPIO_CLK(;

    /* Configure the D0 pin with TIM1 input capture */
    GPIO_InitStructure.Pin = BRD_D38_PIN;                //Pin
    GPIO_InitStructure.Mode =GPIO_MODE_AF_PP;       //Set mode to be output alternate
    GPIO_InitStructure.Pull = GPIO_NOPULL;          //Enable Pull up, down or no pull resister
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM1;   //Set alternate function to be timer 1
    HAL_GPIO_Init(BRD_D38_GPIO_PORT, &GPIO_InitStructure);   //Initialise Pin

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

    /* Set priority of Timer 3 Interrupt [0 (HIGH priority) to 15(LOW priority)] */
    HAL_NVIC_SetPriority(TIM1_IRQn, 10, 0); //Set Main priority ot 10 and sub-priority ot 0.

    /* Enable Timer 3 interrupt and interrupt vector */
    NVIC_EnableIRQ(TIM1_IRQn);

    /* Enable input capture for Timer 3, channel 4 */
    HAL_TIM_IC_Init(&TIM_Init);
    HAL_TIM_IC_ConfigChannel(&TIM_Init, &TIM_ICInitStructure, TIM_CHANNEL_4);

    /* Start Input Capture */
    HAL_TIM_IC_Start_IT(&TIM_Init, TIM_CHANNEL_4);
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

    /* Read and display the Input Capture value of Timer 3, channel 4 */
    input_capture_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);

    debug_printf("Input capture: %d\n\r", input_capture_value);

}

/**
  * @brief  Timer 1 Input Capture Interrupt handler
  * Override default mapping of this handler to Default_Handler
  * @param  None.
  * @retval None
  */
void TIM1_IRQHandler(void) {
    HAL_TIM1_IRQHandler(&TIM_Init);
    BRD_LEDRedToggle();
}



