 /** 
 **************************************************************
 * @file mylib/joystick/s4527438_hal_joystick.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib joystick driver
 * REFERENCE: 
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
ADC_HandleTypeDef AdcHandle_x;
ADC_ChannelConfTypeDef AdcChanConfig_x;

ADC_HandleTypeDef AdcHandle_y;
ADC_ChannelConfTypeDef AdcChanConfig_y;

static JOYSTICK_Debounce_TypeDef joystick_info = {0};


/* Private function prototypes -----------------------------------------------*/
void EXTI9_5_IRQHandler(void);
static void joystick_init_x_adc(void);
static void joystick_init_y_adc(void);

static void joystick_switch_init(void);
static void joystick_switch_debounce_update(void);

void s4527438_hal_joystick_init(void) {
    joystick_init_x_adc();
    joystick_init_y_adc();

    /* Init Joystick IRQ */
    joystick_switch_init();
}

static void joystick_init_x_adc(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_A0_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_A0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

    __ADC1_CLK_ENABLE();

    AdcHandle_x.Instance = (ADC_TypeDef *)(ADC1_BASE);                      //Use ADC1
    AdcHandle_x.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;  //Set clock prescaler
    AdcHandle_x.Init.Resolution            = ADC_RESOLUTION12b;             //Set 12-bit data resolution
    AdcHandle_x.Init.ScanConvMode          = DISABLE;
    AdcHandle_x.Init.ContinuousConvMode    = DISABLE;
    AdcHandle_x.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle_x.Init.NbrOfDiscConversion   = 0;
    AdcHandle_x.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; //No Trigger
    AdcHandle_x.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;       //No Trigger
    AdcHandle_x.Init.DataAlign             = ADC_DATAALIGN_RIGHT;               //Right align data
    AdcHandle_x.Init.NbrOfConversion       = 1;
    AdcHandle_x.Init.DMAContinuousRequests = DISABLE;
    AdcHandle_x.Init.EOCSelection          = DISABLE;

    HAL_ADC_Init(&AdcHandle_x);     //Initialise ADC

    /* Configure ADC Channel */
    AdcChanConfig_x.Channel = BRD_A0_ADC_CHAN;                          //Use AO pin
    AdcChanConfig_x.Rank         = 1;
    AdcChanConfig_x.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfig_x.Offset       = 0;

    HAL_ADC_ConfigChannel(&AdcHandle_x, &AdcChanConfig_x);      //Initialise ADC channel
}

static void joystick_init_y_adc() {
    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_A1_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_A1_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(BRD_A1_GPIO_PORT, &GPIO_InitStructure);

    __ADC1_CLK_ENABLE();

    AdcHandle_y.Instance = (ADC_TypeDef *)(ADC1_BASE);                      //Use ADC1
    AdcHandle_y.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;  //Set clock prescaler
    AdcHandle_y.Init.Resolution            = ADC_RESOLUTION12b;             //Set 12-bit data resolution
    AdcHandle_y.Init.ScanConvMode          = DISABLE;
    AdcHandle_y.Init.ContinuousConvMode    = DISABLE;
    AdcHandle_y.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle_y.Init.NbrOfDiscConversion   = 0;
    AdcHandle_y.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; //No Trigger
    AdcHandle_y.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;       //No Trigger
    AdcHandle_y.Init.DataAlign             = ADC_DATAALIGN_RIGHT;               //Right align data
    AdcHandle_y.Init.NbrOfConversion       = 1;
    AdcHandle_y.Init.DMAContinuousRequests = DISABLE;
    AdcHandle_y.Init.EOCSelection          = DISABLE;

    HAL_ADC_Init(&AdcHandle_y);     //Initialise ADC

    /* Configure ADC Channel */
    AdcChanConfig_y.Channel = BRD_A1_ADC_CHAN;                          //Use AO pin
    AdcChanConfig_y.Rank         = 1;
    AdcChanConfig_y.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfig_y.Offset       = 0;

    HAL_ADC_ConfigChannel(&AdcHandle_y, &AdcChanConfig_y);      //Initialise ADC channel
}

int joystick_readxy(ADC_HandleTypeDef* hadc) {
    int return_value = 0;

    HAL_ADC_Start(hadc); // Start ADC conversion

    // Wait for ADC conversion to finish
    while (HAL_ADC_PollForConversion(hadc, 10) != HAL_OK);

    return_value = HAL_ADC_GetValue(hadc);
    return return_value;
}

static void joystick_switch_init(void) {

    s4527438_hal_joystick_switch_reset();

    GPIO_InitTypeDef GPIO_InitStructure;

    JOYSTICK_BUTTON_GPIO_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    //GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    //GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pull = GPIO_PULLUP; 
    GPIO_InitStructure.Pin  = JOYSTICK_BUTTON_PIN;
    //GPIO_InitStructure.Speed = GPIO_SPEED_FAST;         //Pin latency
    HAL_GPIO_Init(JOYSTICK_BUTTON_GPIO_PORT, &GPIO_InitStructure);

    HAL_NVIC_SetPriority(JOYSTICK_BUTTON_EXTI_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(JOYSTICK_BUTTON_EXTI_IRQn);
}

void s4527438_hal_joystick_switch_reset(void) {
    joystick_info.tick_count = 0U;
    joystick_info.triggered = 0;
}

uint32_t s4527438_hal_joystick_is_switch_triggered(void) {
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
            s4527438_hal_joystick_switch_reset();
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
    if (GPIO_Pin == JOYSTICK_BUTTON_PIN)
    {
        joystick_switch_debounce_update();
    }
}


//Override default mapping of this handler to Default_Handler
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(JOYSTICK_BUTTON_PIN);
}

