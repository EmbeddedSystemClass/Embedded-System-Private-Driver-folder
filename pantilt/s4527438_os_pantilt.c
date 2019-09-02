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

#include "s4527438_hal_pantilt.h"

/* Private typedef -----------------------------------------------------------*/
struct Message {    /* Message consists of sequence number and payload string */
    int Sequence_Number;
    int angle;
    char Payload[10];
};
/* Private define ------------------------------------------------------------*/
#define mainRECEIVERTASK_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainRECEIVERTASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTaskPantiltOsHandle;
QueueHandle_t s4527438QueuePan;
QueueHandle_t s4527438QueueTilt;

SemaphoreHandle_t s4527438SemaphorePanLeft = NULL;
SemaphoreHandle_t s4527438SemaphorePanRight = NULL; 

SemaphoreHandle_t s4527438SemaphoreTiltUp = NULL;
SemaphoreHandle_t s4527438SemaphoreTiltDown = NULL;

static QueueSetHandle_t xQueueSet;
/* Private function prototypes -----------------------------------------------*/
void s4527438_os_pantilt_init(void);
void s4527438_os_pantilt_deinit(void);

void s4527438_os_pan_write_angle(int angle);
void s4527438_os_tilt_write_angle(int angle);

void s4527438_os_pan_left(void);
void s4527438_os_pan_right(void);

void s4527438_os_tilt_up(void);
void s4527438_os_tilt_down(void);

static void Receiver_Task( void );

void s4527438_os_pantilt_init(void) {
    // Harware init
    s4527438_hal_pantilt_init();

    s4527438SemaphorePanLeft = xSemaphoreCreateBinary();
    s4527438SemaphorePanRight = xSemaphoreCreateBinary();

    s4527438SemaphoreTiltUp = xSemaphoreCreateBinary();
    s4527438SemaphoreTiltDown = xSemaphoreCreateBinary();

    s4527438QueuePan = xQueueCreate(10, sizeof(struct Message));       /* Create queue of length 10 Message items */
    s4527438QueueTilt = xQueueCreate(10, sizeof(struct Message));       /* Create queue of length 10 Message items */

    /* Create QueueSet */
    xQueueSet = xQueueCreateSet(10 * 2 + 4);    //Size of Queueset = Size of Queue (10) * 2 + Size of Binary Semaphore (1) * 4

    xQueueAddToSet(s4527438SemaphorePanLeft, xQueueSet);
    xQueueAddToSet(s4527438SemaphorePanRight, xQueueSet);

    xQueueAddToSet(s4527438SemaphoreTiltUp, xQueueSet);
    xQueueAddToSet(s4527438SemaphoreTiltDown, xQueueSet);

    xQueueAddToSet(s4527438QueuePan, xQueueSet);
    xQueueAddToSet(s4527438QueueTilt, xQueueSet);

    xTaskCreate( (void *) &Receiver_Task, (const signed char *) "RECV", mainRECEIVERTASK_STACK_SIZE, NULL, mainRECEIVERTASK_PRIORITY, &xTaskPantiltOsHandle );
}

void s4527438_os_pantilt_deinit(void) {
    // stop task
    vTaskDelete(xTaskPantiltOsHandle);

    // empty queue set
    xQueueRemoveFromSet(s4527438SemaphorePanLeft, xQueueSet);
    xQueueRemoveFromSet(s4527438SemaphorePanRight, xQueueSet);

    xQueueRemoveFromSet(s4527438SemaphoreTiltUp, xQueueSet);
    xQueueRemoveFromSet(s4527438SemaphoreTiltDown, xQueueSet);

    xQueueRemoveFromSet(s4527438QueuePan, xQueueSet);
    xQueueRemoveFromSet(s4527438QueueTilt, xQueueSet);

    // Delete semphore
    vSemaphoreDelete(s4527438SemaphorePanLeft);
    vSemaphoreDelete(s4527438SemaphorePanRight);

    vSemaphoreDelete(s4527438SemaphoreTiltUp);
    vSemaphoreDelete(s4527438SemaphoreTiltDown);
}

void s4527438_os_pan_write_angle(int angle) {
    struct Message SendMessage;
    SendMessage.angle = angle;

    if( angle < -70 || angle > 70) {
        return;
    }

    if (s4527438QueuePan != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueuePan, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_tilt_write_angle(int angle) {
    struct Message SendMessage;
    SendMessage.angle = angle;

    if( angle < -70 || angle > 70) {
        return;
    }

    if (s4527438QueueTilt != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueTilt, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_pan_left(void) {
    if (s4527438SemaphorePanLeft != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438SemaphorePanLeft);
    }
}

void s4527438_os_pan_right(void) {
    if (s4527438SemaphorePanRight != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438SemaphorePanRight);
    }
}

void s4527438_os_tilt_up(void) {
    if (s4527438SemaphoreTiltUp != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438SemaphoreTiltUp);
    }
}

void s4527438_os_tilt_down(void) {
    if (s4527438SemaphoreTiltDown != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438SemaphoreTiltDown);
    }
}

void Receiver_Task( void ) {

    struct Message RecvMessage;
    QueueSetMemberHandle_t xActivatedMember;
    int currentAngle = 0;

    for (;;) {

        xActivatedMember = xQueueSelectFromSet(xQueueSet, 0);

        /* Which set member was selected?  Receives/takes can use a block time
        of zero as they are guaranteed to pass because xQueueSelectFromSet()
        would not have returned the handle unless something was available. */
        if (xActivatedMember == s4527438QueuePan) {

            /* Receive item */
            xQueueReceive( s4527438QueuePan, &RecvMessage, 0 );

            /* display received item */
            pantilt_angle_write(PAN_TYPE, RecvMessage.angle);

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: angle = %d\n\r", RecvMessage.angle);
            portEXIT_CRITICAL();
#endif

        } else if (xActivatedMember == s4527438QueueTilt) {

            /* Receive item */
            xQueueReceive( s4527438QueueTilt, &RecvMessage, 0 );

            /* display received item */
            pantilt_angle_write(TILT_TYPE, RecvMessage.angle);

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: angle = %d\n\r", RecvMessage.angle);
            portEXIT_CRITICAL();
#endif

        } else if (xActivatedMember == s4527438SemaphorePanLeft) {  /* Check if LED semaphore occurs */

            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake(s4527438SemaphorePanLeft, 0 );

            currentAngle = pantilt_angle_read(PAN_TYPE);
            pantilt_angle_write(PAN_TYPE, (currentAngle - 5));

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: PanLeft\n\r");
            portEXIT_CRITICAL();
#endif
        } else if (xActivatedMember == s4527438SemaphorePanRight) {   /* Check if pb semaphore occurs */

            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake( s4527438SemaphorePanRight, 0 );

            currentAngle = pantilt_angle_read(PAN_TYPE);
            pantilt_angle_write(PAN_TYPE, (currentAngle + 5));

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: PanRight\n\r");
            portEXIT_CRITICAL();
#endif
        } else if (xActivatedMember == s4527438SemaphoreTiltUp) {  /* Check if LED semaphore occurs */

            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake(s4527438SemaphoreTiltUp, 0 );

            currentAngle = pantilt_angle_read(TILT_TYPE);
            pantilt_angle_write(TILT_TYPE, (currentAngle + 5));

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: TiltUp\n\r");
            portEXIT_CRITICAL();
#endif
        } else if (xActivatedMember == s4527438SemaphoreTiltDown) {   /* Check if pb semaphore occurs */

            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake( s4527438SemaphoreTiltDown, 0 );

            currentAngle = pantilt_angle_read(TILT_TYPE);
            pantilt_angle_write(TILT_TYPE, (currentAngle - 5));

#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: TiltDown\n\r");
            portEXIT_CRITICAL();
#endif
        }

        /* Delay for 10ms */
        vTaskDelay(1);
    }
}



