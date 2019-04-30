/** 
 **************************************************************
 * @file mylib/cli/s4527438_cli_task.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib cli driver
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

#include "FreeRTOS_CLI.h"

#include "s4527438_cli_pantilt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Task Priorities ------------------------------------------------------------*/
#define mainCLI_PRIORITY                    ( tskIDLE_PRIORITY + 3 )
#define mainCLI_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTaskCliHandle;

/* Private function prototypes -----------------------------------------------*/
void CLI_Task(void);

void s4527438_cli_init(void) {
#ifdef S4527438_CLI_PANTILT_H_
    s4527438_cli_pantilt_init();
#endif

    xTaskCreate( (void *) &CLI_Task, (const signed char *) "CLI", mainCLI_TASK_STACK_SIZE, NULL, mainCLI_PRIORITY, &xTaskCliHandle );
}

void s4527438_cli_deinit(void) {
    vTaskDelete(xTaskCliHandle);
}

/**
  * @brief  CLI Receive Task.
  * @param  None
  * @retval None
  */
void CLI_Task(void) {

    int i;
    char cRxedChar;
    char cInputString[100];
    int InputIndex = 0;
    char *pcOutputString;
    BaseType_t xReturned;

    /* Initialise pointer to CLI output buffer. */
    memset(cInputString, 0, sizeof(cInputString));
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    for (;;) {

        /* Receive character from terminal */
        cRxedChar = debug_getc();

        /* Process if character if not Null */
        if (cRxedChar != '\0') {

            /* Echo character */
            debug_putc(cRxedChar);

            /* Process only if return is received. */
            if (cRxedChar == '\r') {

                //Put new line and transmit buffer
                debug_putc('\n');
                debug_flush();

                /* Put null character in command input string. */
                cInputString[InputIndex] = '\0';

                xReturned = pdTRUE;
                /* Process command input string. */
                while (xReturned != pdFALSE) {


                     /* Returns pdFALSE, when all strings have been returned */
                     xReturned = FreeRTOS_CLIProcessCommand( cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE );

                     /* Display CLI command output string (not thread safe) */
                     if(strlen(pcOutputString) > 0 ) {
                        portENTER_CRITICAL();
                        for (i = 0; i < strlen(pcOutputString); i++) {
                             debug_putc(*(pcOutputString + i));
                        }
                        debug_flush();
                        portEXIT_CRITICAL();
                        memset(pcOutputString,0x00,strlen(pcOutputString));
                     }

                     //vTaskDelay(5);  //Must delay between debug_printfs.
                 }

                 memset(cInputString, 0, sizeof(cInputString));
                 InputIndex = 0;

             } else {

                 debug_flush();      //Transmit USB buffer

                 if( cRxedChar == '\r' ) {

                     /* Ignore the character. */
                 } else if( cRxedChar == '\b' ) {

                     /* Backspace was pressed.  Erase the last character in the
                      string - if any.*/
                     if( InputIndex > 0 ) {
                         InputIndex--;
                         cInputString[ InputIndex ] = '\0';
                     }

                 } else {

                     /* A character was entered.  Add it to the string
                        entered so far.  When a \n is entered the complete
                        string will be passed to the command interpreter. */
                     if( InputIndex < 20 ) {
                         cInputString[ InputIndex ] = cRxedChar;
                         InputIndex++;
                     }
                 }
             }
         }

         vTaskDelay(50);
     }
}


