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

/**
 * @brief Retardo de tiempo bloqueante.
 */
#define __delay_ms(x) delay_ms(x)

/**
 * @brief Retardo bloqueante.
 * @param[in] ms Tiempo en milisegundos
 */
static void delay_ms(uint16_t ms);
/**
 * @brief Lectura y escritura del modulo can.
 */
static void canmsg_baremetal(void);
/**
 * @brief Escritura del modulo can.
 */
static void canmsg_escritura(void);
/**
 * @brief Lectura del modulo can.
 */
static void canmsg_lectura(void);

/**
 * @brief Interrupcion por recepcion de datos del modulo can.
 *
 * Cuando se genera una interrupcion por recepcion se activa
 * la bandera y luego dicha bandera se detecta por polling en
 * el programa principal.
 *
 * @note Debe ser de tipo volatile para que no sea optimizada
 * por el compilador, dado que es una variable que se modifica
 * en una interrupcion.
 */
volatile static bool Rx_flag_mcp2515 = false;

#endif

/**
 * @brief Mensaje 1 de can.
 */
struct can_frame canMsg1;
/**
 * @brief Mensaje de lectura.
 */
struct can_frame canMsgRead = {
		.can_dlc = 0,
		.can_id = 0,
};

/**
 * @brief Inicializacion de perifericos.
 */
static void perifericos_init(void);
/**
 * @brief Configuraciones de interrupcion.
 */
static void interrupt_init(void);

/**
 * @brief   Application entry point.
 */
int main(void)
{

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Ejemplo del modulo can MCP2515\n\r");

    canMsg1.can_id = 232;
    canMsg1.can_dlc = 8;
    canMsg1.data[0] = 10;
    canMsg1.data[1] = 22;
    canMsg1.data[2] = 32;
    canMsg1.data[3] = 16;
    canMsg1.data[4] = 26;
    canMsg1.data[5] = 78;
    canMsg1.data[6] = 69;
    canMsg1.data[7] = 5;

#if (!USE_FREERTOS)

    /*
     * @note
     * Si utilizamos el modo de baremetal entonces podemos
     * inicializar tranquilamente el modulo.
     * En vez si utilizamos un rtos debido al uso de retardos
     * como lo es vtaskdelay debemos encontrarnos en el ámbito
     * del sistema de tiempo real, es decir una tarea. Habiendo
     * inicializado el scheduler anteriormente.
     * */
    perifericos_init();
    interrupt_init();

#endif

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

static void canmsg_baremetal(void)
{
	canmsg_escritura();

	if (Rx_flag_mcp2515)
		canmsg_lectura(),Rx_flag_mcp2515 = false;

    __delay_ms(500);

    return;
}

static void canmsg_escritura(void)
{
	ERROR_t estado;

	estado = mcp2515_sendMessage(&canMsg1);

	if (estado == ERROR_OK)
	{
		PRINTF("\nMensaje enviado\n\r");
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

	return;
}

static void canmsg_lectura(void)
{
	ERROR_t estado;

	estado = mcp2515_readMessage(&canMsgRead);

    if (estado != ERROR_NOMSG)
    {
    	PRINTF("\nMensaje de recepcion\n\r");
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

	return;
}

#endif

static void perifericos_init(void)
{
	ERROR_t error;

    mcp2515_init();

    error = mcp2515_reset();

    if (error != ERROR_OK)
    	PRINTF("Fallo al resetear el modulo\n\r");

    error = mcp2515_setBitrate(CAN_125KBPS, MCP_8MHZ);

    if (error != ERROR_OK)
    	PRINTF("Fallo al setear el bit rate\n\r");

//    error = mcp2515_setNormalMode();
//    if (error != ERROR_OK)
//      	PRINTF("Fallo al setear el normal mode\n\r");
    error = mcp2515_setLoopbackMode();
    if (error != ERROR_OK)
    	PRINTF("Fallo al setear el loopback mode\n\r");

	return;
}

#define PIN_NUMBER 17

static void interrupt_init(void)
{
	CLOCK_EnableClock(kCLOCK_PortA);  // Por ejemplo, para el puerto A

	port_pin_config_t config = {
	    .pullSelect = kPORT_PullUp,
	    .slewRate = kPORT_FastSlewRate,
	    .passiveFilterEnable = kPORT_PassiveFilterDisable,
	    .driveStrength = kPORT_LowDriveStrength,
	    .mux = kPORT_MuxAsGpio
	};

	PORT_SetPinConfig(PORTA, PIN_NUMBER, &config);
	PORT_SetPinInterruptConfig(PORTA, PIN_NUMBER, kPORT_InterruptFallingEdge); // Configura interrupción por flanco descendente

	NVIC_EnableIRQ(PORTA_IRQn);  // Habilita la interrupción para el puerto A

	gpio_pin_config_t gpioConfig = {
	    .pinDirection = kGPIO_DigitalInput,
	    .outputLogic = 0U
	};

	GPIO_PinInit(GPIOA, PIN_NUMBER, &gpioConfig);

	return;
}

void PORTA_IRQHandler(void) {
    // Obtiene el estado de las banderas de interrupción del puerto A
    uint32_t interruptFlags = GPIO_GetPinsInterruptFlags(GPIOA);

    if (interruptFlags & (1U << PIN_NUMBER))
    {
		// Código que se ejecutará cuando ocurra la interrupción

    	/*
    	 * @note
    	 * Se configuraron por defecto interrupciones para rx0, rx1, err, merr. En la
    	 * funcion de mcp2515_reset() se pueden configurar algunas mas.
    	 * */

    	/* Leemos las interrupciones generadas */
    	ERROR_t error = mcp2515_getInterrupts();
    	if (error != ERROR_OK)
    		PRINTF("Fallo al leer la interrupcion\n\r");

    	/* Detectamos las que nos sirvan */
    	if (mcp2515_getIntERRIF())
    	{
    		// Acciones ...
    		PRINTF("Error interrupt flag\n\r");

    		// Limpiamos la bandera
    		mcp2515_clearERRIF();
    	}

    	if (mcp2515_getIntMERRF())
    	{
    		// Acciones ...
    		PRINTF("Message error interrupt flag\n\r");

    		// Limpiamos la bandera
    		mcp2515_clearMERR();
    	}

    	if (mcp2515_getIntRX0IF() || mcp2515_getIntRX1IF())
    	{
    		// Acciones ...
    		Rx_flag_mcp2515 = true;

    		// La bandera se limpia en la funcion de recepcion
    		// mcp2515_readMessage().
    	}

    	/*
    	 * Descomentar si es necesario tener en cuenta dicha interrupcion.
    	 * Ademas debe habilitarse en la funcion de reset del mcp2515.
    	 * */
//    	if (mcp2515_getIntTX0IF() ||
//    		mcp2515_getIntTX1IF() ||
//			mcp2515_getIntTX2IF())
//    	{
//    		// Acciones ...
////    		PRINTF("Mensaje enviado\n\r");
//
//    		// Limpiamos la bandera
//    		mcp2515_clearTXInterrupts();
//    	}

		// Limpia la bandera de interrupción
		GPIO_ClearPinsInterruptFlags(GPIOA, 1U << PIN_NUMBER);
    }

    return;
}
