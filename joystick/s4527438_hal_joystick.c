 /** 
 **************************************************************
 * @file mylib/sxxxxxx_mylib_template.c
 * @author MyName - MyStudent ID
 * @date 22022018
 * @brief mylib template driver
 * REFERENCE: DON'T JUST COPY THIS BLINDLY.pdf 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * sxxxxxx_mylib_template_init() - intialise mylib template driver
 * sxxxxxx_mylib_template_set() - set mylib template driver 
 *************************************************************** 
 */
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

void s4527438_hal_joystick_init(void) {
    BRD_USER_BUTTON_GPIO_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin  = BRD_USER_BUTTON_PIN;
    HAL_GPIO_Init(BRD_USER_BUTTON_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(BRD_USER_BUTTON_EXTI_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(BRD_USER_BUTTON_EXTI_IRQn);


















    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_A0_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_A0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

    __ADC1_CLK_ENABLE();

    AdcHandle.Instance = (ADC_TypeDef *)(ADC1_BASE);                        //Use ADC1
    AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;    //Set clock prescaler
    AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;               //Set 12-bit data resolution
    AdcHandle.Init.ScanConvMode          = DISABLE;
    AdcHandle.Init.ContinuousConvMode    = DISABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion   = 0;
    AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;   //No Trigger
    AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;     //No Trigger
    AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;             //Right align data
    AdcHandle.Init.NbrOfConversion       = 1;
    AdcHandle.Init.DMAContinuousRequests = DISABLE;
    AdcHandle.Init.EOCSelection          = DISABLE;

    HAL_ADC_Init(&AdcHandle);       //Initialise ADC

    /* Configure ADC Channel */
    AdcChanConfig.Channel = BRD_A0_ADC_CHAN;                            //Use AO pin
    AdcChanConfig.Rank         = 1;
    AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfig.Offset       = 0;

    HAL_ADC_ConfigChannel(&AdcHandle, &AdcChanConfig);      //Initialise ADC channel
}

int joystick_readxy(ADC_HandleTypeDef* hadc) {
}

void s4527438_hal_lta1000g_write(unsigned short value) {
}
