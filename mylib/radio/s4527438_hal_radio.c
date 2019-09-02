/** 
 **************************************************************
 * @file mylib/s4527438_hal_radio.c
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib radio driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */
/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32f4xx_hal.h"
#include "debug_printf.h"
#include "s4527438_hal_radio.h"
#include "s4527438_lib_hamming.h"
#include "radio_fsm.h"
#include "nrf24l01plus.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/

typedef enum{
    TX_MODE,
    RX_MODE
}Transmission_Mode_TypeEnum;

typedef struct{
    int state_num;
    void (*state_handle_first_enter)(void);
    void (*state_handle_fsm_process)(void);
    void (*state_handle_before_exit)(void);
} Radio_State_Obj_TypeStruct;

/* Private define ------------------------------------------------------------*/
#define DEFAULT_RX_ADDR_STRING                      "45274389"
#define DEFAULT_TX_ADDR_STRING                      "11223354"
#define DEFAULT_TX_CHANNEL			                45	

#define RADIO_HAL_PAYLOAD_WIDTH             22

// TX packet format
#define RADIO_HAL_TX_BYTE_0_VALUE           0xA1 
#define RADIO_HAL_TX_BYTE_0_LEN             1

#define RADIO_HAL_TX_BYTE_1_LEN             4 // TX addr

#define RADIO_HAL_TX_BYTE_5_LEN             4 // RX addr

#define RADIO_HAL_TX_BYTE_9_VALUE           0x00 
#define RADIO_HAL_TX_BYTE_9_LEN             1

#define NO_ENCODE_WIDTH                     (RADIO_HAL_TX_BYTE_0_LEN + RADIO_HAL_TX_BYTE_1_LEN + RADIO_HAL_TX_BYTE_5_LEN + RADIO_HAL_TX_BYTE_9_LEN )
#define ENCODE_WIDTH                        (RADIO_HAL_TOTAL_PACKET_WIDTH - NO_ENCODE_WIDTH) 

#define RADIO_HAL_ONE_BYTE_ENCODED_OUTPUT_SIZE   2

#define TX_STATUS_BUFFER_WAITING_SEND   0
#define TX_STATUS_BUFFER_EMPTY          1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int halRadioRxstatus = RX_STATUS_NO_PACKET_RECEIVED; 
int halRadioTxstatus = TX_STATUS_BUFFER_WAITING_SEND; 
static int halRadioFsmcurrentstate = S4527438_RADIO_IDLE_STATE;
static int halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
static Radio_State_Obj_TypeStruct state_handle_obj[S4527438_RADIO_STATE_MAX_VALUE + 1];
static uint8_t send_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
static unsigned char rxBuffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
static Transmission_Mode_TypeEnum currentMode = RX_MODE;

/* Private function prototypes -----------------------------------------------*/
static void string_to_hex(char *input_string,unsigned char input_length,uint8_t *output_buffer,unsigned char output_length);

static void IDLE_STATE_state_handle_first_enter(void);
static void IDLE_STATE_state_handle_fsm_process(void);
static void IDLE_STATE_state_handle_before_exit(void);

static void RX_STATE_state_handle_first_enter(void);
static void RX_STATE_state_handle_fsm_process(void);
static void RX_STATE_state_handle_before_exit(void);

static void TX_STATE_state_handle_first_enter(void);
static void TX_STATE_state_handle_fsm_process(void);
static void TX_STATE_state_handle_before_exit(void);

static void WAITING_STATE_state_handle_first_enter(void);
static void WAITING_STATE_state_handle_fsm_process(void);
static void WAITING_STATE_state_handle_before_exit(void);

void s4527438_hal_radio_init(void) {
    Radio_State_Obj_TypeStruct * target_state = NULL;
    /* Init private state handler */
    /* IDLE_STATE */
    target_state = &(state_handle_obj[S4527438_RADIO_IDLE_STATE]);
    target_state->state_num = S4527438_RADIO_IDLE_STATE;
    target_state->state_handle_first_enter = IDLE_STATE_state_handle_first_enter;
    target_state->state_handle_fsm_process = IDLE_STATE_state_handle_fsm_process;
    target_state->state_handle_before_exit = IDLE_STATE_state_handle_before_exit;

    /* RX_STATE */
    target_state = &(state_handle_obj[S4527438_RADIO_RX_STATE]);
    target_state->state_num = S4527438_RADIO_RX_STATE;
    target_state->state_handle_first_enter = RX_STATE_state_handle_first_enter;
    target_state->state_handle_fsm_process = RX_STATE_state_handle_fsm_process;
    target_state->state_handle_before_exit = RX_STATE_state_handle_before_exit;

    /* TX_STATE */
    target_state = &(state_handle_obj[S4527438_RADIO_TX_STATE]);
    target_state->state_num = S4527438_RADIO_TX_STATE;
    target_state->state_handle_first_enter = TX_STATE_state_handle_first_enter;
    target_state->state_handle_fsm_process = TX_STATE_state_handle_fsm_process;
    target_state->state_handle_before_exit = TX_STATE_state_handle_before_exit;

    /* WAITING_STATE */
    target_state = &(state_handle_obj[S4527438_RADIO_WAITING_STATE]);
    target_state->state_num = S4527438_RADIO_WAITING_STATE;
    target_state->state_handle_first_enter = WAITING_STATE_state_handle_first_enter;
    target_state->state_handle_fsm_process = WAITING_STATE_state_handle_fsm_process;
    target_state->state_handle_before_exit = WAITING_STATE_state_handle_before_exit;

    halRadioFsmcurrentstate = S4527438_RADIO_IDLE_STATE;
    halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
    /* Initialise radio FSM */
    radio_fsm_init();

    halRadioRxstatus = RX_STATUS_NO_PACKET_RECEIVED; 
    halRadioTxstatus = TX_STATUS_BUFFER_EMPTY; 

    /* set radio FSM state to IDLE */
    radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
}

void s4527438_hal_radio_fsmprocessing(){
    Radio_State_Obj_TypeStruct * target_state = NULL;

    if( halRadioFsmNextstate != halRadioFsmcurrentstate ) {
        /* Call previous exit handle */
        target_state = &(state_handle_obj[halRadioFsmcurrentstate]);
        (*(target_state->state_handle_before_exit))();

        /* Call first enter handle */
        target_state = &(state_handle_obj[halRadioFsmNextstate]);
        (*(target_state->state_handle_first_enter))();

        halRadioFsmcurrentstate = halRadioFsmNextstate;
    }
    target_state = &(state_handle_obj[halRadioFsmcurrentstate]);
    (*(target_state->state_handle_fsm_process))();
}

void s4527438_hal_radio_setchan(unsigned char
chan) {
    nrf24l01plus_wr(NRF24L01P_RF_CH, chan);
}

void s4527438_hal_radio_settxaddress(unsigned
char *addr) {
    uint8_t addr_hex[4] = {0};
    int i = 0;
    string_to_hex(addr,strlen(addr),addr_hex,sizeof(addr_hex));
    nrf24l01plus_wb(NRF24L01P_WRITE_REG | NRF24L01P_TX_ADDR, addr_hex, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

void s4527438_hal_radio_setrxaddress(unsigned
char *addr) {
    uint8_t addr_hex[4] = {0};
    int i = 0;
    string_to_hex(addr,strlen(addr),addr_hex,sizeof(addr_hex));
    nrf24l01plus_wb(NRF24L01P_WRITE_REG | NRF24L01P_RX_ADDR_P0, addr_hex, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

unsigned char s4527438_hal_radio_getchan() {
    unsigned char current_channel;
    radio_fsm_register_read(NRF24L01P_RF_CH, &current_channel);
    return current_channel;
}

void s4527438_hal_radio_gettxaddress(unsigned
char *addr) {
    if( addr == NULL ) {
        return;
    }
    nrf24l01plus_rb(NRF24L01P_TX_ADDR, addr, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

void s4527438_hal_radio_getrxaddress(unsigned
char *addr) {
    if( addr == NULL ) {
        return;
    }
    nrf24l01plus_rb(NRF24L01P_RX_ADDR_P0, addr, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

static uint8_t convert_2_char_to_1_hex_byte(unsigned char *input_2_char) {
    uint8_t result = 0;
    unsigned char target_char = '0';
    unsigned char i = 0;

    i = 0;
    while(1) {
        if( i >= 2 ) {
            break;
        }
        result *= 16;
        target_char = (*input_2_char);
        if( target_char >= '0' && target_char <= '9' ) {
            result += (target_char - '0');
        } else {
            if( target_char >= 'A' && target_char <= 'F' ) {
                result += (target_char - 'A') + 10;
            } else if( target_char >= 'a' && target_char <= 'f' ) {
                result += (target_char - 'a') + 10;
            }
        }
        input_2_char++;
        i++;
    }
    return result;
}

static void string_to_hex(char *input_string,unsigned char input_length,uint8_t *output_buffer,unsigned char output_length) {
    unsigned char j = 0;
#if  1
    char *input_string_end = input_string;

    input_string = input_string + input_length - 2;

    if( strcmp(input_string_end,"0x") == 0 ) {
        input_string_end += 2;
    }

    while(1) {
        if( (input_string - input_string_end) < 0 ) {
            break;
        }
        if( j >= output_length ) {
            break;
        }
        output_buffer[j] = convert_2_char_to_1_hex_byte(input_string);
        input_string -= 2;
        j++;
    }
#endif
#if 0
    char *input_string_end = NULL;

    input_string_end = input_string + input_length - 2;

    if( strcmp(input_string,"0x") == 0 ) {
        input_string += 2;
    }

    while(1) {
        if( (input_string_end - input_string) < 0 ) {
            break;
        }
        if( j >= output_length ) {
            break;
        }
        output_buffer[j] = convert_2_char_to_1_hex_byte(input_string);
        input_string += 2;
        j++;
    }
#endif
}

void s4527438_hal_radio_sendpacket(char
chan, unsigned char *addr, unsigned
char *txpacket) {
    uint8_t *current_packet_position = NULL;
    if( halRadioTxstatus == TX_STATUS_BUFFER_WAITING_SEND ) {
        return;
    }
    /*
        DEFAULT_RX_ADDR_STRING :45274389
        
    */
    uint8_t addr_hex[4] = {0};

    memset(send_buffer,0x00,sizeof(send_buffer));
    current_packet_position = send_buffer;

    // 1 byte
    memset(current_packet_position,RADIO_HAL_TX_BYTE_0_VALUE,RADIO_HAL_TX_BYTE_0_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_0_LEN;

    // 4 bytes tx address (Destination Address)
    if( addr == NULL ) {
        string_to_hex(DEFAULT_TX_ADDR_STRING,strlen(DEFAULT_TX_ADDR_STRING),addr_hex,sizeof(addr_hex));
    } else {
        string_to_hex(addr,strlen(addr),addr_hex,sizeof(addr_hex));
    }
    memcpy(current_packet_position,addr_hex,RADIO_HAL_TX_BYTE_1_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_1_LEN;

    memset(addr_hex,0x00,sizeof(addr_hex));
    // 4 bytes rx address (Source Address)
    string_to_hex(DEFAULT_RX_ADDR_STRING,strlen(DEFAULT_RX_ADDR_STRING),addr_hex,sizeof(addr_hex));
    memcpy(current_packet_position,addr_hex,RADIO_HAL_TX_BYTE_5_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_5_LEN;

    // 1 byte padding
    memset(current_packet_position,RADIO_HAL_TX_BYTE_9_VALUE,RADIO_HAL_TX_BYTE_9_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_9_LEN;

    // 22 byte payload
    {
#if 0
        unsigned char j = 0;
        char *input_string_end = txpacket;
        unsigned char input_length = strlen(txpacket);
        unsigned char output_length = RADIO_HAL_PAYLOAD_WIDTH;
        char *input_string;
        unsigned char encoded_buffer[2];

        input_string = input_string_end + input_length - 1;

        while(1) {
            if( (input_string - input_string_end) < 0 ) {
                break;
            }
            if( j >= output_length ) {
                break;
            }
            s4527438_lib_hamming_byte_encoder((*input_string),encoded_buffer);

            memcpy(current_packet_position,encoded_buffer,RADIO_HAL_ONE_BYTE_ENCODED_OUTPUT_SIZE);
            current_packet_position += RADIO_HAL_ONE_BYTE_ENCODED_OUTPUT_SIZE;

            input_string--;
            j++;
        }
#endif
#if 1
        unsigned char j = 0;
        char *input_string_end;
        unsigned char input_length = strlen(txpacket);
        unsigned char output_length = RADIO_HAL_PAYLOAD_WIDTH;
        char *input_string  = txpacket ;
        unsigned char encoded_buffer[2];

        input_string_end = input_string + input_length - 1;

        while(1) {
            if( (input_string_end - input_string) < 0 ) {
                break;
            }
            if( j >= output_length ) {
                break;
            }
            s4527438_lib_hamming_byte_encoder((*input_string),encoded_buffer);

            memcpy(current_packet_position,encoded_buffer,RADIO_HAL_ONE_BYTE_ENCODED_OUTPUT_SIZE);
            current_packet_position += RADIO_HAL_ONE_BYTE_ENCODED_OUTPUT_SIZE;

            input_string++;
            j++;
        }
#endif
    }

    halRadioTxstatus = TX_STATUS_BUFFER_WAITING_SEND;
    currentMode = TX_MODE;
    return;
}

void s4527438_hal_radio_setfsmrx() {
    currentMode = RX_MODE;
}

int s4527438_hal_radio_getrxstatus() {
    return halRadioRxstatus;
}

void s4527438_hal_radio_getpacket(unsigned char *rxpacket) {
    memcpy(rxpacket,rxBuffer,RADIO_HAL_TOTAL_PACKET_WIDTH);
    halRadioRxstatus = RX_STATUS_PACKET_COPIED_BY_USER;
}

int s4527438_hal_radio_get_current_fsm_state(void) {
    return halRadioFsmcurrentstate;
}

static void IDLE_STATE_state_handle_first_enter(void) {
    if( halRadioRxstatus == RX_STATUS_PACKET_COPIED_BY_USER ) {
        halRadioRxstatus = RX_STATUS_NO_PACKET_RECEIVED;
        memset(rxBuffer,0x00,RADIO_HAL_TOTAL_PACKET_WIDTH);
    }
    radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
}
static void IDLE_STATE_state_handle_fsm_process(void) {
    // Get current channel, if radio FSM is in IDLE state
    if (radio_fsm_getstate() == RADIO_FSM_IDLE_STATE) {
        if( currentMode == RX_MODE ) {
            halRadioFsmNextstate = S4527438_RADIO_RX_STATE;
        } else {
            if( halRadioTxstatus == TX_STATUS_BUFFER_WAITING_SEND ) {
                halRadioFsmNextstate = S4527438_RADIO_TX_STATE;
            }
        }

    } else {

        // if error occurs set state back to IDLE state
        debug_printf("ERROR: Radio FSM not in IDLE state\r\n");
        radio_fsm_setstate(RADIO_FSM_IDLE_STATE);

    }   
}
static void IDLE_STATE_state_handle_before_exit(void) {
}

static void RX_STATE_state_handle_first_enter(void) {
}
static void RX_STATE_state_handle_fsm_process(void) {
    if ((radio_fsm_getstate() != RADIO_FSM_IDLE_STATE)
        && (radio_fsm_getstate() != RADIO_FSM_WAIT_STATE)) {
        halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
        debug_printf("ERROR: Radio FSM no in IDLE state\r\n");
        return;
    }

    if (radio_fsm_setstate(RADIO_FSM_RX_STATE)
            == RADIO_FSM_INVALIDSTATE) {
        debug_printf("ERROR: Cannot set radio FSM to RX state\r\n");
        HAL_Delay(100);
        return;
    }
    halRadioFsmNextstate = S4527438_RADIO_WAITING_STATE;
}
static void RX_STATE_state_handle_before_exit(void) {
}

static void TX_STATE_state_handle_first_enter(void) {
}

static void TX_STATE_state_handle_fsm_process(void) {
    if (radio_fsm_getstate() != RADIO_FSM_IDLE_STATE){
        halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
        return;
    }
    if (radio_fsm_setstate(RADIO_FSM_TX_STATE) == RADIO_FSM_INVALIDSTATE) {
        debug_printf("ERROR: Cannot set Radio FSM TX state\n\r");
        HAL_Delay(100);
        return;
    }

    debug_printf("sending...\n\r");

    /* Send packet - radio FSM will automatically go to IDLE state, after write completes. */
    radio_fsm_write(send_buffer);

    halRadioTxstatus = TX_STATUS_BUFFER_EMPTY;
    memset(send_buffer,0x00,sizeof(send_buffer));
    halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
}

static void TX_STATE_state_handle_before_exit(void) {
}

static void WAITING_STATE_state_handle_first_enter(void) {
}

static void WAITING_STATE_state_handle_fsm_process(void) {
    int i = 0,j = 0;
    if (radio_fsm_getstate() != RADIO_FSM_WAIT_STATE) {
        return;
    }
    // Check for received packet and display
    if (radio_fsm_read(rxBuffer) == RADIO_FSM_DONE) {
        uint8_t decoded_buffer[ENCODE_WIDTH];
        uint8_t decode_result = 0;

#ifdef DEBUG
        debug_printf("Received: ");
        for (i = 0; i < NO_ENCODE_WIDTH; i++) {
            debug_printf("%02x ", rxBuffer[i]);
        }
#endif
        i = NO_ENCODE_WIDTH;
        memset(decoded_buffer,0x00,sizeof(decoded_buffer));
        for (j = 0; i < RADIO_HAL_TOTAL_PACKET_WIDTH; i+=2,j++) {
            decode_result = s4527438_lib_hamming_byte_decoder(&(rxBuffer[i]),&(decoded_buffer[j]));
            if( decode_result ) {
                halRadioRxstatus = RX_STATUS_PACKET_DECODE_ERROR;
                return;
            }
#ifdef DEBUG
            debug_printf("%02x(%c)", decoded_buffer[j],decoded_buffer[j]);
#endif
        }
        memcpy(&(rxBuffer[NO_ENCODE_WIDTH]),decoded_buffer,ENCODE_WIDTH);
        // After decoded
        
#ifdef DEBUG
        debug_printf("\r\n");
#endif
        halRadioRxstatus = RX_STATUS_PACKET_RECEIVED;
    }

    halRadioFsmNextstate = S4527438_RADIO_IDLE_STATE;
}

static void WAITING_STATE_state_handle_before_exit(void) {
}

