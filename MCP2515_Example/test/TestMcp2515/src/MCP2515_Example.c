/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MCP2515_Example.c
 * @brief   Application entry point.
 */

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "Includes/spi.h"
#include "Includes/mcp2515.h"

/* TODO: insert other definitions and declarations here. */

#if USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"

#define __delay_ms(x) vTaskDelay(pdMS_TO_TICKS(x))
#define taskCanMsg_STACK configMINIMAL_STACK_SIZE + 20
#define taskCanMsg_PRIORITIES 1

static void vtaskRtos_canmsg(void *pvParameter);

#else

#define __delay_ms(x) delay_ms(x)
static void delay_ms(uint16_t ms);
static void canmsg_baremetal(void);

static void delay_ms(uint16_t ms)
{
    // Calcula el número de ciclos necesarios
    uint32_t cycles = (CLOCK_GetCoreSysClkFreq() / 1000) * ms / 4;

    // Realiza el bucle para generar el retardo
    for (uint32_t i = 0; i < cycles; i++)
    {
        __NOP(); // No Operation (1 ciclo de instrucción)
    }

    return;
}

#endif

struct can_frame canMsg1;
struct can_frame canMsgRead = {
		.can_dlc = 0,
		.can_id = 0,
};

/**
 * @brief   Application entry point.
 */
int main(void)
{
	ERROR_t error;

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Ejemplo del modulo can MCP2515\n\r");

    canMsg1.can_id = 125;
    canMsg1.can_dlc = 8;
    canMsg1.data[0] = 10;
    canMsg1.data[1] = 22;
    canMsg1.data[2] = 32;
    canMsg1.data[3] = 16;
    canMsg1.data[4] = 26;
    canMsg1.data[5] = 78;
    canMsg1.data[6] = 69;
    canMsg1.data[7] = 5;

    mcp2515_init();

    error = mcp2515_reset();

    if (error != ERROR_OK)
    	PRINTF("Fallo al resetear el modulo\n\r");

    error = mcp2515_setBitrate(CAN_125KBPS, MCP_8MHZ);

    if (error != ERROR_OK)
    	PRINTF("Fallo al setear el bit rate\n\r");

//    mcp2515_setNormalMode();
    error = mcp2515_setLoopbackMode();

    if (error != ERROR_OK)
    	PRINTF("Fallo al setear el loopback mode\n\r");

#if (USE_FREERTOS)

    xTaskCreate(vtaskRtos_canmsg, "Task Can Msg", taskCanMsg_STACK, NULL, taskCanMsg_PRIORITIES, NULL);

    vTaskStartScheduler();

#endif

    while (1)
    {
#if (!USE_FREERTOS)
    canmsg_baremetal();
#endif
    }
    return 0;
}

#if USE_FREERTOS

static void vtaskRtos_canmsg(void *pvParameter)
{
    mcp2515_sendMessage(&canMsg1);
    __delay_ms(100);

    while (true)
    {
    }

    return;
}

#else

static void canmsg_baremetal(void)
{
	ERROR_t estado;

	estado = mcp2515_sendMessage(&canMsg1);

	if (estado == ERROR_OK)
	{
		PRINTF("ID\tDLC\tDATA\n\r");
		PRINTF("%d\t%d\t",canMsg1.can_id,canMsg1.can_dlc);

		for (uint8_t i = 0; i < 8;i++){
			PRINTF("%d ",canMsg1.data[i]);
		}
		PRINTF("\n\r");
	}
	else
	{
		PRINTF("Error al enviar\n\r");
	}
    __delay_ms(500);

    estado = mcp2515_readMessage(&canMsgRead);

    if (estado != ERROR_NOMSG)
    {
		PRINTF("ID\tDLC\tDATA\n\r");
		PRINTF("%d\t%d\t",canMsgRead.can_id,canMsgRead.can_dlc);

		for (uint8_t i = 0; i < 8;i++){
			PRINTF("%d ",canMsgRead.data[i]);
		}
    }
    else
    {
    	PRINTF("No hubo mensajes");
    }

    PRINTF("\n\r");

    __delay_ms(1000);

    return;
}

#endif
