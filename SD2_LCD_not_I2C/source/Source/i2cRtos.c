/*
 * i2cConfig.c
 *
 *  Created on: 29 jun. 2024
 *      Author: Zuliani, Agustin
 */

/*< Archivos >*/
#include "Include/i2cRtos.h"
#include "fsl_debug_console.h"
#include "fsl_i2c_freertos.h"

/*< Definiciones >*/
#define I2C_RELEASE_SDA_PORT PORTE
#define I2C_RELEASE_SCL_PORT PORTE
#define I2C_RELEASE_SDA_GPIO GPIOE
#define I2C_RELEASE_SDA_PIN 25U
#define I2C_RELEASE_SCL_GPIO GPIOE
#define I2C_RELEASE_SCL_PIN 24U
#define I2C_RELEASE_BUS_COUNT 100U
#define I2C_MASTER_IRQN (I2C0_IRQn)

#define I2C_MASTER_SLAVE_ADDR_7BIT (0x1d)
#define I2C_ACCEL_ADDRESS 0x1D
#define I2C_BAUDRATE 			100000U
#define I2C_DATA_LENGTH (32)  /* MAX is 256 */

static i2c_master_config_t masterConfig;
static i2c_rtos_handle_t master_rtos_handle;
static i2c_master_transfer_t masterXfer;

extern void i2cRtos_init(void) {
	uint32_t sourceClock;
	i2c_master_transfer_t masterXfer;
	status_t status;

	/*< Prioridad del i2c >*/
	NVIC_SetPriority(I2C_MASTER_IRQN, 0);

	/*
	 * masterConfig.baudRate_Bps = 100000U;
	 * masterConfig.enableStopHold = false;
	 * masterConfig.glitchFilterWidth = 0U;
	 * masterConfig.enableMaster = true;
	 */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;
	sourceClock = CLOCK_GetFreq(I2C0_CLK_SRC);

	/*< Incializacion >*/
	status = I2C_RTOS_Init(&master_rtos_handle, I2C0, &masterConfig,
			sourceClock);

	if (status != kStatus_Success) {
		PRINTF("I2C master: error during init, %d", status);
		while (1)
			;
	}

	return;
}

extern void i2cRtos_xtransfer(i2c_master_transfer_t master_transfer) {
	status_t status;

	/*< Cargo datos en masterXfer >*/
	masterXfer.slaveAddress = master_transfer.slaveAddress;
	masterXfer.direction = master_transfer.direction;
	masterXfer.subaddress = master_transfer.subaddress;
	masterXfer.subaddressSize = master_transfer.subaddressSize;
	masterXfer.data = master_transfer.data;
	masterXfer.dataSize = master_transfer.dataSize;
	masterXfer.flags = master_transfer.flags;

	/*< Transmito >*/
	status = I2C_RTOS_Transfer(&master_rtos_handle, &masterXfer);
	if (status != kStatus_Success) {
		PRINTF("I2C master: error during transfer\r\n");
		while (1)
			;
	}

	return;
}
