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

#ifndef S4527438_CLI_PANTILT_H_
#define S4527438_CLI_PANTILT_H_

void s4527438_cli_pantilt_init(void);
extern CLI_Command_Definition_t xPan;
extern CLI_Command_Definition_t xTilt;

#endif /* S4527438_CLI_PANTILT_H_ */
