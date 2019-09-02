/** 
 **************************************************************
 * @file mylib/s4527438_os_pantilt.h
 * @author KO-CHEN CHI - s4527438
 * @date 26032019
 * @brief mylib pantilt driver
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 *************************************************************** 
 */

#ifndef S4527438_OS_PANTILT_H_
#define S4527438_OS_PANTILT_H_

extern void s4527438_os_pantilt_init(void);
extern void s4527438_os_pantilt_deinit(void);

extern void s4527438_os_pan_write_angle(int angle);
extern void s4527438_os_tilt_write_angle(int angle);

extern void s4527438_os_pan_left(void);
extern void s4527438_os_pan_right(void);

extern void s4527438_os_tilt_up(void);
extern void s4527438_os_tilt_down(void);
#endif /* S4527438_OS_PANTILT_H_ */
