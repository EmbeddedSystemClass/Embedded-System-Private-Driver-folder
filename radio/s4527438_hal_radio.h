/** 
 **************************************************************
 * @file mylib/s4527438_hal_radio.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib radio driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_HAL_RADIO_H_
#define S4527438_HAL_RADIO_H_


void s4527438_hal_radio_init(void);
void s4527438_hal_radio_fsmprocessing(void);
void s4527438_hal_radio_setchan(unsigned char
chan);
void s4527438_hal_radio_settxaddress(unsigned
char *addr);
void s4527438_hal_radio_setrxaddress(unsigned
char *addr);
unsigned char s4527438_hal_radio_getchan(void);
void s4527438_hal_radio_gettxaddress(unsigned
char *addr);
void s4527438_hal_radio_getrxaddress(unsigned
char *addr);
void s4527438_hal_radio_sendpacket(char
chan, unsigned char *addr, unsigned
char *txpacket);
void s4527438_hal_radio_setfsmrx(void);
int s4527438_hal_radio_getrxstatus(void);
void s4527438_hal_radio_getpacket(unsigned
char *rxpacket);


#define S4527438_RADIO_IDLE_STATE       0
#define S4527438_RADIO_RX_STATE         1
#define S4527438_RADIO_TX_STATE         2
#define S4527438_RADIO_WAITING_STATE    3
#define S4527438_RADIO_STATE_MAX_VALUE  S4527438_RADIO_WAITING_STATE

#endif /* S4527438_HAL_RADIO_H_ */
