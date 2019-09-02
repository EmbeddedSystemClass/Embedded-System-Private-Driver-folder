/** 
 **************************************************************
 * @file mylib/s4527438_hal_pantilt.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib pantilt driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_HAL_PANTILT_H_
#define S4527438_HAL_PANTILT_H_

#define PAN_TYPE    0
#define TILT_TYPE   1

void s4527438_hal_pantilt_init(void);
void pantilt_angle_write(int type, int angle);
int pantilt_angle_read(int type);

#define s4527438_hal_pantilt_pan_write(angle) pantilt_angle_write(PAN_TYPE,angle)
#define s4527438_hal_pantilt_pan_read() pantilt_angle_read(PAN_TYPE)

#endif /* S4527438_HAL_PANTILT_H_ */
