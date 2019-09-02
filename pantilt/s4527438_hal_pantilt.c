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
#include "s4527438_hal_pantilt.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Definition for TIMx clock resources */

#define PWM_PULSE_DURATION_TICK_MIN                 225            // PWM_PERIOD_TICK * 2.25%  
#define PWM_PULSE_DURATION_TICK_LOWER_LIMIT         252            // PWM_PERIOD_TICK * 2.25%  
#define PWM_PULSE_DURATION_TICK_MIDDLE              725           // PWM_PERIOD_TICK * 7.25%  
#define PWM_PULSE_DURATION_TICK_UPPER_LIMIT         1197           // PWM_PERIOD_TICK * 12.25%  
#define PWM_PULSE_DURATION_TICK_MAX                 1225           // PWM_PERIOD_TICK * 12.25%  

#define PWM_PULSE_DURATION_TICK_MINUS_81    275          // -81 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_72    325          // -72 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_63    375          // -63 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_54    425          // -54 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_45    475          // -45 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_36    525          // -36 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_27    575          // -27 degree  
#define PWM_PULSE_DURATION_TICK_MINUS_18    625          // -18 degree 
#define PWM_PULSE_DURATION_TICK_MINUS_9     675          // -9 degree

#define PWM_PULSE_DURATION_TICK_9           775             // 9 degree , 0000 1001
#define PWM_PULSE_DURATION_TICK_9_MASK      (uint32_t)0x00000008         

#define PWM_PULSE_DURATION_TICK_18          825             // 18 degree ,0001 0010
#define PWM_PULSE_DURATION_TICK_18_MASK     (uint32_t)0x00000010          

#define PWM_PULSE_DURATION_TICK_27          875             // 27 degree ,0001 1011 
#define PWM_PULSE_DURATION_TICK_27_MASK     (uint32_t)0x00000018           

#define PWM_PULSE_DURATION_TICK_36          925             // 36 degree ,0010 0100 
#define PWM_PULSE_DURATION_TICK_36_MASK     (uint32_t)0x00000020 

#define PWM_PULSE_DURATION_TICK_45          975             // 45 degree ,0010 1101 
#define PWM_PULSE_DURATION_TICK_45_MASK     (uint32_t)0x0000002c 

#define PWM_PULSE_DURATION_TICK_54          1025            // 54 degree ,0011 0110 
#define PWM_PULSE_DURATION_TICK_54_MASK     (uint32_t)0x00000030

#define PWM_PULSE_DURATION_TICK_63          1075            // 63 degree ,0011 1111
#define PWM_PULSE_DURATION_TICK_63_MASK     (uint32_t)0x0000003c

#define PWM_PULSE_DURATION_TICK_72          1125            // 72 degree ,0100 1000 
#define PWM_PULSE_DURATION_TICK_72_MASK     (uint32_t)0x00000040 

#define PWM_PULSE_DURATION_TICK_81          1175            // 81 degree ,0101 0001 
#define PWM_PULSE_DURATION_TICK_81_MASK     (uint32_t)0x00000050 


/* Definition for PWM Channel Pin (D6) : PE9 */
#define PWM_TIM                             TIM1 // TIM1
#define PWM_TIMx_CLK_ENABLE()               __HAL_RCC_TIM1_CLK_ENABLE()

#define PWM_PERIOD                          20          //    20ms
#define PWM_CLKSPEED                        500000        //    CLKSPEED >= 500kHz , CLKSPEED <= 3200kHz
#define PWM_PERIOD_TICK                     (uint32_t)((PWM_CLKSPEED / 1000)* PWM_PERIOD - 1) // 1 ms * PERIOD(ms)         
#define PWM_PULSE_DEFAULT_DURATION_TICK     (uint32_t)(PWM_PULSE_DURATION_TICK_MIDDLE) 

#define PAN_PWM_CHANNEL                         TIM_CHANNEL_1          
#define PAN_PWM_TIMx_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOE_CLK_ENABLE();
#define PAN_PWM_TIMx_GPIO_PORT_CHANNEL          GPIOE
#define PAN_PWM_TIMx_GPIO_PIN_CHANNEL           GPIO_PIN_9
#define PAN_PWM_TIMx_GPIO_AF_CHANNEL            GPIO_AF1_TIM1

#define TILT_PWM_CHANNEL                         TIM_CHANNEL_2
#define TILT_PWM_TIMx_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOE_CLK_ENABLE();
#define TILT_PWM_TIMx_GPIO_PORT_CHANNEL          GPIOE
#define TILT_PWM_TIMx_GPIO_PIN_CHANNEL           GPIO_PIN_11
#define TILT_PWM_TIMx_GPIO_AF_CHANNEL            GPIO_AF1_TIM1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t uhPrescalerValue = 0;
static TIM_HandleTypeDef TimHandle;
static TIM_OC_InitTypeDef sConfig;

static int pan_current_angle = 0;
static int tilt_current_angle = 0;
/* Private function prototypes -----------------------------------------------*/
static void Error_Handler(void);

void s4527438_hal_pantilt_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    PWM_TIMx_CLK_ENABLE();

    PAN_PWM_TIMx_CHANNEL_GPIO_PORT();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Alternate = PAN_PWM_TIMx_GPIO_AF_CHANNEL;
    GPIO_InitStruct.Pin = PAN_PWM_TIMx_GPIO_PIN_CHANNEL;
    HAL_GPIO_Init(PAN_PWM_TIMx_GPIO_PORT_CHANNEL, &GPIO_InitStruct);

    /* Calculate Prescaler */
    uhPrescalerValue = (uint32_t)((SystemCoreClock) / PWM_CLKSPEED) - 1;

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

    /* PAN : PWM Channel */
    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    pan_current_angle = 0;
    /* Set the pulse value for channel 1 */
    sConfig.Pulse = PWM_PULSE_DEFAULT_DURATION_TICK;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, PAN_PWM_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&TimHandle, PAN_PWM_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

    /* TILT : PWM Channel2 TIM1 */
    TILT_PWM_TIMx_CHANNEL_GPIO_PORT();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Alternate = TILT_PWM_TIMx_GPIO_AF_CHANNEL;
    GPIO_InitStruct.Pin = TILT_PWM_TIMx_GPIO_PIN_CHANNEL;
    HAL_GPIO_Init(TILT_PWM_TIMx_GPIO_PORT_CHANNEL, &GPIO_InitStruct);

    /*##-2- Configure the PWM channels #########################################*/
    /* Common configuration for all channels */
    tilt_current_angle = 0;
    /* Set the pulse value for channel 1 */
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TILT_PWM_CHANNEL) != HAL_OK)
    {
        /* Configuration Error */
        Error_Handler();
    }

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    if (HAL_TIM_PWM_Start(&TimHandle, TILT_PWM_CHANNEL) != HAL_OK)
    {
        /* PWM Generation Error */
        Error_Handler();
    }

}

void pantilt_angle_write(int type, int angle) {
    unsigned int test_angle = 0;
    int is_minus = 0;
    int current_angle = 0;

    if( angle > 0 ) {
        test_angle = angle;
    } else if( angle < 0 ) {
        test_angle = angle * (-1);
        is_minus = 1;
    } else { // == 0
        test_angle = 0;
    }

    current_angle = angle;
    if( ((test_angle & PWM_PULSE_DURATION_TICK_81_MASK)^PWM_PULSE_DURATION_TICK_81_MASK) == 0 ) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_81;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_81;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_72_MASK) ^ PWM_PULSE_DURATION_TICK_72_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_72;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_72;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_63_MASK) ^ PWM_PULSE_DURATION_TICK_63_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_63;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_63;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_54_MASK) ^ PWM_PULSE_DURATION_TICK_54_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_54;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_54;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_45_MASK) ^ PWM_PULSE_DURATION_TICK_45_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_45;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_45;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_36_MASK)^PWM_PULSE_DURATION_TICK_36_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_36;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_36;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_27_MASK) ^ PWM_PULSE_DURATION_TICK_27_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_27;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_27;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_18_MASK) ^ PWM_PULSE_DURATION_TICK_18_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_18;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_18;
        }
    } else if( ((test_angle & PWM_PULSE_DURATION_TICK_9_MASK) ^ PWM_PULSE_DURATION_TICK_9_MASK) == 0) {
        if( is_minus ) {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_9;
        } else {
            sConfig.Pulse = PWM_PULSE_DURATION_TICK_9;
        }
    } else {
        sConfig.Pulse = PWM_PULSE_DEFAULT_DURATION_TICK;
    }

    // Avoid hurt servo
    if( sConfig.Pulse > PWM_PULSE_DURATION_TICK_UPPER_LIMIT ) {
        sConfig.Pulse = PWM_PULSE_DURATION_TICK_81;
        current_angle = 85;
    } else if ( sConfig.Pulse < PWM_PULSE_DURATION_TICK_LOWER_LIMIT ) {
        sConfig.Pulse = PWM_PULSE_DURATION_TICK_MINUS_81;
        current_angle = -85;
    }

    if( type == PAN_TYPE ) {
        pan_current_angle = current_angle;
        if (HAL_TIM_PWM_Stop(&TimHandle, PAN_PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, PAN_PWM_CHANNEL) != HAL_OK)
        {
            /* Configuration Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_Start(&TimHandle, PAN_PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }
    } else if( type == TILT_TYPE ) {
        tilt_current_angle = current_angle;
        if (HAL_TIM_PWM_Stop(&TimHandle, TILT_PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TILT_PWM_CHANNEL) != HAL_OK)
        {
            /* Configuration Error */
            Error_Handler();
        }

        if (HAL_TIM_PWM_Start(&TimHandle, TILT_PWM_CHANNEL) != HAL_OK)
        {
            /* PWM Generation Error */
            Error_Handler();
        }
    }
}

int pantilt_angle_read(int type) {
    if( type == PAN_TYPE ) {
        return pan_current_angle;
    } else if( type == TILT_TYPE ) {
        return tilt_current_angle;
    }
    return 0;
}

static void Error_Handler(void)
{
    while (1)
    {
    }
}

