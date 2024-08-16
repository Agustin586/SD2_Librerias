/*
 * spi.c
 *
 *  Created on: 9 jul. 2024
 *      Author: agustin
 */

/**
 * @file spi.c
 * @brief Libreria del spi para manipular con freertos o con baremetal.
 * @author Zuliani, Agustin
 * @date 09/07/24
 */

#include "Includes/spi.h"
#include "fsl_debug_console.h"
#include "clock_config.h"
#include "Includes/mcp2515.h"
#include <string.h>

// #define USE_FREERTOS 0

#if (!USE_FREERTOS)

#include "fsl_spi.h"

#elif (USE_FREERTOS)

#include "fsl_spi_freertos.h"
#include "FreeRTOS.h"
#include "task.h"
static spi_rtos_handle_t master_rtos_handle;

#endif

/* Definiciones >*/
#define BUFFER_SIZE 25
#define SPI_MASTER_BASE SPI0
#define SPI_MASTER_IRQN SPI0_IRQn
#define SPI_MASTER_CLK_SRC SPI0_CLK_SRC
#define SPI_MASTER_CLK_FREQ CLOCK_GetFreq((SPI0_CLK_SRC))
#define SPI_MASTER_BASEADDR ((SPI_Type *)SPI_MASTER_BASE)
#define SPI_NVIC_PRIO 1

/* Variables */
// static uint8_t srcBuff[BUFFER_SIZE];
//static uint8_t destBuff[BUFFER_SIZE];

/* Funciones */
extern void spi_init(void)
{
	spi_master_config_t masterConfig;
	uint32_t sourceClock;
#if USE_FREERTOS
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

#if (USE_FREERTOS)
	status = SPI_RTOS_Init(&master_rtos_handle, SPI_MASTER_BASEADDR,
						   &masterConfig, sourceClock);

	if (status != kStatus_Success)
	{
		PRINTF("DSPI master: error during initialization. \r\n");
		while (1)
			;
	}
#elif (!USE_FREERTOS)
	SPI_MasterInit(SPI_MASTER_BASEADDR, &masterConfig, sourceClock);
#endif

	return;
}

extern void spi_write(uint8_t *tx_buffer, uint16_t n)
{
	spi_transfer_t masterXfer = {0};
	status_t status;

	masterXfer.txData = tx_buffer;
	masterXfer.rxData = NULL;
	masterXfer.dataSize = n;

#if USE_FREERTOS
	status = SPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);
#elif (!USE_FREERTOS)
	status = SPI_MasterTransferBlocking(SPI_MASTER_BASE, &masterXfer);
#endif

	if (status == kStatus_Success)
	{
		PRINTF("SPI transfer completed successfully. \r\n");
	}
	else
	{
		PRINTF("SPI transfer completed with error. \r\n");
	}

	return;
}

extern void spi_receive(uint8_t *rx_buffer, uint8_t n)
{
	spi_transfer_t masterXfer = {0};
	status_t status;

	masterXfer.txData = NULL;
	masterXfer.rxData = rx_buffer;
	masterXfer.dataSize = n;

#if USE_FREERTOS
	status = SPI_RTOS_Transfer(&master_rtos_handle, &masterXfer);
#elif (!USE_FREERTOS)
	status = SPI_MasterTransferBlocking(SPI_MASTER_BASE, &masterXfer);
#endif

	if (status == kStatus_Success)
	{
		PRINTF("SPI transfer completed successfully. \r\n");
	}
	else
	{
		PRINTF("SPI transfer completed with error. \r\n");
	}

	return;
}
