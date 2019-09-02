/** 
 **************************************************************
 * @file mylib/s4527438_cli_pantilt.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib cli pantilt driver
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

#include "s4527438_cli_atimer.h"
#include "s4527438_os_atimer.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static BaseType_t prvAtimerGetSysCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

CLI_Command_Definition_t xAtimerGetSys = {  /* Structure that defines the "pan" command line command. */
    "getsys",
    "getsys: getsys \r\n",
    prvAtimerGetSysCommand,
    0
};

void s4527438_cli_atimer_init(void) {
    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xAtimerGetSys);
}

static BaseType_t prvAtimerGetSysCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_atimer_send_timer_print_event();

    return returnedValue;
}
