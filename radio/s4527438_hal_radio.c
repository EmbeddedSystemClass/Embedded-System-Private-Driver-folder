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
#include "radio_fsm.h"
#include "nrf24l01plus.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define RX_ADDR_STRING_DEFINE               45274389

#define STRINGIFY_HELPER(A) #A
#define STRINGIFY(...)  STRINGIFY_HELPER(__VA_ARGS__)
#define RX_ADDR_STRING STRINGIFY(RX_ADDR_STRING_DEFINE) 

#define HEX2PER_BYTE(c)     (*(c) >= 'A' ? (*(c) - 'A')+10 : (*(c)-'0'))
#define HEX2BYTE(c)         (HEX2PER_BYTE(c)*16 + HEX2PER_BYTE(c+1))

#define RADIO_HAL_TOTAL_PACKET_WIDTH        32
#define RADIO_HAL_TX_RX_ADDR_WIDTH          4
#define RADIO_HAL_PAYLOAD_WIDTH             22

// TX packet format
#define RADIO_HAL_TX_BYTE_0_VALUE           0xA1 
#define RADIO_HAL_TX_BYTE_0_LEN             1

#define RADIO_HAL_TX_BYTE_1_LEN             4 // TX addr

#define RADIO_HAL_TX_BYTE_5_LEN             4 // RX addr

#define RADIO_HAL_TX_BYTE_9_VALUE           0x00 
#define RADIO_HAL_TX_BYTE_9_LEN             1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static int halRadioFsmcurrentstate = S4527438_RADIO_IDLE_STATE;

/* Private function prototypes -----------------------------------------------*/
void s4527438_hal_radio_init(void) {
    /* Initialise radio FSM */
    radio_fsm_init();

    /* set radio FSM state to IDLE */
    radio_fsm_setstate(RADIO_FSM_IDLE_STATE);
}

void s4527438_hal_radio_fsmprocessing(){
}

void s4527438_hal_radio_setchan(unsigned char
chan) {
    nrf24l01plus_wr(NRF24L01P_RF_CH, chan);
}

void s4527438_hal_radio_settxaddress(unsigned
char *addr) {
    nrf24l01plus_wb(NRF24L01P_WRITE_REG | NRF24L01P_TX_ADDR, addr, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

void s4527438_hal_radio_setrxaddress(unsigned
char *addr) {
    nrf24l01plus_wb(NRF24L01P_WRITE_REG | NRF24L01P_RX_ADDR_P0, addr, RADIO_HAL_TX_RX_ADDR_WIDTH);
}

unsigned char s4527438_hal_radio_getchan() {
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

void s4527438_hal_radio_sendpacket(char
chan, unsigned char *addr, unsigned
char *txpacket) {
    uint8_t send_buffer[RADIO_HAL_TOTAL_PACKET_WIDTH];
    uint8_t *current_packet_position = NULL;
    uint8_t rx_addr_hex[] = {
        HEX2BYTE(RX_ADDR_STRING+6),
        HEX2BYTE(RX_ADDR_STRING+4),
        HEX2BYTE(RX_ADDR_STRING+2),
        HEX2BYTE(RX_ADDR_STRING+0),
    };

    memset(send_buffer,0x00,sizeof(send_buffer));
    current_packet_position = send_buffer;

    // 1 byte
    memset(current_packet_position,RADIO_HAL_TX_BYTE_0_VALUE,RADIO_HAL_TX_BYTE_0_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_0_LEN;

    // 4 bytes tx address TODO : transfer string to hex
    memcpy(current_packet_position,addr,RADIO_HAL_TX_BYTE_1_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_1_LEN;

    // 4 bytes rx address
    memcpy(current_packet_position,rx_addr_hex,RADIO_HAL_TX_BYTE_5_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_5_LEN;

    // 1 byte padding
    memset(current_packet_position,RADIO_HAL_TX_BYTE_9_VALUE,RADIO_HAL_TX_BYTE_9_LEN);
    current_packet_position += RADIO_HAL_TX_BYTE_9_LEN;

    // 22 byte payload TODO : Encode with hamming code

    nrf24l01plus_send(send_buffer);
    return;
}

void s4527438_hal_radio_setfsmrx() {
}

int s4527438_hal_radio_getrxstatus() {
}

void s4527438_hal_radio_getpacket(unsigned char *rxpacket) {
}


