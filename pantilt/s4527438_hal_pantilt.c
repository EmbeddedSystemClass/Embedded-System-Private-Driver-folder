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
#include "s4527438_hal_pantilt.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Definition for TIMx clock resources */

#define PWM_PULSE_DURATION_TICK_MIN         6750            // PWM_PERIOD_TICK * 2.25%  
#define PWM_PULSE_DURATION_TICK_MIDDLE      21750           // PWM_PERIOD_TICK * 7.25%  
#define PWM_PULSE_DURATION_TICK_MAX         36750           // PWM_PERIOD_TICK * 12.25%  

#define PWM_PULSE_DURATION_TICK_MINUS_81    8250          // -81 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_72    9750          // -72 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_63    11250          // -63 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_54    12750          // -54 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_45    14250          // -45 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_36    15750          // -36 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_27    17250          // -27 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_18    18750          // -18 degree 
#define PWM_PULSE_DURATION_TICK_MINUS_9     20250          // -9 degree

#define PWM_PULSE_DURATION_TICK_9           23250          // 9 degree , 0000 1001
#define PWM_PULSE_DURATION_TICK_9_MASK      0x08         

#define PWM_PULSE_DURATION_TICK_18          24750          // 18 degree ,0001 0010
#define PWM_PULSE_DURATION_TICK_18_MASK     0x12          

#define PWM_PULSE_DURATION_TICK_27          26250          // 27 degree ,0001 1011 
#define PWM_PULSE_DURATION_TICK_27_MASK     0x18           

#define PWM_PULSE_DURATION_TICK_36          27750          // 36 degree ,0010 0100 
#define PWM_PULSE_DURATION_TICK_36_MASK     0x24 

#define PWM_PULSE_DURATION_TICK_45          29250          // 45 degree ,0010 1101 
#define PWM_PULSE_DURATION_TICK_45_MASK     0x2c 

#define PWM_PULSE_DURATION_TICK_54          30750          // 54 degree ,0011 0110 
#define PWM_PULSE_DURATION_TICK_54_MASK     0x34 

#define PWM_PULSE_DURATION_TICK_63          32250          // 63 degree ,0011 1111
#define PWM_PULSE_DURATION_TICK_63_MASK     0x3c

#define PWM_PULSE_DURATION_TICK_72          33750          // 72 degree ,0100 1000 
#define PWM_PULSE_DURATION_TICK_72_MASK     0x48 

#define PWM_PULSE_DURATION_TICK_81          35250          // 81 degree ,0101 0001 
#define PWM_PULSE_DURATION_TICK_81_MASK     0x50 


/* Definition for PWM Channel Pin (D6) : PE9 */
#ifdef DEBUG
#define PWM_TIM                             TIM1 // TIM1
#define PWM_TIMx_CLK_ENABLE()               __HAL_RCC_TIM1_CLK_ENABLE()

#define PWM_PERIOD                          20          //    20ms
#define PWM_CLKSPEED                        15000000    //    15 MHz
#define PWM_PERIOD_TICK                     (uint32_t)(666 - 1)          
#define PWM_PULSE_DEFAULT_DURATION_TICK     (uint32_t)(PWM_PERIOD_TICK/2) 

#define PWM_CHANNEL                         TIM_CHANNEL_1          
//#define PWM_TIMx_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOB_CLK_ENABLE();
//#define PWM_TIMx_GPIO_PORT_CHANNEL          GPIOB
//#define PWM_TIMx_GPIO_PIN_CHANNEL           GPIO_PIN_4
#define PWM_TIMx_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIMx_GPIO_PORT_CHANNEL          GPIOE
#define PWM_TIMx_GPIO_PIN_CHANNEL           GPIO_PIN_9
#define PWM_TIMx_GPIO_AF_CHANNEL            GPIO_AF1_TIM1

#else

#define PWM_TIM                             TIM1 // TIM1
#define PWM_TIMx_CLK_ENABLE()               __HAL_RCC_TIM1_CLK_ENABLE()

#define PWM_PERIOD                          20          //    20ms
#define PWM_CLKSPEED                        15000000    //    15 MHz
#define PWM_PERIOD_TICK                     (uint32_t)(PWM_PERIOD * PWM_CLKSPEED / 1000 )          
#define PWM_PULSE_DEFAULT_DURATION_TICK     (uint32_t)21750          // PWM_PERIOD_TICK * 7.25%  

#define PWM_CHANNEL                         TIM_CHANNEL_1          
#define PWM_TIMx_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOE_CLK_ENABLE();
#define PWM_TIMx_GPIO_PORT_CHANNEL          GPIOE
#define PWM_TIMx_GPIO_PIN_CHANNEL           GPIO_PIN_9
#define PWM_TIMx_GPIO_AF_CHANNEL            GPIO_AF1_TIM1
#endif /* DEBUG */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t uhPrescalerValue = 0;
static TIM_HandleTypeDef TimHandle;
static TIM_OC_InitTypeDef sConfig;

static int current_angle = 0;
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);

void s4527438_hal_pantilt_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    PWM_TIMx_CLK_ENABLE();

    PWM_TIMx_CHANNEL_GPIO_PORT();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Alternate = PWM_TIMx_GPIO_AF_CHANNEL;
    GPIO_InitStruct.Pin = PWM_TIMx_GPIO_PIN_CHANNEL;
    HAL_GPIO_Init(PWM_TIMx_GPIO_PORT_CHANNEL, &GPIO_InitStruct);

    /* Calculate Prescaler */
    uhPrescalerValue = (uint32_t)((SystemCoreClock/2) / PWM_CLKSPEED) - 1;

    TimHandle.Instance = PWM_TIM;
    TimHandle.Init.Prescaler         = uhPrescalerValue;
    TimHandle.Init.Period            = PWM_PERIOD_TICK;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    current_angle = 0;
    /* Set the pulse value for channel 1 */
    sConfig.Pulse = PWM_PULSE_DEFAULT_DURATION_TICK;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, PWM_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&TimHandle, PWM_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }


}

void pantilt_angle_write(int type, int angle) {
    if( type == PAN_TYPE ) {
        int change_degree = 0;
        unsigned int test_angle = 0;
        int is_minus = 0;

        if( angle > 0 ) {
            test_angle = angle;
        } else if( angle < 0 ) {
            test_angle = angle * (-1);
            is_minus = 1;
        } else { // == 0
            test_angle = 0;
        }

        if( test_angle & PWM_PULSE_DURATION_TICK_81_MASK ) {
            if( is_minus ) {
                current_angle = -81;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_81;
            } else {
                current_angle = 81;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_81;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_72_MASK) {
            if( is_minus ) {
                current_angle = -72;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_72;
            } else {
                current_angle = 72;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_72;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_63_MASK) {
            if( is_minus ) {
                current_angle = -63;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_63;
            } else {
                current_angle = 63;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_63;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_54_MASK) {
            if( is_minus ) {
                current_angle = -54;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_54;
            } else {
                current_angle = 54;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_54;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_45_MASK) {
            if( is_minus ) {
                current_angle = -45;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_45;
            } else {
                current_angle = 45;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_45;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_36_MASK) {
            if( is_minus ) {
                current_angle = -36;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_36;
            } else {
                current_angle = 36;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_36;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_27_MASK) {
            if( is_minus ) {
                current_angle = -27;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_27;
            } else {
                current_angle = 27;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_27;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_18_MASK) {
            if( is_minus ) {
                current_angle = -18;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_18;
            } else {
                current_angle = 18;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_18;
            }
        } else if(test_angle & PWM_PULSE_DURATION_TICK_9_MASK) {
            if( is_minus ) {
                current_angle = 9;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_9;
            } else {
                current_angle = -9;
                sConfig.Pulse = PWM_PULSE_DURATION_TICK_9;
            }
        } else {
            current_angle = 0;
            sConfig.Pulse = PWM_PULSE_DEFAULT_DURATION_TICK;
        }

        if (HAL_TIM_PWM_Stop(&TimHandle, PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, PWM_CHANNEL) != HAL_OK)
        {
            /* Configuration Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_Start(&TimHandle, PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }
    } else {
    }
}

int pantilt_angle_read(int type) {
    if( type == PAN_TYPE ) {
        return current_angle;
    }
    return 0;
}

static void Error_Handler(void)
{
    while (1)
    {
    }
}

