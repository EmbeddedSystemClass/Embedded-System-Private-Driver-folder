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
#include "s4527438_hal_radio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	MAIN_LOOP_POLLING_DELAY	10 //100ms
#define RX_ADDR_STRING                      "45274389"
//#define TX_ADDR_STRING                      "11223354"
#define TX_ADDR_STRING                      "11223345"
//#define TX_CHANNEL				54
#define TX_CHANNEL			                45	
#define RADIO_HAL_TX_RX_ADDR_WIDTH          4
#define	RADIO_HAL_TOTAL_PACKET_WIDTH		32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Hardware_init(void);

/**
  * @brief  Main program - flashes onboard LEDs
  * @param  None
  * @retval None
  */
int main(void)  {
	int i;
	uint8_t current_channel;
	uint8_t addr_buffer[RADIO_HAL_TX_RX_ADDR_WIDTH];
	unsigned char rxBuffer[RADIO_HAL_TOTAL_PACKET_WIDTH];

	BRD_init();			//Initalise Board
    	BRD_LEDInit();
	Hardware_init();	//Initalise hardware modules
	
    char RxChar;
    while(1) {
        RxChar = debug_getc();
        if( RxChar == 'h' ) {
            break;
        }
    }

    unsigned char encoded_buffer[2];
    unsigned char A = 0x41;
    s4527438_lib_hamming_byte_encoder(A,encoded_buffer);
	debug_printf("A = %x %x \r\n", encoded_buffer[0],encoded_buffer[1]);

    unsigned char C = 0x43;
    s4527438_lib_hamming_byte_encoder(C,encoded_buffer);
	debug_printf("C = %x %x \r\n", encoded_buffer[0],encoded_buffer[1]);

    unsigned char K = 0x4b;
    s4527438_lib_hamming_byte_encoder(K,encoded_buffer);
	debug_printf("K = %x %x \r\n", encoded_buffer[0],encoded_buffer[1]);

    while(1) {
        RxChar = debug_getc();
        if( RxChar == 's' ) {
            break;
        }
    }

	s4527438_hal_radio_setchan(TX_CHANNEL);
	s4527438_hal_radio_settxaddress(TX_ADDR_STRING);
	s4527438_hal_radio_setrxaddress(RX_ADDR_STRING);

	current_channel = s4527438_hal_radio_getchan();
	debug_printf("Current channel %X\n\r", current_channel);

	s4527438_hal_radio_gettxaddress(addr_buffer);
	debug_printf("gettxaddress: ");
	for (i = 0; i < RADIO_HAL_TX_RX_ADDR_WIDTH; i++) {
    		debug_printf("%c(0x%x) ", addr_buffer[i],addr_buffer[i]);
	}
	debug_printf("\r\n");

	s4527438_hal_radio_getrxaddress(addr_buffer);
	debug_printf("getrxaddress: ");
	for (i = 0; i < RADIO_HAL_TX_RX_ADDR_WIDTH; i++) {
    		debug_printf("%c(%x) ", addr_buffer[i],addr_buffer[i]);
	}
	debug_printf("\r\n");


    while(1) {
        RxChar = debug_getc();
        if( RxChar == 'f' ) {
	        s4527438_hal_radio_fsmprocessing();
            continue;
        }
        if( RxChar == 'e' ) {
            break;
        }
    }
	s4527438_hal_radio_sendpacket(TX_CHANNEL, TX_ADDR_STRING, "ABCDEFGHIJK");
    while(1) {
        RxChar = debug_getc();
        if( RxChar == 'f' ) {
	        s4527438_hal_radio_fsmprocessing();
            continue;
        }
        if( RxChar == 'e' ) {
            break;
        }
    }

	s4527438_hal_radio_setfsmrx();

    while(1) {
        RxChar = debug_getc();
        if( RxChar == 'c' ) {
            break;
        }
    }

    while (1) {

	    if( s4527438_hal_radio_getrxstatus() ) {
		    s4527438_hal_radio_getpacket(rxBuffer);
		    debug_printf("Received: ");
		    for (i = 0; i < RADIO_HAL_TOTAL_PACKET_WIDTH; i++) {
    			debug_printf("%c(0x%x) ", rxBuffer[i],rxBuffer[i]);
		    }
		    debug_printf("\r\n");
		    s4527438_hal_radio_setfsmrx();
	    }
	    s4527438_hal_radio_fsmprocessing();
        HAL_Delay(MAIN_LOOP_POLLING_DELAY);
    }

    return 0;
}

/**
  * @brief  Initialise Hardware
  * @param  None
  * @retval None
  */
void Hardware_init(void) {
	s4527438_hal_radio_init();
}

