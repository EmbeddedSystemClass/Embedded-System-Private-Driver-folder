/**
  ******************************************************************************
  * @file    blink/main.c
  * @author  MDS
  * @date    27112018
  * @brief   Nucleo429ZI onboard LED flashing example.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_os_pantilt.h"
#include "s4527438_os_joystick.h"
#include "s4527438_cli_task.h"

#include "FreeRTOS.h"
#include "task.h"

#include "s4527438_hal_sysmon.h"

#ifdef S4527438_SYSMON_CONF_INC
#include "s4527438_hal_sysmon_config.h"
#endif

/* Private typedef -----------------------------------------------------------*/
 typedef enum
 {
     eNeedDelete = 0,
     eNeedCreate,      
     eNoCmd,      
 } eTaskToDo;
/* Private define ------------------------------------------------------------*/
#define mainTASK1_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainTASK1_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 2 )

#define mainTASK2_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainTASK2_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 2 )

#define mainTASK3_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainTASK3_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 2 )

#define mainTASKControl_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainTASKControl_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 2 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTask1OsHandle;
static TaskHandle_t xTask2OsHandle;
static TaskHandle_t xTask3OsHandle;
static TaskHandle_t xTaskControlOsHandle;
static eTaskToDo task2ToDo = eNoCmd;

/* Private function prototypes -----------------------------------------------*/
static void Hardware_init();
static void Task1( void );
static void Task2( void );
static void Task3( void );
static void TaskControl( void );

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {
    BRD_init();
    Hardware_init();

    s4527438_os_pantilt_init();
    s4527438_os_joystick_init();
    s4527438_cli_init();
    portENABLE_INTERRUPTS();
	
    // Task 1
    xTaskCreate( (void *) &Task1, (const signed char *) "Task1", mainTASK1_STACK_SIZE, NULL, mainTASK1_PRIORITY, &xTask1OsHandle );

    // Task 2
    xTaskCreate( (void *) &Task2, (const signed char *) "Task2", mainTASK2_STACK_SIZE, NULL, mainTASK2_PRIORITY, &xTask2OsHandle );

    // Task 3
    xTaskCreate( (void *) &Task3, (const signed char *) "Task3", mainTASK3_STACK_SIZE, NULL, mainTASK3_PRIORITY, &xTask3OsHandle );

    // Task 4
    xTaskCreate( (void *) &TaskControl, (const signed char *) "TaskControl", mainTASKControl_STACK_SIZE, NULL, mainTASKControl_PRIORITY, &xTaskControlOsHandle );

    vTaskStartScheduler();
    return 0;
}

void Task1( void ) {

    for (;;) {
        s4527438_hal_sysmon_chan0_set();

        BRD_LEDBlueToggle();

        /* Extra Delay for 3ms */
        vTaskDelay(3);

        s4527438_hal_sysmon_chan0_clr();

        /* Wait for 1ms */
        vTaskDelay(1);
    }
}

void Task2( void ) {

    for (;;) {
        if( task2ToDo == eNeedDelete ) {
            task2ToDo = eNeedCreate;
            vTaskDelete(NULL);
        }
        s4527438_hal_sysmon_chan1_set();


        /* Extra Delay for 3ms */
        vTaskDelay(3);

        s4527438_hal_sysmon_chan1_clr();

        /* Wait for 1ms */
        vTaskDelay(1);
    }
}

void Task3( void ) {

    for (;;) {
        s4527438_hal_sysmon_chan2_set();

        BRD_LEDGreenToggle();
        /* Extra Delay for 3ms */
        vTaskDelay(1);

        s4527438_hal_sysmon_chan2_clr();

        /* Wait for 1ms */
        //vTaskDelay(1);
    }
}

void TaskControl( void ) {

    for (;;) {

        /* Check is joystick triggered*/
        if( s4527438_os_joystick_is_switch_triggered() ) {
            //portENTER_CRITICAL();
            if( eTaskGetState(xTask2OsHandle) != eDeleted ) {
                task2ToDo = eNeedDelete;
                BRD_LEDRedOn();
            } else if( eTaskGetState(xTask2OsHandle) == eDeleted || task2ToDo == eNeedCreate ) {
                BRD_LEDRedOff();
                xTaskCreate( (void *) &Task2, (const signed char *) "Task2", mainTASK2_STACK_SIZE, NULL, mainTASK2_PRIORITY, &xTask2OsHandle );
                task2ToDo = eNoCmd;
            }
            s4527438_os_joystick_switch_reset();
            //portEXIT_CRITICAL();
        }

        /* Wait for 1ms */
        vTaskDelay(100);
    }
}

void Hardware_init( void ) {

    portDISABLE_INTERRUPTS();   //Disable interrupts

    BRD_LEDInit();              //Initialise Blue LED
    BRD_LEDBlueOff();               //Turn off Blue LED
    BRD_LEDRedOff();               //Turn off Blue LED
    BRD_LEDGreenOff();               //Turn off Blue LED

    s4527438_hal_sysmon_init();
    portENABLE_INTERRUPTS();    //Enable interrupts

}
