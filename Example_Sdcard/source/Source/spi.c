/*
 * spi.c
 *
 *  Created on: 9 jul. 2024
 *      Author: agustin
 */

#include "Include/spi.h"
#include "fsl_debug_console.h"
#include "clock_config.h"
#include <string.h>

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#ifdef	USE_NOT_FREERTOS
#include "fsl_spi.h"
#elif defined(USE_FREERTOS)
#include "fsl_spi_freertos.h"
#include "FreeRTOS.h"
#include "task.h"

static spi_rtos_handle_t master_rtos_handle;
#endif

/*< Definiciones >*/
#define BUFFER_SIZE 			25
#define SPI_MASTER_BASE 		SPI0_BASE
#define SPI_MASTER_IRQN 		SPI0_IRQn
#define SPI_MASTER_CLK_SRC 		SPI0_CLK_SRC
#define SPI_MASTER_CLK_FREQ 	CLOCK_GetFreq((SPI0_CLK_SRC))
#define SPI_MASTER_BASEADDR 	((SPI_Type *)SPI_MASTER_BASE)
#define SPI_NVIC_PRIO 			1

/*< Variables >*/
//static uint8_t srcBuff[BUFFER_SIZE];
static uint8_t destBuff[BUFFER_SIZE];

/*< Funciones>*/
extern void spi_init(void) {
	spi_master_config_t masterConfig;
	uint32_t sourceClock;
#ifdef USE_FREERTOS
	status_t status;
#endif

	NVIC_SetPriority(SPI_MASTER_IRQN, SPI_NVIC_PRIO);

	/*
	 * masterConfig.enableStopInWaitMode = false;
	 * masterConfig.polarity = kSPI_ClockPolarityActiveHigh;
	 * masterConfig.phase = kSPI_ClockPhaseFirstEdge;
	 * masterConfig.direction = kSPI_MsbFirst;
	 * masterConfig.dataMode = kSPI_8BitMode;
	 * masterConfig.txWatermark = kSPI_TxFifoOneHalfEmpty;
	 * masterConfig.rxWatermark = kSPI_RxFifoOneHalfFull;
	 * masterConfig.pinMode = kSPI_PinModeNormal;
	 * masterConfig.outputMode = kSPI_SlaveSelectAutomaticOutput;
	 * masterConfig.baudRate_Bps = 500000U;
	 */
	SPI_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 400000;

	sourceClock = SPI_MASTER_CLK_FREQ;

#ifdef	USE_FREERTOS
	status = SPI_RTOS_Init(&master_rtos_handle, SPI_MASTER_BASEADDR,
			&masterConfig, sourceClock);

	if (status != kStatus_Success) {
		PRINTF("DSPI master: error during initialization. \r\n");
		while (1)
			;
	}
#elif defined(USE_NOT_FREERTOS)
	SPI_MasterInit(SPI_MASTER_BASEADDR, &masterConfig, sourceClock);
#endif

	return;
}

extern void spi_write(uint8_t *tx_buffer) {
	spi_transfer_t masterXfer = { 0 };
	status_t status;

	masterXfer.txData = tx_buffer;
	masterXfer.rxData = NULL;
	masterXfer.dataSize = 1;

#ifdef USE_FREERTOS
	status = SPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);
#elif	defined(USE_NOT_FREERTOS)
	status = SPI_MasterTransferBlocking(SPI_MASTER_BASE, &masterXfer);
#endif

	if (status == kStatus_Success) {
		PRINTF("SPI transfer completed successfully. \r\n");
	} else {
		PRINTF("SPI transfer completed with error. \r\n");
	}

	return;
}

extern void spi_receive(uint8_t *rx_buffer) {
	spi_transfer_t masterXfer = { 0 };
	status_t status;

//	memcpy(srcBuff, tx_Data, sizeof(tx_Data));

	masterXfer.txData = NULL;
	masterXfer.rxData = rx_buffer;
	masterXfer.dataSize = 1;

#ifdef USE_FREERTOS
	status = SPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);
#elif defined(USE_NOT_FREERTOS)
	status = SPI_MasterTransferBlocking(SPI_MASTER_BASE, &masterXfer);
#endif

	if (status == kStatus_Success) {
		PRINTF("SPI transfer completed successfully. \r\n");
	} else {
		PRINTF("SPI transfer completed with error. \r\n");
	}

	return;
}
