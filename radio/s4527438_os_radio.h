/** 
 **************************************************************
 * @file mylib/s4527438_os_radio.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib radio driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_OS_RADIO_H_
#define S4527438_OS_RADIO_H_

typedef enum{
    VACUUM_ON,
    VACUUM_OFF,
} Message_Vacuum_Action_Enum;

void s4527438_os_radio_init(void);
void s4527438_os_radio_send_join_packet(void);
void s4527438_os_radio_send_xyz_packet(uint32_t x_coordinate, uint32_t y_coordinate, uint32_t z_coordinate);
void s4527438_os_radio_send_vacuum_packet(Message_Vacuum_Action_Enum vacuum_action);
#endif /* S4527438_OS_RADIO_H_ */
