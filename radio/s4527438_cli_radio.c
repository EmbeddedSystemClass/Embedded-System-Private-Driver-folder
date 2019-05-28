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

#include "s4527438_cli_radio.h"
#include "s4527438_os_radio.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define X_MAX_VALUE     319
#define Y_MAX_VALUE     319
#define Z_MAX_VALUE     99

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t sorter_z_max_value = Z_MAX_VALUE;
/* Private function prototypes -----------------------------------------------*/
void s4527438_cli_radio_init(void);

static BaseType_t prvRadioSetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioSetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioSetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioGetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioJoin(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvRadioOriginCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioHeadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioVacuumCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioHeadDebugLowerValue(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvRadioOrbSetCp(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioOrbShow(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioOrbOrbOnOff(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioOrbTestSendRAE(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioRadioOrbDebugRAE4BitSwapOnOff(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static BaseType_t prvRadioLoadSorter(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRadioLoadOrb(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

CLI_Command_Definition_t xRadioOrigin = {  /* Structure that defines the "pan" command line command. */
    "origin",
    "origin: origin \r\n",
    prvRadioOriginCommand,
    0
};

CLI_Command_Definition_t xRadioMove = {  /* Structure that defines the "tilt" command line command. */
    "move",
    "move: move <x coordinate> <y coordinate> \r\n",
    prvRadioMoveCommand,
    2
};

CLI_Command_Definition_t xRadioHead = {  /* Structure that defines the "pan" command line command. */
    "head",
    "head: head <raise|lower> \r\n",
    prvRadioHeadCommand,
    1
};

CLI_Command_Definition_t xRadioVacuum = {  /* Structure that defines the "pan" command line command. */
    "vacuum",
    "vacuum: vacuum <on|off> \r\n",
    prvRadioVacuumCommand,
    1
};

CLI_Command_Definition_t xRadioHeadDebugLowerValue = {  /* Structure that defines the "pan" command line command. */
    "headlowerval",
    "headlowerval: headlowerval <1 ~ 99(mm)> \r\n",
    prvRadioHeadDebugLowerValue,
    1
};

/***********************************************************************************************************/
CLI_Command_Definition_t xRadioSetChan = {  /* Structure that defines the "pan" command line command. */
    "setchan",
    "setchan: setchan <chan> \r\n",
    prvRadioSetChan,
    1
};

CLI_Command_Definition_t xRadioGetChan = {  /* Structure that defines the "pan" command line command. */
    "getchan",
    "getchan: getchan \r\n",
    prvRadioGetChan,
    0
};

CLI_Command_Definition_t xRadioSetTxAddr = {  /* Structure that defines the "pan" command line command. */
    "settxaddr",
    "settxaddr: settxaddr <TXaddress(HEX)> \r\n",
    prvRadioSetTxAddr,
    1
};

CLI_Command_Definition_t xRadioSetRxAddr = {  /* Structure that defines the "pan" command line command. */
    "setrxaddr",
    "setrxaddr: setrxaddr <RXaddress(HEX)> \r\n",
    prvRadioSetRxAddr,
    1
};

CLI_Command_Definition_t xRadioGetRxAddr = {  /* Structure that defines the "pan" command line command. */
    "getrxaddr",
    "getrxaddr: getrxaddr \r\n",
    prvRadioGetRxAddr,
    0
};

CLI_Command_Definition_t xRadioGetTxAddr = {  /* Structure that defines the "pan" command line command. */
    "gettxaddr",
    "gettxaddr: gettxaddr \r\n",
    prvRadioGetTxAddr,
    0
};

CLI_Command_Definition_t xRadioJoin = {  /* Structure that defines the "pan" command line command. */
    "join",
    "join: join \r\n",
    prvRadioJoin,
    0
};

/***********************************************************************************************************/
CLI_Command_Definition_t xRadioOrbSetCp = {  /* Structure that defines the "pan" command line command. */
    "cp",
    "cp: cp <red|green|blue|yellow|orange> <x_coordinate> <y_coordinate> \r\n",
    prvRadioOrbSetCp,
    3
};
CLI_Command_Definition_t xRadioOrbShow = {  /* Structure that defines the "pan" command line command. */
    "show",
    "show: show <on|off> \r\n",
    prvRadioOrbShow,
    1
};
CLI_Command_Definition_t xRadioOrbOnOff = {  /* Structure that defines the "pan" command line command. */
    "orb",
    "orb: orb <on|off> \r\n",
    prvRadioOrbOrbOnOff,
    1
};
CLI_Command_Definition_t xRadioOrbTestSendRAE = {  /* Structure that defines the "pan" command line command. */
    "orbtest",
    "orbtest: orbtest <red|green|blue|yellow|orange> <x_coordinate> <y_coordinate> \r\n",
    prvRadioOrbTestSendRAE,
    3
};
CLI_Command_Definition_t xRadioOrbDebugRAE4BitSwapOnOff = {  /* Structure that defines the "pan" command line command. */
    "orb4bitswap",
    "orb4bitswap: orb4bitswap <on|off> \r\n",
    prvRadioRadioOrbDebugRAE4BitSwapOnOff,
    1
};
/***********************************************************************************************************/
CLI_Command_Definition_t xRadioLoadSorter = {  /* Structure that defines the "pan" command line command. */
    "loadsorter",
    "loadsorter: loadsorter <LAB sorter index(1 ~ 4))> \r\n",
    prvRadioLoadSorter,
    1
};

CLI_Command_Definition_t xRadioLoadOrb = {  /* Structure that defines the "pan" command line command. */
    "loadorb",
    "loadorb: loadorb <LAB ORB index(1 ~ 4)> \r\n",
    prvRadioLoadOrb,
    1
};

void s4527438_cli_radio_init(void) {
    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioSetChan);
    FreeRTOS_CLIRegisterCommand(&xRadioGetChan);
    FreeRTOS_CLIRegisterCommand(&xRadioSetTxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioSetRxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioGetRxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioGetTxAddr);
    FreeRTOS_CLIRegisterCommand(&xRadioJoin);

    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioOrbSetCp);
    FreeRTOS_CLIRegisterCommand(&xRadioOrbShow);
    FreeRTOS_CLIRegisterCommand(&xRadioOrbOnOff);
    FreeRTOS_CLIRegisterCommand(&xRadioOrbTestSendRAE);
    FreeRTOS_CLIRegisterCommand(&xRadioOrbDebugRAE4BitSwapOnOff);

    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioOrigin);
    FreeRTOS_CLIRegisterCommand(&xRadioMove);
    FreeRTOS_CLIRegisterCommand(&xRadioHead);
    FreeRTOS_CLIRegisterCommand(&xRadioVacuum);
    FreeRTOS_CLIRegisterCommand(&xRadioHeadDebugLowerValue);

    /* Register CLI commands */
    FreeRTOS_CLIRegisterCommand(&xRadioLoadSorter);
    FreeRTOS_CLIRegisterCommand(&xRadioLoadOrb);
}

/***********************************************************************************************************/
static BaseType_t prvRadioSetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        chan_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_chan(chan_index);

    return returnedValue;
}

static BaseType_t prvRadioGetChan(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_chan();

    return returnedValue;
}

static BaseType_t prvRadioSetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    uint8_t string_buffer[RADIO_OS_TX_RX_ADDR_STRING_WIDTH + 1];

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;

        memset(string_buffer,0x00,sizeof(string_buffer));
        memcpy(string_buffer,cCmd_string,lParam_len);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_txaddr(string_buffer);

    return returnedValue;
}

static BaseType_t prvRadioSetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t chan_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    uint8_t string_buffer[RADIO_OS_TX_RX_ADDR_STRING_WIDTH + 1];

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;

        memset(string_buffer,0x00,sizeof(string_buffer));
        memcpy(string_buffer,cCmd_string,lParam_len);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_rxaddr(string_buffer);

    return returnedValue;
}

static BaseType_t prvRadioGetRxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_rxaddr();

    return returnedValue;
}

static BaseType_t prvRadioGetTxAddr(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_get_txaddr();

    return returnedValue;
}

static BaseType_t prvRadioJoin(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_send_join_packet();

    return returnedValue;
}
/***********************************************************************************************************/
static BaseType_t prvRadioOriginCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {
 
    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    s4527438_os_radio_send_xyz_packet(0, 0, 0);

    return returnedValue;
}

static BaseType_t prvRadioMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t x_coordinate = 0;
    uint32_t y_coordinate = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        x_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    /* Get parameters 2 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        y_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_move_only_xy(x_coordinate, y_coordinate);

    return returnedValue;
}

static BaseType_t prvRadioHeadCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t z_coordinate = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"raise") == 0 ) {
        z_coordinate = 0;
    } else if( strcmp(cCmd_string,"lower") == 0 ) {
        z_coordinate = sorter_z_max_value;
    } else {
        return returnedValue;
    }

    s4527438_os_radio_move_only_z(z_coordinate);

    return returnedValue;
}

static BaseType_t prvRadioVacuumCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;
    Message_Vacuum_Action_Enum vacuum_action    =   VACUUM_OFF;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"on") == 0 ) {
        vacuum_action    =   VACUUM_ON;
    } else if( strcmp(cCmd_string,"off") == 0 ) {
        vacuum_action    =   VACUUM_OFF;
    } else {
        return returnedValue;
    }
    s4527438_os_radio_send_vacuum_packet(vacuum_action);

    return returnedValue;
}

static BaseType_t prvRadioHeadDebugLowerValue(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint8_t head_lower_val = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        head_lower_val = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    sorter_z_max_value = head_lower_val;
    s4527438_os_radio_head_debug_set_max_lower_val(head_lower_val);
    debug_printf("[Current channel]: <%d>\n\r", sorter_z_max_value);
    return returnedValue;
}
/***********************************************************************************************************/
static BaseType_t prvRadioOrbSetCp(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t x_coordinate = 0;
    uint32_t y_coordinate = 0;
    uint8_t  color_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t  *color_map[OBJ_COLOR_MAX + 1] = {"NO COLOR","red","green","blue","yellow","orange"};
        uint8_t  color_string[9];

        memset(color_string,0x00,sizeof(color_string));
        memcpy(color_string,cCmd_string,lParam_len);

        for( i = 0; i < (OBJ_COLOR_MAX + 1) ;i++) {
            if( strcmp(color_string,color_map[i]) == 0) {
                break;
            }
        }
        if( i == (OBJ_COLOR_MAX + 1) ) {
            color_index = 0;
        } else {
            color_index = i;
        }
    } else {
        return returnedValue;
    }

    /* Get parameters 2 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        x_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    /* Get parameters 3 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        y_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_set_cp(color_index,x_coordinate,y_coordinate);

    return returnedValue;
}

static BaseType_t prvRadioOrbShow(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint8_t on_off_switch = RADIO_TYPE_ON;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"on") == 0 ) {
        on_off_switch = RADIO_TYPE_ON;
    } else if( strcmp(cCmd_string,"off") == 0 ) {
        on_off_switch = RADIO_TYPE_OFF;
    } else {
        return returnedValue;
    }
    s4527438_os_radio_orb_show_rx_message(on_off_switch);

    return returnedValue;
}

static BaseType_t prvRadioOrbOrbOnOff(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint8_t on_off_switch = RADIO_TYPE_ON;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"on") == 0 ) {
        on_off_switch = RADIO_TYPE_ON;
    } else if( strcmp(cCmd_string,"off") == 0 ) {
        on_off_switch = RADIO_TYPE_OFF;
    } else {
        return returnedValue;
    }
    s4527438_os_radio_orb_on_off(on_off_switch);

    return returnedValue;
}

static BaseType_t prvRadioOrbTestSendRAE(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t x_coordinate = 0;
    uint32_t y_coordinate = 0;
    uint8_t  color_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t  *color_map[OBJ_COLOR_MAX + 1] = {"NO COLOR","red","green","blue","yellow","orange"};
        uint8_t  color_string[9];

        memset(color_string,0x00,sizeof(color_string));
        memcpy(color_string,cCmd_string,lParam_len);

        for( i = 0; i < (OBJ_COLOR_MAX + 1) ;i++) {
            if( strcmp(color_string,color_map[i]) == 0) {
                break;
            }
        }
        if( i == (OBJ_COLOR_MAX + 1) ) {
            color_index = 0;
        } else {
            color_index = i;
        }
    } else {
        return returnedValue;
    }

    /* Get parameters 2 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        x_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    /* Get parameters 3 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        y_coordinate = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_orb_test_send_RAE(color_index,x_coordinate,y_coordinate);

    return returnedValue;
}

static BaseType_t prvRadioRadioOrbDebugRAE4BitSwapOnOff(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint8_t on_off_switch = RADIO_TYPE_ON;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( strcmp(cCmd_string,"on") == 0 ) {
        on_off_switch = RADIO_TYPE_ON;
    } else if( strcmp(cCmd_string,"off") == 0 ) {
        on_off_switch = RADIO_TYPE_OFF;
    } else {
        return returnedValue;
    }
    s4527438_os_radio_orb_debug_RAE_4_bit_swap_on_off(on_off_switch);

    return returnedValue;
}
/***********************************************************************************************************/
static BaseType_t prvRadioLoadSorter(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t sorter_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        sorter_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_load_sorter_setting(sorter_index);

    return returnedValue;
}

static BaseType_t prvRadioLoadOrb(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString ) {

    long lParam_len;
    const char *cCmd_string;
    char *target_string = NULL;
    uint32_t orb_index = 0;
    // We always need to stop search further, even if the command is not correct
    BaseType_t returnedValue = pdFALSE;

    /* Get parameters 1 from command string */
    cCmd_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &lParam_len);

    if( cCmd_string != NULL ) {
        int i = 0;
        uint8_t numeric_string[4];

        memset(numeric_string,0x00,sizeof(numeric_string));
        for( i = 0; i < lParam_len ;i++) {
            if( cCmd_string[i] < '0'
                || cCmd_string[i] > '9' ) {
                return returnedValue;
            }
            numeric_string[i] = cCmd_string[i];
        }
        orb_index = atoi(numeric_string);
    } else {
        return returnedValue;
    }

    s4527438_os_radio_load_orb_setting(orb_index);

    return returnedValue;
}
