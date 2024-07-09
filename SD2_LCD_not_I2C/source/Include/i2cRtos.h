/*
 * i2cConfig.h
 *
 *  Created on: 29 jun. 2024
 *      Author: aguat
 */

#ifndef INCLUDE_I2CRTOS_H_
#define INCLUDE_I2CRTOS_H_

/*< Archivos >*/
#include "fsl_i2c_freertos.h"

/*< Funciones >*/
extern void i2cRtos_init(void);
extern void i2cRtos_xtransfer(i2c_master_transfer_t master_transfer);

#endif /* INCLUDE_I2CRTOS_H_ */
