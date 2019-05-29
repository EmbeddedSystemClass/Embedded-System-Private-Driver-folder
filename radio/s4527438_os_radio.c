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

#include "s4527438_hal_radio.h"
#include "s4527438_os_atimer.h"
#include "s4527438_os_radio.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum{
    MESSAGE_TX_XYZ_TYPE,
    MESSAGE_RX_XYZ_REPLY_TYPE,
    MESSAGE_TX_JOIN_TYPE,
    MESSAGE_RX_JOIN_REPLY_TYPE,
    MESSAGE_TX_VACUUM_ACTION,
    MESSAGE_RX_VACUUM_REPLY_TYPE,
    MESSAGE_TX_KEEPALIVE_ACTION,
    MESSAGE_RX_KEEPALIVE_REPLY_ACTION,
    MESSAGE_RX_ORB_ACTION,

    MESSAGE_SET_CHAN,
    MESSAGE_GET_CHAN,
    MESSAGE_SET_TXADDR,
    MESSAGE_SET_RXADDR,
    MESSAGE_GET_TXADDR,
    MESSAGE_GET_RXADDR,
    MESSAGE_JOIN,

    MESSAGE_ORB_CP_CMD,
    MESSAGE_ORB_SHOW_CMD,
    MESSAGE_ORB_ON_OFF_CMD,
    MESSAGE_ORB_TEST_SEND_RAE_COLOR_AND_COORDINATE,
    MESSAGE_ORB_DEBUG_4_BIT_SWAP_ON_OFF_CMD,

    MESSAGE_LOAD_SORTER,
    MESSAGE_LOAD_ORB,
} Message_Type_Enum;

typedef enum{
    IDLE,
    NOT_REGISTERED,
    OPERATING_COMMAND,
} Sorter_Status_Type_Enum;

typedef struct{
    uint8_t     color;
    uint32_t    x_coordinate;
    uint32_t    y_coordinate;
}Obj_Color_Cp_Coordinate_Map_type;

typedef struct{
    Sorter_Status_Type_Enum status;
    uint32_t    x_coordinate;
    uint32_t    y_coordinate;
    uint32_t    z_coordinate;
    uint8_t     color;
    
    uint8_t     channel;
    uint8_t     tx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
    uint8_t     rx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];

    Obj_Color_Cp_Coordinate_Map_type color_map[OBJ_COLOR_MAX];
}Sorter_handler_Type;

typedef struct{
    uint8_t     channel;
    uint8_t     tx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
    uint8_t     rx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
    uint8_t     is_display_message;
    uint8_t     is_switch;

    uint8_t     is_enable_4_bit_swap;
    uint16_t    block_list[ORB_BLOCK_LIST_LEN];
    uint8_t     block_list_cur_index;
}ORB_handler_Type;

struct Message {    /* Message consists of sequence number and payload string */
    Message_Type_Enum   MessageType;
    Message_Vacuum_Action_Enum vacuum_action;
    uint32_t    x_coordinate;
    uint32_t    y_coordinate;
    uint32_t    z_coordinate;
    uint8_t     color;
    uint8_t     is_display_message;
    uint8_t     is_switch;

    uint8_t     select_index;
    uint8_t     channel;
    uint8_t     tx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
    uint8_t     rx_addr[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
};

typedef struct {
        uint16_t     ID;
        uint16_t x_coordinate;
        uint16_t y_coordinate;

        uint16_t width;
        uint16_t height;

        uint8_t color;
} RAE_Type;

/* Private define ------------------------------------------------------------*/
#define mainRADIO_TASK_PRIORITY               ( tskIDLE_PRIORITY + 4 )
#define mainRADIO_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

#define mainORBReceiver_TASK_PRIORITY               ( tskIDLE_PRIORITY + 2 )
#define mainORBReceiver_TASK_STACK_SIZE     ( configMINIMAL_STACK_SIZE * 16 )

#define QUEUE_BLOCK_TIME_AS_FSM_PERIOD_TICK 100

#define SELF_SOURCE_ADDR_STRING                        "45274389"

#define COM_BASE_TX_DESTINATION_ADDR_STRING                   "11223345"
#define COM_BASE_TX_CHANNEL                                   45	

#define SORTER_1_TX_DESTINATION_ADDR_STRING                   "80000055"
#define SORTER_1_TX_CHANNEL                                   55

#define SORTER_2_TX_DESTINATION_ADDR_STRING                   "80000057"
#define SORTER_2_TX_CHANNEL                                   57

#define SORTER_3_TX_DESTINATION_ADDR_STRING                   "80000058"
#define SORTER_3_TX_CHANNEL                                   58

#define SORTER_4_TX_DESTINATION_ADDR_STRING                   "80000059"
#define SORTER_4_TX_CHANNEL                                   59

#define ORB_1_RX_DESTINATION_ADDR_STRING                   "90000051"
#define ORB_1_RX_CHANNEL                                   51

#define ORB_2_RX_DESTINATION_ADDR_STRING                   "90000052"
#define ORB_2_RX_CHANNEL                                   52

#define ORB_3_RX_DESTINATION_ADDR_STRING                   "90000053"
#define ORB_3_RX_CHANNEL                                   53

#define ORB_4_RX_DESTINATION_ADDR_STRING                   "90000054"
#define ORB_4_RX_CHANNEL                                   54

#define	RADIO_HAL_TOTAL_PACKET_WIDTH		32

#define RADIO_RX_RETRY_COUNT                200
#define RADIO_RX_RETRY_COUNT_DELAY          20

#define QUEUE_LENGTH                        50

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTaskRadioOsHandle;
static TaskHandle_t xTaskORBReceiverOsHandle;
static QueueHandle_t s4527438QueueSorterPacketSend;
static SemaphoreHandle_t s4527438Semaphore_ORB_RX_Event = NULL;
static QueueSetHandle_t xQueueSet;

static Sorter_handler_Type sorter_handler;
static ORB_handler_Type orb_handler;
static uint8_t sorter_z_max_value = 99;
/* Private function prototypes -----------------------------------------------*/
static void RadioTask( void );
static void ORBReceiverTask( void );

void s4527438_os_radio_init(void) {

    s4527438QueueSorterPacketSend = xQueueCreate(QUEUE_LENGTH, sizeof(struct Message));       /* Create queue of length 10 Message items */

    /* Init sorter */
    sorter_handler.status = NOT_REGISTERED;
#ifdef COM_BASE_SETTING
    memcpy(sorter_handler.tx_addr,COM_BASE_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
    sorter_handler.channel = COM_BASE_TX_CHANNEL;

    memcpy(orb_handler.tx_addr,COM_BASE_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
    orb_handler.channel = COM_BASE_TX_CHANNEL;
#endif

#ifdef SORTER1_SETTING
    memcpy(sorter_handler.tx_addr,SORTER_1_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
    sorter_handler.channel = SORTER_1_TX_CHANNEL;
#endif

#ifdef SORTER2_SETTING
    memcpy(sorter_handler.tx_addr,SORTER_2_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
    sorter_handler.channel = SORTER_2_TX_CHANNEL;
#endif

#ifdef SORTER3_SETTING
    memcpy(sorter_handler.tx_addr,SORTER_3_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
    sorter_handler.channel = SORTER_3_TX_CHANNEL;
#endif

#ifdef SORTER4_SETTING
    memcpy(sorter_handler.tx_addr,SORTER_4_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
    sorter_handler.channel = SORTER_4_TX_CHANNEL;
#endif

#ifdef ORB1_SETTING
    memcpy(orb_handler.tx_addr,ORB_1_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
    orb_handler.channel = ORB_1_RX_CHANNEL;
#endif

#ifdef ORB2_SETTING
    memcpy(orb_handler.tx_addr,ORB_2_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
    orb_handler.channel = ORB_2_RX_CHANNEL;
#endif

#ifdef ORB3_SETTING
    memcpy(orb_handler.tx_addr,ORB_3_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
    orb_handler.channel = ORB_3_RX_CHANNEL;
#endif

#ifdef ORB4_SETTING
    memcpy(orb_handler.tx_addr,ORB_4_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
    memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
    orb_handler.channel = ORB_4_RX_CHANNEL;
#endif

    /* Init ORB */ 
    orb_handler.is_enable_4_bit_swap = RADIO_TYPE_ON;

    s4527438Semaphore_ORB_RX_Event = xSemaphoreCreateBinary();

    /* Create QueueSet */
    xQueueSet = xQueueCreateSet(QUEUE_LENGTH * 2 );    //Size of Queueset = Size of Queue (10) * 2 

    xQueueAddToSet(s4527438Semaphore_ORB_RX_Event, xQueueSet);
    xQueueAddToSet(s4527438QueueSorterPacketSend, xQueueSet);

    xTaskCreate( (void *) &ORBReceiverTask, (const signed char *) "ORBReceiverTask", mainORBReceiver_TASK_STACK_SIZE, NULL, mainORBReceiver_TASK_PRIORITY, &xTaskORBReceiverOsHandle );

    xTaskCreate( (void *) &RadioTask, (const signed char *) "RadioTask", mainRADIO_TASK_STACK_SIZE, NULL, mainRADIO_TASK_PRIORITY, &xTaskRadioOsHandle );
}
/***********************************************************************************************************/
void s4527438_os_radio_set_chan(uint32_t channel) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_SET_CHAN;
    SendMessage.channel = channel;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_get_chan(void) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_GET_CHAN;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_set_txaddr(uint8_t *addr) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_SET_TXADDR;
    memcpy(SendMessage.tx_addr,addr,sizeof(SendMessage.tx_addr));

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_set_rxaddr(uint8_t *addr) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_SET_RXADDR;
    memcpy(SendMessage.rx_addr,addr,sizeof(SendMessage.rx_addr));

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_get_txaddr(void) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_GET_TXADDR;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_get_rxaddr(void) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_GET_RXADDR;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

/***********************************************************************************************************/
void s4527438_os_radio_set_cp(uint8_t color,uint32_t x_coordinate, uint32_t y_coordinate) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_ORB_CP_CMD;
    SendMessage.color = color;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_orb_show_rx_message(uint8_t is_display_message) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_ORB_SHOW_CMD;
    SendMessage.is_display_message = is_display_message;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_orb_on_off(uint8_t is_switch) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_ORB_ON_OFF_CMD;
    SendMessage.is_switch = is_switch;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_orb_test_send_RAE(uint8_t ID,uint8_t color,uint32_t x_coordinate, uint32_t y_coordinate) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_ORB_TEST_SEND_RAE_COLOR_AND_COORDINATE;
    SendMessage.color = color;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;
    SendMessage.select_index = ID;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_orb_debug_RAE_4_bit_swap_on_off(uint8_t is_switch) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_ORB_DEBUG_4_BIT_SWAP_ON_OFF_CMD;
    SendMessage.is_switch = is_switch;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}
/***********************************************************************************************************/
void s4527438_os_radio_send_join_packet(void) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_TX_JOIN_TYPE;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }

    SendMessage.MessageType = MESSAGE_RX_JOIN_REPLY_TYPE;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_send_xyz_packet(uint32_t x_coordinate, uint32_t y_coordinate, uint32_t z_coordinate) {
    struct Message SendMessage;

    if( sorter_handler.status == NOT_REGISTERED ) {
        s4527438_os_radio_send_join_packet();
    }

    SendMessage.MessageType = MESSAGE_TX_XYZ_TYPE;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;
    SendMessage.z_coordinate = z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }

    SendMessage.MessageType = MESSAGE_RX_XYZ_REPLY_TYPE;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;
    SendMessage.z_coordinate = z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_move_only_xy(uint32_t x_coordinate, uint32_t y_coordinate) {
    struct Message SendMessage;

    if( sorter_handler.status == NOT_REGISTERED ) {
        s4527438_os_radio_send_join_packet();
    }

    SendMessage.MessageType = MESSAGE_TX_XYZ_TYPE;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;
    SendMessage.z_coordinate = sorter_handler.z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }

    SendMessage.MessageType = MESSAGE_RX_XYZ_REPLY_TYPE;
    SendMessage.x_coordinate = x_coordinate;
    SendMessage.y_coordinate = y_coordinate;
    SendMessage.z_coordinate = sorter_handler.z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_move_only_z(uint32_t z_coordinate) {
    struct Message SendMessage;

    if( sorter_handler.status == NOT_REGISTERED ) {
        s4527438_os_radio_send_join_packet();
    }

    SendMessage.MessageType = MESSAGE_TX_XYZ_TYPE;
    SendMessage.x_coordinate = sorter_handler.x_coordinate;
    SendMessage.y_coordinate = sorter_handler.y_coordinate;
    SendMessage.z_coordinate = z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }

    SendMessage.MessageType = MESSAGE_RX_XYZ_REPLY_TYPE;
    SendMessage.x_coordinate = sorter_handler.x_coordinate;
    SendMessage.y_coordinate = sorter_handler.y_coordinate;
    SendMessage.z_coordinate = z_coordinate;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_send_vacuum_packet(Message_Vacuum_Action_Enum vacuum_action) {
    struct Message SendMessage;

    if( sorter_handler.status == NOT_REGISTERED ) {
        s4527438_os_radio_send_join_packet();
    }

    SendMessage.MessageType = MESSAGE_TX_VACUUM_ACTION;
    SendMessage.vacuum_action = vacuum_action;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }

    SendMessage.MessageType = MESSAGE_RX_VACUUM_REPLY_TYPE;
    SendMessage.vacuum_action = vacuum_action;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_head_debug_set_max_lower_val(uint8_t headMaxVal) {
    sorter_z_max_value = headMaxVal;
}
/***********************************************************************************************************/
void s4527438_os_radio_load_sorter_setting(uint32_t sorter_index) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_LOAD_SORTER;
    SendMessage.select_index = sorter_index;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

void s4527438_os_radio_load_orb_setting(uint8_t orb_index) {
    struct Message SendMessage;

    SendMessage.MessageType = MESSAGE_LOAD_ORB;
    SendMessage.select_index = orb_index;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
}

static void radio_orb_send_rx_event(void) {
#if 0
    struct Message SendMessage;

    if( sorter_handler.status == NOT_REGISTERED ) {
        return;
    }

    SendMessage.MessageType = MESSAGE_RX_ORB_ACTION;

    if (s4527438QueueSorterPacketSend != NULL) { /* Check if queue exists */
        xQueueSend(s4527438QueueSorterPacketSend, ( void * ) &SendMessage, ( portTickType ) 0 );
    }
#endif
    if( sorter_handler.status == NOT_REGISTERED ) {
        return;
    }

    if (s4527438Semaphore_ORB_RX_Event != NULL) { /* Check if semaphore exists */
        xSemaphoreGive(s4527438Semaphore_ORB_RX_Event);
    }
}

static void configure_sorter_tx_setting(void) {
	s4527438_hal_radio_setchan(sorter_handler.channel);
	s4527438_hal_radio_settxaddress(sorter_handler.tx_addr);
	s4527438_hal_radio_setrxaddress(sorter_handler.rx_addr);
}

static void configure_sorter_rx_setting(void) {
	s4527438_hal_radio_setchan(sorter_handler.channel);
	s4527438_hal_radio_settxaddress(sorter_handler.tx_addr);
	s4527438_hal_radio_setrxaddress(sorter_handler.rx_addr);
}

static void configure_orb_rx_setting(void) {
	s4527438_hal_radio_setchan(orb_handler.channel);
	s4527438_hal_radio_settxaddress(orb_handler.tx_addr);
    /* NOTE : tx and rx are the same as tx for broadcast address */
	s4527438_hal_radio_setrxaddress(orb_handler.tx_addr);
}

#define HIGH_4_BIT  0xF0
#define LOW_4_BIT   0x0F

static uint8_t swap_4_bit(uint8_t *input_byte){
    uint8_t result_1_byte = 0;

    if( orb_handler.is_enable_4_bit_swap == RADIO_TYPE_OFF ) {
        return;
    }

    result_1_byte = ((*input_byte) & LOW_4_BIT);
    result_1_byte = (result_1_byte << 4);
    result_1_byte |= (((*input_byte) & HIGH_4_BIT) >> 4);
    return result_1_byte;
}

static void radio_RAE_parser(uint8_t *rx_buffer,RAE_Type *parsed_RAE) {
    uint8_t *payload = &(rx_buffer[RADIO_HAL_HEADER_WIDTH]);
    uint8_t *current_index = 0 , result_1_byte;
    uint16_t swap_1_byte = 0;

    current_index = payload;

    // 2 byte : Marker ID
    result_1_byte = swap_4_bit(&(current_index[0]));
    swap_1_byte = 0;
    swap_1_byte = (uint16_t)result_1_byte;
    swap_1_byte = (swap_1_byte << 8); 

    result_1_byte = swap_4_bit(&(current_index[1]));
    swap_1_byte |= (uint16_t)result_1_byte;

    parsed_RAE->ID = swap_1_byte;
    current_index += 2;

    // 2 byte : x coordinate
    result_1_byte = swap_4_bit(&(current_index[0]));
    swap_1_byte = 0;
    swap_1_byte = (uint16_t)result_1_byte;
    swap_1_byte = (swap_1_byte << 8); 

    result_1_byte = swap_4_bit(&(current_index[1]));
    swap_1_byte |= (uint16_t)result_1_byte;

    parsed_RAE->x_coordinate = swap_1_byte;
    current_index += 2;

    // 2 byte : y coordinate
    result_1_byte = swap_4_bit(&(current_index[0]));
    swap_1_byte = 0;
    swap_1_byte = (uint16_t)result_1_byte;
    swap_1_byte = (swap_1_byte << 8); 

    result_1_byte = swap_4_bit(&(current_index[1]));
    swap_1_byte |= (uint16_t)result_1_byte;

    parsed_RAE->y_coordinate = swap_1_byte;
    current_index += 2;

    // 2 byte : width
    result_1_byte = swap_4_bit(&(current_index[0]));
    swap_1_byte = 0;
    swap_1_byte = (uint16_t)result_1_byte;
    swap_1_byte = (swap_1_byte << 8); 

    result_1_byte = swap_4_bit(&(current_index[1]));
    swap_1_byte |= (uint16_t)result_1_byte;

    parsed_RAE->width = swap_1_byte;
    current_index += 2;

    // 2 byte : height
    result_1_byte = swap_4_bit(&(current_index[0]));
    swap_1_byte = 0;
    swap_1_byte = (uint16_t)result_1_byte;
    swap_1_byte = (swap_1_byte << 8); 

    result_1_byte = swap_4_bit(&(current_index[1]));
    swap_1_byte |= (uint16_t)result_1_byte;

    parsed_RAE->height = swap_1_byte;
    current_index += 2;

    // 1 byte : color
    result_1_byte = swap_4_bit(current_index);
    parsed_RAE->color = swap_1_byte;
}

static void process_hal_fsm_after_packets_sent(void){
    while(1){
        s4527438_hal_radio_fsmprocessing();
        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_TX_STATE ){
            break;
        }
        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
    }
    while(1){
        s4527438_hal_radio_fsmprocessing();
        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
            break;
        }
        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
    }
    s4527438_hal_radio_setfsmrx();
    while(1){
        s4527438_hal_radio_fsmprocessing();
        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_WAITING_STATE ){
            break;
        }
        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
    }
    while(1){
        s4527438_hal_radio_fsmprocessing();
        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
            break;
        }
        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
    }
}

static uint8_t orb_is_ID_in_block_list(uint16_t obj_id){
    uint8_t i = 0;

    for(i = 0; i < ORB_BLOCK_LIST_LEN ;i++){
        if( obj_id == orb_handler.block_list[i] ) {
            return RADIO_TYPE_TRUE;
        }
    }
    return RADIO_TYPE_FALSE;
}

static uint8_t orb_put_ID_in_block_list(uint16_t obj_id){
    if( orb_handler.block_list_cur_index < ORB_BLOCK_LIST_LEN ) {
        orb_handler.block_list[orb_handler.block_list_cur_index] = obj_id;
        (orb_handler.block_list_cur_index)++;
    }
}

static void orb_reset_block_list(void){
    uint8_t i = 0;

	debug_printf("reset block list\r\n");
    orb_handler.block_list_cur_index = 0;
    for(i = 0; i < ORB_BLOCK_LIST_LEN ;i++){
        orb_handler.block_list[i] = 0;
    }
}

static void RadioTask( void ) {

    struct Message RecvMessage;
    QueueSetMemberHandle_t xActivatedMember;
    int currentAngle = 0;
    uint8_t tx_string[12] = {0};
    int i = 0;

    /* Init radio hal */
    // Harware init
    s4527438_hal_radio_init();


    for (;;) {

        xActivatedMember = xQueueSelectFromSet(xQueueSet, QUEUE_BLOCK_TIME_AS_FSM_PERIOD_TICK);

        /* Which set member was selected?  Receives/takes can use a block time
        of zero as they are guaranteed to pass because xQueueSelectFromSet()
        would not have returned the handle unless something was available. */
        if (xActivatedMember == s4527438QueueSorterPacketSend) {

            /* Receive item */
            xQueueReceive( s4527438QueueSorterPacketSend, &RecvMessage, 0 );

            switch(RecvMessage.MessageType) {
                case MESSAGE_SET_CHAN:
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
	                s4527438_hal_radio_setchan(RecvMessage.channel);
                    break;
                case MESSAGE_GET_CHAN:
                    {
	                    uint8_t current_channel;
	                    current_channel = s4527438_hal_radio_getchan();
                        debug_printf("[Current channel]: <%d>\n\r", current_channel);
                    }
                    break;
                case MESSAGE_SET_TXADDR:
	                s4527438_hal_radio_settxaddress(RecvMessage.tx_addr);
                    break;
                case MESSAGE_SET_RXADDR:
	                s4527438_hal_radio_setrxaddress(RecvMessage.rx_addr);
                    break;
                case MESSAGE_GET_TXADDR:
                    {
	                    uint8_t addr_buffer[RADIO_HAL_TX_RX_ADDR_WIDTH];
	                    uint8_t addr_string_buffer[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
	                    uint8_t *cur_string;
                        uint8_t i = 0, j = 0;

	                    s4527438_hal_radio_gettxaddress(addr_buffer);
	                    debug_printf("[Current tx address]: 0x");
	                    for (j = 0 ,i = (RADIO_HAL_TX_RX_ADDR_WIDTH - 1); i >= 0 && j < RADIO_HAL_TX_RX_ADDR_STRING_WIDTH ; i--,j+=2) {
                            sprintf(&(addr_string_buffer[j]),"%02X",addr_buffer[i]);
	                    }
                        addr_string_buffer[j] = '\0';
	                    debug_printf("%s",addr_string_buffer);
	                    debug_printf("\r\n");
                    }
                    break;
                case MESSAGE_GET_RXADDR:
                    {
	                    uint8_t addr_buffer[RADIO_HAL_TX_RX_ADDR_WIDTH];
	                    uint8_t addr_string_buffer[RADIO_HAL_TX_RX_ADDR_STRING_WIDTH + 1];
	                    uint8_t *cur_string;
                        uint8_t i = 0, j = 0;

	                    s4527438_hal_radio_getrxaddress(addr_buffer);
	                    debug_printf("[Current rx address]: 0x");
	                    for (j = 0 ,i = (RADIO_HAL_TX_RX_ADDR_WIDTH - 1); i >= 0 && j < RADIO_HAL_TX_RX_ADDR_STRING_WIDTH ; i--,j+=2) {
                            sprintf(&(addr_string_buffer[j]),"%02X",addr_buffer[i]);
	                    }
                        addr_string_buffer[j] = '\0';
	                    debug_printf("%s",addr_string_buffer);
	                    debug_printf("\r\n");
                    }
                    break;
                case MESSAGE_JOIN:
                    break;
                /***********************************************************************************************************/
                case MESSAGE_ORB_CP_CMD:
                    if( RecvMessage.color >= 0 && RecvMessage.color <= OBJ_COLOR_MAX ) {
                        Obj_Color_Cp_Coordinate_Map_type *color_map = NULL;
                        color_map = &(sorter_handler.color_map[RecvMessage.color]);

                        color_map->x_coordinate = RecvMessage.x_coordinate;
                        color_map->y_coordinate = RecvMessage.y_coordinate;
	                    debug_printf("[set cp]: color = <%d> , x_coordinate = <%d> , y_coordinate = <%d>\r\n",RecvMessage.color,sorter_handler.color_map[RecvMessage.color].x_coordinate,sorter_handler.color_map[RecvMessage.color].y_coordinate);
                    }
                    break;
                case MESSAGE_ORB_SHOW_CMD:
                    orb_handler.is_display_message = RecvMessage.is_display_message;
	                debug_printf("[orb show on/off]: <%d>\r\n",orb_handler.is_display_message);
                    break;
                case MESSAGE_ORB_ON_OFF_CMD:
                    orb_handler.is_switch = RecvMessage.is_switch;
	                debug_printf("[orb on/off]: <%d>\r\n",orb_handler.is_switch);
                    if( orb_handler.is_switch == RADIO_TYPE_OFF ) {
                        orb_reset_block_list();
                    }
                    break;
                case MESSAGE_ORB_TEST_SEND_RAE_COLOR_AND_COORDINATE:
                    {
                        uint8_t rae_id = RecvMessage.select_index;
                        uint8_t is_RAE_id_in_block_list = RADIO_TYPE_FALSE;
                        is_RAE_id_in_block_list = orb_is_ID_in_block_list(rae_id);
                        if( (RecvMessage.color >= 0 && RecvMessage.color <= OBJ_COLOR_MAX) &&
                            !is_RAE_id_in_block_list
                        ) {
                            s4527438_os_radio_send_xyz_packet(RecvMessage.x_coordinate, RecvMessage.y_coordinate, sorter_z_max_value);
                            s4527438_os_radio_send_vacuum_packet(VACUUM_ON);
                            s4527438_os_radio_send_xyz_packet(RecvMessage.x_coordinate, RecvMessage.y_coordinate, 0);
                            s4527438_os_radio_send_xyz_packet(sorter_handler.color_map[RecvMessage.color].x_coordinate, sorter_handler.color_map[RecvMessage.color].y_coordinate, sorter_z_max_value);
                            s4527438_os_radio_send_vacuum_packet(VACUUM_OFF);
                            s4527438_os_radio_send_xyz_packet(sorter_handler.color_map[RecvMessage.color].x_coordinate, sorter_handler.color_map[RecvMessage.color].y_coordinate, 0);

                            orb_put_ID_in_block_list(rae_id);
	                        debug_printf("[Enter send RAE]\r\n");
                        }
                    }
                    break;
                case MESSAGE_ORB_DEBUG_4_BIT_SWAP_ON_OFF_CMD:
                    orb_handler.is_enable_4_bit_swap = RecvMessage.is_switch;
	                debug_printf("[orb 4bit swap on/off]: <%d>\r\n",orb_handler.is_enable_4_bit_swap);
                    break;
                /***********************************************************************************************************/
                case MESSAGE_TX_XYZ_TYPE:
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    configure_sorter_tx_setting();
                    snprintf(tx_string,sizeof(tx_string),"XYZ%03d%03d%02d",RecvMessage.x_coordinate,RecvMessage.y_coordinate,RecvMessage.z_coordinate);
                    s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,tx_string);

                    process_hal_fsm_after_packets_sent();
                    break;
                case MESSAGE_RX_XYZ_REPLY_TYPE:
                    s4527438_hal_radio_setfsmrx();
                    {
                        uint8_t need_retrasmit = 0;
                        uint8_t rx_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
                        memset(rx_buffer,0x00,sizeof(rx_buffer));
                        for(i = 0;i < RADIO_RX_RETRY_COUNT;i++){
                            s4527438_hal_radio_fsmprocessing();
	                        if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_RECEIVED ) {
                                s4527438_hal_radio_getpacket(rx_buffer);
                                if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"A C K") == 0 ) {
                                    // Only Update Location when receive ACK
                                    sorter_handler.x_coordinate = RecvMessage.x_coordinate;
                                    sorter_handler.y_coordinate = RecvMessage.y_coordinate;
                                    sorter_handler.z_coordinate = RecvMessage.z_coordinate;
                                    debug_printf("Received: A C K\n\r");
                                    break;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"N A C K") == 0 ) {
                                    need_retrasmit = 1;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"E R R") == 0 ) {
                                    need_retrasmit = 1;
                                }
                            } else if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_DECODE_ERROR ) {
                                debug_printf("[H ERROR][2-bit error]\n\r");
                                need_retrasmit = 1;
                            }

                            if( need_retrasmit ) {
                                while(1){
                                    s4527438_hal_radio_fsmprocessing();
                                    if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                                        break;
                                    }
                                    vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                                }
                                configure_sorter_tx_setting();
                                snprintf(tx_string,sizeof(tx_string),"XYZ%03d%03d%02d",RecvMessage.x_coordinate,RecvMessage.y_coordinate,RecvMessage.z_coordinate);
                                s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,tx_string);

                                process_hal_fsm_after_packets_sent();
                            }
                            vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                        }
                    }
                    break;
                case MESSAGE_TX_JOIN_TYPE:
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    configure_sorter_tx_setting();
                    s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"JOIN");

                    process_hal_fsm_after_packets_sent();
                    break;
                case MESSAGE_RX_JOIN_REPLY_TYPE:
                    s4527438_hal_radio_setfsmrx();

                    {
                        uint8_t need_retrasmit = 0;
                        uint8_t rx_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
                        memset(rx_buffer,0x00,sizeof(rx_buffer));
                        for(i = 0;i < RADIO_RX_RETRY_COUNT;i++){
                            s4527438_hal_radio_fsmprocessing();
	                        if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_RECEIVED ) {
                                s4527438_hal_radio_getpacket(rx_buffer);
                                if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"A C K") == 0 ) {
                                    sorter_handler.status = IDLE;
                                    debug_printf("Received: A C K\n\r");
                                    break;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"N A C K") == 0 ) {
                                    need_retrasmit = 1;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"E R R") == 0 ) {
                                    need_retrasmit = 1;
                                }
                            } else if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_DECODE_ERROR ) {
                                debug_printf("[H ERROR][2-bit error]\n\r");
                                need_retrasmit = 1;
                            }

                            if( need_retrasmit ) {
                                while(1){
                                    s4527438_hal_radio_fsmprocessing();
                                    if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                                        break;
                                    }
                                    vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                                }
                                configure_sorter_tx_setting();
                                s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"JOIN");

                                process_hal_fsm_after_packets_sent();
                            }
                            vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                        }
                    }
                    break;
                case MESSAGE_TX_VACUUM_ACTION:
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    configure_sorter_tx_setting();
                    if(RecvMessage.vacuum_action == VACUUM_ON) {
                        s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"VON");
                    } else if( RecvMessage.vacuum_action == VACUUM_OFF ) {
                        s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"VOFF");
                    }
                    process_hal_fsm_after_packets_sent();
                    break;
                case MESSAGE_RX_VACUUM_REPLY_TYPE:
                    {
                        uint8_t need_retrasmit = 0;
                        uint8_t rx_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
                        memset(rx_buffer,0x00,sizeof(rx_buffer));
                        for(i = 0;i < RADIO_RX_RETRY_COUNT;i++){
                            s4527438_hal_radio_fsmprocessing();
	                        if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_RECEIVED ) {
                                s4527438_hal_radio_getpacket(rx_buffer);
                                if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"A C K") == 0 ) {
                                    debug_printf("Received: A C K\n\r");
                                    break;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"N A C K") == 0 ) {
                                    need_retrasmit = 1;
                                }else if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"E R R") == 0 ) {
                                    need_retrasmit = 1;
                                }
                            } else if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_DECODE_ERROR ) {
                                debug_printf("[H ERROR][2-bit error]\n\r");
                                need_retrasmit = 1;
                            }

                            if( need_retrasmit ) {
                                while(1){
                                    s4527438_hal_radio_fsmprocessing();
                                    if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                                        break;
                                    }
                                    vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                                }
                                configure_sorter_tx_setting();
                                if(RecvMessage.vacuum_action == VACUUM_ON) {
                                    s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"VON");
                                } else if( RecvMessage.vacuum_action == VACUUM_OFF ) {
                                    s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"VOFF");
                                }

                                process_hal_fsm_after_packets_sent();
                            }
                            vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                        }
                    }
                    break;
                case MESSAGE_TX_KEEPALIVE_ACTION:
                    configure_sorter_tx_setting();
                    s4527438_hal_radio_sendpacket(0,sorter_handler.tx_addr,"JOIN");
                    process_hal_fsm_after_packets_sent();
                    break;
                case MESSAGE_RX_KEEPALIVE_REPLY_ACTION:
                    s4527438_hal_radio_setfsmrx();
                    {
                        uint8_t rx_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
                        memset(rx_buffer,0x00,sizeof(rx_buffer));
                        for(i = 0;i < RADIO_RX_RETRY_COUNT;i++){
                            s4527438_hal_radio_fsmprocessing();
	                        if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_RECEIVED ) {
                                s4527438_hal_radio_getpacket(rx_buffer);
                                if( strcmp(&(rx_buffer[RADIO_HAL_HEADER_WIDTH]),"A C K") == 0 ) {
                                    if( sorter_handler.status == OPERATING_COMMAND ) {
                                        sorter_handler.status = IDLE;
                                    }
                                    break;
                                }
                            } else if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_DECODE_ERROR ) {
                                debug_printf("[H ERROR][2-bit error]\n\r");
                            }
                            vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                        }
                    }
                    break;
                /***********************************************************************************************************/
                case MESSAGE_LOAD_SORTER:
                    if( RecvMessage.select_index == 1) {
                        memcpy(sorter_handler.tx_addr,SORTER_1_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
                        sorter_handler.channel = SORTER_1_TX_CHANNEL;
                    } else if (RecvMessage.select_index == 2) {
                        memcpy(sorter_handler.tx_addr,SORTER_2_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
                        sorter_handler.channel = SORTER_2_TX_CHANNEL;
                    } else if (RecvMessage.select_index == 3) {
                        memcpy(sorter_handler.tx_addr,SORTER_3_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
                        sorter_handler.channel = SORTER_3_TX_CHANNEL;
                    } else if (RecvMessage.select_index == 4) {
                        memcpy(sorter_handler.tx_addr,SORTER_4_TX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(sorter_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(sorter_handler.rx_addr));
                        sorter_handler.channel = SORTER_4_TX_CHANNEL;
                    }
                    break;
                case MESSAGE_LOAD_ORB:
                    if( RecvMessage.select_index == 1) {
                        memcpy(orb_handler.tx_addr,ORB_1_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
                        orb_handler.channel = ORB_1_RX_CHANNEL;
                    } else if (RecvMessage.select_index == 2) {
                        memcpy(orb_handler.tx_addr,ORB_2_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
                        orb_handler.channel = ORB_2_RX_CHANNEL;
                    } else if (RecvMessage.select_index == 3) {
                        memcpy(orb_handler.tx_addr,ORB_3_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
                        orb_handler.channel = ORB_3_RX_CHANNEL;
                    } else if (RecvMessage.select_index == 4) {
                        memcpy(orb_handler.tx_addr,ORB_4_RX_DESTINATION_ADDR_STRING,sizeof(sorter_handler.tx_addr));
                        memcpy(orb_handler.rx_addr,SELF_SOURCE_ADDR_STRING,sizeof(orb_handler.rx_addr));
                        orb_handler.channel = ORB_4_RX_CHANNEL;
                    }
                    break;
            }

#if 0
#ifdef DEBUG
            portENTER_CRITICAL();
            debug_printf("Received: angle = %d\n\r", RecvMessage.angle);
            portEXIT_CRITICAL();
#endif
#endif

        } else if (xActivatedMember == s4527438Semaphore_ORB_RX_Event) {  
            RAE_Type parsed_RAE;

            /* We were able to obtain the semaphore and can now access the shared resource. */
            xSemaphoreTake(s4527438Semaphore_ORB_RX_Event, 0 );

            {
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    configure_orb_rx_setting();
                    s4527438_hal_radio_setfsmrx();

                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_WAITING_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    while(1){
                        s4527438_hal_radio_fsmprocessing();
                        if(s4527438_hal_radio_get_current_fsm_state() == S4527438_RADIO_IDLE_STATE ){
                            break;
                        }
                        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                    }
                    {
                        uint8_t rx_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
                        uint8_t is_RAE_id_in_block_list = RADIO_TYPE_FALSE;
                        memset(rx_buffer,0x00,sizeof(rx_buffer));
                        for(i = 0;i < RADIO_RX_RETRY_COUNT;i++){
                            s4527438_hal_radio_fsmprocessing();
	                        if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_RECEIVED ) {
                                s4527438_hal_radio_getpacket(rx_buffer);
                                radio_RAE_parser(rx_buffer,&parsed_RAE);
                                is_RAE_id_in_block_list = orb_is_ID_in_block_list(parsed_RAE.ID);

                                if( orb_handler.is_switch == RADIO_TYPE_ON ) {
                                    if( (parsed_RAE.color >= 0 && parsed_RAE.color <= OBJ_COLOR_MAX) &&
                                        !is_RAE_id_in_block_list
                                         ) {
                                        s4527438_os_radio_send_xyz_packet(parsed_RAE.x_coordinate, parsed_RAE.y_coordinate, sorter_z_max_value);
                                        s4527438_os_radio_send_vacuum_packet(VACUUM_ON);
                                        s4527438_os_radio_send_xyz_packet(parsed_RAE.x_coordinate, parsed_RAE.y_coordinate, 0);
                                        s4527438_os_radio_send_xyz_packet(sorter_handler.color_map[parsed_RAE.color].x_coordinate, sorter_handler.color_map[parsed_RAE.color].y_coordinate, sorter_z_max_value);
                                        s4527438_os_radio_send_vacuum_packet(VACUUM_OFF);
                                        s4527438_os_radio_send_xyz_packet(sorter_handler.color_map[parsed_RAE.color].x_coordinate, sorter_handler.color_map[parsed_RAE.color].y_coordinate, 0);
                                        orb_put_ID_in_block_list(parsed_RAE.ID);
                                    }
                                }

                                if( orb_handler.is_display_message == RADIO_TYPE_ON ) {
                                    char    *color_map[OBJ_COLOR_MAX + 1] = {"NO COLOR","RED","GREEN","BLUE","YELLO","ORANGE"};
                                    char    color_string[9];
                                    uint32_t    cur_time_ms = s4527438_os_atimer_read_ms();

                                    if( parsed_RAE.color >= 0 && parsed_RAE.color <= OBJ_COLOR_MAX ) {
                                        strcpy(color_string,color_map[parsed_RAE.color]);
                                    }

                                    debug_printf("[%d(ms)][color:%s][x_coordinate:%d][y_coordinate:%d] \r\n",   
                                                                                                    cur_time_ms,
                                                                                                    color_string,
                                                                                                    parsed_RAE.x_coordinate,
                                                                                                    parsed_RAE.y_coordinate
                                                                                                    );
                                }
#ifdef DEBUG
                                debug_printf("RAE ID : %d , X : %d , Y : %d , width : %d , height : %d , color : %d \r\n",parsed_RAE.ID,
                                                                                                                          parsed_RAE.x_coordinate,
                                                                                                                          parsed_RAE.y_coordinate,
                                                                                                                          parsed_RAE.width,
                                                                                                                          parsed_RAE.height,
                                                                                                                          parsed_RAE.color
                                                                                                                          );
#endif
                                break;
                            } else if( s4527438_hal_radio_getrxstatus() == RX_STATUS_PACKET_DECODE_ERROR ) {
                                debug_printf("[H ERROR][2-bit error]\n\r");
                            }
                            vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
                        }
                    }
            }
        }

        s4527438_hal_radio_fsmprocessing();

        /* Delay for 10ms */
        vTaskDelay(RADIO_RX_RETRY_COUNT_DELAY);
        
    }
}



static void ORBReceiverTask( void ) {
    for(;;){
        if( orb_handler.is_switch == RADIO_TYPE_ON ) {
            radio_orb_send_rx_event();
        }
        vTaskDelay(100);
    }
}
