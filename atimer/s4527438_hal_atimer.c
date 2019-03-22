 /** 
 **************************************************************
 * @file mylib/s4527438_hal_atimer.c
 * @author MyName - s4527438
 * @date 20032019
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * sxxxxxx_mylib_template_init() - intialise mylib template driver
 * sxxxxxx_mylib_template_set() - set mylib template driver 
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_atimer.h"

#ifdef S4527438_HAL_ATIMER_CONFIG
#include "s4527438_hal_atimer_config.h"
#endif
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INT_UPPER_BOUNDARY  0x7FFFFFFFU
/* D10 : DPORT , 14 pin */
#define     S4527438_HAL_ATIMER_PIN         GPIO_PIN_14
#define     S4527438_HAL_ATIMER_PINPORT     GPIOD
#define     S4527438_HAL_ATIMER_PINCLK()    __GPIOD_CLK_ENABLE()
#define     S4527438_HAL_ATIMER_PERIOD      1000  //    1000ms
#define     S4527438_HAL_ATIMER_CLKSPEED    25000 //    25 kHz

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int atimerPeriodVal = S4527438_HAL_ATIMER_PERIOD;
static int atimerClkspeedVal = S4527438_HAL_ATIMER_CLKSPEED;
static uint32_t atimerPrescalerVal = 0;
int atimerCounterVal = 0;

static TIM_HandleTypeDef TimHandle;
/* Private function prototypes -----------------------------------------------*/
void TIM3_IRQHandler(void);

void s4527438_hal_atimer_init(void) {
    // Timer 3 clock enable
    __TIM3_CLK_ENABLE();

    atimerPrescalerVal = (uint16_t) ((SystemCoreClock / 2) / atimerClkspeedVal) - 1;

    /* Configure Timer 3 settings */
    TimHandle.Instance = TIM3;                   //Enable Timer 3
    TimHandle.Init.Period = (atimerPeriodVal * atimerClkspeedVal / 1000 );
    TimHandle.Init.Prescaler = atimerPrescalerVal;   //Set presale value
    TimHandle.Init.ClockDivision = 0;            //Set clock division
    TimHandle.Init.RepetitionCounter = 0;        // Set Reload Value
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; //Set timer to count up.

    /* Initialise Timer */
    HAL_TIM_Base_Init(&TimHandle);

    /* Set priority of Timer 3 update Interrupt [0 (HIGH priority) to 15(LOW priority)] */
    /*  DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
    HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);     //Set Main priority to 10 and sub-priority to 0.

    // Enable the Timer 3 interrupt
    HAL_NVIC_EnableIRQ(TIM3_IRQn);

    // Start Timer 3 base unit in interrupt mode
    HAL_TIM_Base_Start_IT(&TimHandle);
}

int s4527438_hal_atimer_timer_getms(void) {
    return atimerCounterVal*atimerPeriodVal;
}

void s4527438_hal_atimer_timer_reset(void){
    atimerCounterVal = 0;
}

void s4527438_hal_atimer_timer_pause(void) {
    HAL_TIM_Base_Stop_IT(&TimHandle);
}

void s4527438_hal_atimer_timer_resume(void) {
    HAL_TIM_Base_Start_IT(&TimHandle);
}

void s4527438_hal_atimer_clkspeed_set(int frequency) {
    atimerClkspeedVal = frequency;
}

void s4527438_hal_atimer_period_set(int period) {
    atimerPeriodVal = period;
}

void s4527438_hal_atimer_init_pin() {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    S4527438_HAL_ATIMER_PINCLK();

    GPIO_InitStructure.Pin = S4527438_HAL_ATIMER_PIN;                //Pin
    GPIO_InitStructure.Mode =GPIO_MODE_AF_PP;       //Set mode to be output alternate
    GPIO_InitStructure.Pull = GPIO_NOPULL;          //Enable Pull up, down or no pull resister
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;   //Set alternate function to be timer 2
    HAL_GPIO_Init(S4527438_HAL_ATIMER_PINPORT, &GPIO_InitStructure);   //Initialise Pin
}

static void atimer_isr_callback(void) {
#if 0
        if (atimerCounterVal > 1000) {
            atimerCounterVal = 0;
        }
#endif
        if( atimerCounterVal == INT_UPPER_BOUNDARY ) {
            atimerCounterVal = 0;
        } else {
            atimerCounterVal++;
        }
}

/**
 * @brief Period elapsed callback in non blocking mode
 * @param htim: Pointer to a TIM_HandleTypeDef that contains the configuration information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        atimer_isr_callback();
    }
}

//Override default mapping of this handler to Default_Handler
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}

