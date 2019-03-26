/** 
 **************************************************************
 * @file mylib/s4527438_hal_atimer.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib atimer driver
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
#include "s4527438_hal_atimer.h"

#ifdef S4527438_HAL_ATIMER_CONFIG
#include "s4527438_hal_atimer_config.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define INT_UPPER_BOUNDARY  0x7FFFU
#define UNSIGNED_INT_UPPER_BOUNDARY  0xFFFFU

#define     ATIMER_TIM                             TIM3 // TIM3
#define     ATIMER_TIMx_CLK_ENABLE()               __HAL_RCC_TIM3_CLK_ENABLE()
#define     ATIMER_TIMx_GPIO_AF_CHANNEL            GPIO_AF2_TIM3
#define     ATIMER_TIMx_IRQn                       TIM3_IRQn

#define     ATIMER_CLOCK_LOWER_BOUND               S4527438_HAL_CLOCK_LOWER_BOUND    // 8000000(system core clock/2) / 65535(16-bit bus)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int atimerPeriodVal = S4527438_HAL_ATIMER_PERIOD;
static int atimerClkspeedVal = S4527438_HAL_ATIMER_CLKSPEED;
static uint32_t atimerPrescalerVal = 0;
int atimerCounterVal = 0;
static int32_t atimerCounterValMax = 0;

static TIM_HandleTypeDef TimHandle;
/* Private function prototypes -----------------------------------------------*/
void TIM3_IRQHandler(void);

void s4527438_hal_atimer_init(void) {
    // Timer 4 clock enable
    ATIMER_TIMx_CLK_ENABLE();

    atimerPrescalerVal = (uint32_t) (((SystemCoreClock / 2) / atimerClkspeedVal) - 1);

    atimerCounterValMax = (uint32_t)INT_UPPER_BOUNDARY / atimerPeriodVal;
    /* Configure Timer 3 settings */
    TimHandle.Instance = ATIMER_TIM;                   //Enable Timer 3
    TimHandle.Init.Period = (uint32_t)(atimerPeriodVal * atimerClkspeedVal / 1000 - 1);
    TimHandle.Init.Prescaler = atimerPrescalerVal;   //Set presale value
    TimHandle.Init.ClockDivision = 0;            //Set clock division
    TimHandle.Init.RepetitionCounter = 0;        // Set Reload Value
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP; //Set timer to count up.

    /* Initialise Timer */
    HAL_TIM_Base_Init(&TimHandle);

    /* Set priority of Timer 3 update Interrupt [0 (HIGH priority) to 15(LOW priority)] */
    /*  DO NOT SET INTERRUPT PRIORITY HIGHER THAN 3 */
    HAL_NVIC_SetPriority(ATIMER_TIMx_IRQn, 10, 0);     //Set Main priority to 10 and sub-priority to 0.

    // Enable the Timer 3 interrupt
    HAL_NVIC_EnableIRQ(ATIMER_TIMx_IRQn);

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
    uint32_t test_result_period = 0 ;
    
    if( frequency < ATIMER_CLOCK_LOWER_BOUND ) {
        return;
    }
    test_result_period = (uint32_t)(atimerPeriodVal * frequency / 1000 - 1);

    if( test_result_period < 1 ) {
        return;
    }

    if( test_result_period > UNSIGNED_INT_UPPER_BOUNDARY ) {
        return;
    }
    atimerClkspeedVal = frequency;

    atimerPrescalerVal = (uint32_t) (((SystemCoreClock / 2) / atimerClkspeedVal) - 1);

    TimHandle.Init.Period = test_result_period;
    TimHandle.Init.Prescaler = atimerPrescalerVal;   //Set presale value

#ifdef DEBUG
    debug_printf("atimerClkspeedVal = %d\n", atimerClkspeedVal);
    debug_printf("atimerPrescalerVal = %d\n", atimerPrescalerVal);
#endif

    HAL_TIM_Base_Stop_IT(&TimHandle);
    HAL_TIM_Base_Init(&TimHandle);
    HAL_TIM_Base_Start_IT(&TimHandle);
}

void s4527438_hal_atimer_period_set(int period) {
    uint32_t test_result_period = 0 ;

    if( period < 1 ) {
        return;
    }

    test_result_period = (uint32_t)(period * atimerClkspeedVal / 1000 - 1);

    if( test_result_period < 1 ) {
        return;
    }

    if( test_result_period > UNSIGNED_INT_UPPER_BOUNDARY ) {
        return;
    }

    atimerPeriodVal = period;
    TimHandle.Init.Period = test_result_period;

    atimerCounterValMax = (uint32_t)INT_UPPER_BOUNDARY / atimerPeriodVal;

#ifdef DEBUG
    debug_printf("atimerClkspeedVal = %d\n", atimerClkspeedVal);
    debug_printf("atimerPeriodVal = %d\n", atimerPeriodVal);
#endif

    HAL_TIM_Base_Stop_IT(&TimHandle);
    HAL_TIM_Base_Init(&TimHandle);
    HAL_TIM_Base_Start_IT(&TimHandle);
}

void s4527438_hal_atimer_init_pin() {
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO Clock */
    S4527438_HAL_ATIMER_PINCLK();

    GPIO_InitStructure.Pin = S4527438_HAL_ATIMER_PIN;                //Pin
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;       //Set mode to be output alternate
    //GPIO_InitStructure.Pull = GPIO_PULLUP;          //Enable Pull up, down or no pull resister
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;          // Default is pull down , since timer event will send high 
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    //GPIO_InitStructure.Alternate = ATIMER_TIMx_GPIO_AF_CHANNEL;   //Set alternate function to be timer 2
    HAL_GPIO_Init(S4527438_HAL_ATIMER_PINPORT, &GPIO_InitStructure);   //Initialise Pin
}

static void atimer_isr_callback(void) {
    uint32_t read_value = 0;

    if( atimerCounterVal >= atimerCounterValMax ) {
        atimerCounterVal = 0;
    } else {
        atimerCounterVal++;
    }

    /* toggle_frequency = 0.5 * (1/new_timer_period) */
    if( !(atimerCounterVal & 0x01) ) {
        return;
    }
    read_value = HAL_GPIO_ReadPin(S4527438_HAL_ATIMER_PINPORT, S4527438_HAL_ATIMER_PIN);
    HAL_GPIO_WritePin(S4527438_HAL_ATIMER_PINPORT, S4527438_HAL_ATIMER_PIN, (read_value & 0x01)^0x01);    //Toggle pin value
}

/**
 * @brief Period elapsed callback in non blocking mode
 * @param htim: Pointer to a TIM_HandleTypeDef that contains the configuration information for the TIM module.
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIMER_TIM)
    {
        BRD_LEDRedToggle();
        atimer_isr_callback();
    }
}

//Override default mapping of this handler to Default_Handler
void TIM3_IRQHandler(void)
{
    BRD_LEDGreenToggle();
    HAL_TIM_IRQHandler(&TimHandle);
}

