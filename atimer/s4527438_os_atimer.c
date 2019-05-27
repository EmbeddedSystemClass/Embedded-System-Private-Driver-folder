/** 
 **************************************************************
 * @file mylib/s4527438_os_pantilt.c
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
#include "stm32f4xx_hal_conf.h"
#include "debug_printf.h"
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "s4527438_hal_atimer.h"
#include "s4527438_os_atimer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define mainRADIO_TASK_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainRADIO_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

#define QUEUE_BLOCK_TIME_AS_FSM_PERIOD_TICK 100

#define EVENT_NUM                           10
#define ATIMER_PERIOD_MS                    2

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTaskRadioOsHandle;
static SemaphoreHandle_t s4527438Semaphore_timer_print_event = NULL;
static QueueSetHandle_t xQueueSet;
/* Private function prototypes -----------------------------------------------*/
static void AtimerTask( void );
static int32_t count_max_value = 0;
static uint32_t over_max_value_count = 0;
static int atimerCurVal = 0;
static int atimerPreVal = 0;

void s4527438_os_atimer_init(void) {

    s4527438Semaphore_timer_print_event = xSemaphoreCreateBinary();
    xQueueSet = xQueueCreateSet(EVENT_NUM);
    xQueueAddToSet(s4527438Semaphore_timer_print_event, xQueueSet);

    count_max_value = 0;
    over_max_value_count = 0;

    xTaskCreate( (void *) &AtimerTask, (const signed char *) "AtimerTask", mainRADIO_TASK_STACK_SIZE, NULL, mainRADIO_TASK_PRIORITY, &xTaskRadioOsHandle );
}

void s4527438_os_atimer_send_timer_print_event(void) {
    if (s4527438Semaphore_timer_print_event != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438Semaphore_timer_print_event);
    }
}

static void AtimerTask( void ) {

    QueueSetMemberHandle_t xActivatedMember;

    /* Init atimer hal */
    s4527438_hal_atimer_init();
    /* Set 2 ms */
    s4527438_hal_atimer_period_set(ATIMER_PERIOD_MS);

    count_max_value = s4527438_hal_atimer_timer_max_value();

    for (;;) {

        xActivatedMember = xQueueSelectFromSet(xQueueSet, QUEUE_BLOCK_TIME_AS_FSM_PERIOD_TICK);

        /* Which set member was selected?  Receives/takes can use a block time
        of zero as they are guaranteed to pass because xQueueSelectFromSet()
        would not have returned the handle unless something was available. */
        if (xActivatedMember == s4527438Semaphore_timer_print_event) {  
            int cur_val_ms = 0;
            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake(s4527438Semaphore_timer_print_event, 0 );

            /* Update timer value to calculate overflow times */
            atimerPreVal = atimerCurVal;
            atimerCurVal = s4527438_hal_atimer_timer_read();
            if( atimerPreVal > atimerCurVal ) {
                over_max_value_count++;
            }

            cur_val_ms = s4527438_hal_atimer_timer_getms();
            debug_printf("[Cur Sys Time(ms)] : <%d>\n\r", over_max_value_count*count_max_value + cur_val_ms );
        }

        /* Update timer value to calculate overflow times */
        atimerPreVal = atimerCurVal;
        atimerCurVal = s4527438_hal_atimer_timer_read();
        if( atimerPreVal > atimerCurVal ) {
            over_max_value_count++;
        }

        /* Delay for 10ms */
        vTaskDelay(10);
        
    }
}

