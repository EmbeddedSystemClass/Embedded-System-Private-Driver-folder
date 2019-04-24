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

#include "s4527438_hal_pantilt.h"
#include "s4527438_cli_pantilt.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CLI_Command_Definition_t xPan = {  /* Structure that defines the "pan" command line command. */
    "pan",
    "pan: pan <Angle Degrees> or pan [left|right \r\n",
    prvPanCommand,
    1
};

CLI_Command_Definition_t xTilt = {  /* Structure that defines the "tilt" command line command. */
    "tilt",
    "tilt: tilt <Angle Degrees> or tilt [left|right] \r\n",
    prvTiltCommand,
    1
};
/* Private function prototypes -----------------------------------------------*/
void s4527438_cli_pantilt_init(void);
static BaseType_t prvPanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvTiltCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

void s4527438_cli_pantilt_init(void) {
    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xPan);
    FreeRTOS_CLIRegisterCommand(&xTilt);
}

static BaseType_t prvPanCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = null;
    int currentAngle = 0;

    /* Get parameters from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"right") == 0 ) {
        currentAngle = pantilt_angle_read(PAN_TYPE);
        pantilt_angle_write(PAN_TYPE, (currentAngle + 5));
    } else if( strcmp(cCmd_string,"left") == 0 ) {
        currentAngle = pantilt_angle_read(PAN_TYPE);
        pantilt_angle_write(PAN_TYPE, (currentAngle - 5));
    } else if( strlen(cCmd_string) > 0 ) {
        int target_value = 0;
        for(target_string = cCmd_string;target_string != NULL && target_string[0] != '\0';target_string++) {
            if( target_string[0] < '0' 
                || target_string[0] > '9' ) {
                return pdFALSE;
            }
        }
        target_value = atoi(cCmd_string);
        pantilt_angle_write(PAN_TYPE, target_value);
    } else {
        return pdFALSE;
    }

    /* Return pdFALSE, as there are no more strings to return */
    /* Only return pdTRUE, if more strings need to be printed */
    return pdFALSE;
}

static BaseType_t prvTiltCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = null;
    int currentAngle = 0;

    /* Get parameters from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"up") == 0 ) {
        currentAngle = pantilt_angle_read(TILT_TYPE);
        pantilt_angle_write(TILT_TYPE, (currentAngle + 5));
    } else if( strcmp(cCmd_string,"down") == 0 ) {
        currentAngle = pantilt_angle_read(TILT_TYPE);
        pantilt_angle_write(TILT_TYPE, (currentAngle - 5));
    } else if( strlen(cCmd_string) > 0 ) {
        int target_value = 0;
        for(target_string = cCmd_string;target_string != NULL && target_string[0] != '\0';target_string++) {
            if( target_string[0] < '0'
                || target_string[0] > '9' ) {
                return pdFALSE;
            }
        }
        target_value = atoi(cCmd_string);
        pantilt_angle_write(TILT_TYPE, target_value);
    } else {
        return pdFALSE;
    }

    /* Return pdFALSE, as there are no more strings to return */
    /* Only return pdTRUE, if more strings need to be printed */
    return pdFALSE;
}
