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
 * @file    SD2_LCD_not_I2C.c
 * @brief   Application entry point.
 *
 * @author	Zuliani, Agustin
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "Include/Lcd.h"

/* TODO: insert other definitions and declarations here. */
/*
 * @brief	Alguno de los siguiente define debe ser comentado dependiendo
 * si se utiliza freertos o no. La diferencia se encuentra en el uso de tareas y
 * delays.
 * */

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#ifdef	USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

#define taskPrint_PRIORITIES	tskIDLE_PRIORITY + 1
#define taskPrint_STACK			configMINIMAL_STACK_SIZE

static void vtaskRtos_Print(void *pvParameters);
#elif defined(USE_NOT_FREERTOS)
static void Blocking_delay_ms(uint32_t ms);
#endif

/*
 * @brief   Application entry point.
 */
int main(void) {
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
#endif

	PRINTF("=============================\r\n");
	PRINTF("Manejo de lcd sin i2c\r\nSistemas digitales 2\r\n");
	PRINTF("=============================\r\n");

	PRINTF("Debe definirse en la macro si se utiliza freertos o no\r\n");

#ifdef USE_FREERTOS
	xTaskCreate(vtaskRtos_Print, "task Print Lcd", taskPrint_STACK, NULL,
			taskPrint_PRIORITIES, NULL);

	vTaskStartScheduler();

	while (1)
	;
#elif defined(USE_NOT_FREERTOS)
	static uint8_t contador = 0;
	static char buffer[20];

	Lcd_init();

	Lcd_cadena(1, 1, "SD2 NOT FREERTOS");

	Lcd_cadena(2, 4, "Lcd 16x2");

	Blocking_delay_ms(5000);

	Lcd_comando(CLEAR);

	while (1) {
		sprintf(buffer, "Valor: %d", contador);
		Lcd_cadena(1, 1, buffer);
		contador++;
		Blocking_delay_ms(500);		/*< Podria hacerse uso del systick para no bloquear >*/
	}
#endif

	return 0;
}

#ifdef	USE_FREERTOS
static void vtaskRtos_Print(void *pvParameters) {
	static uint8_t contador = 0;
	static char buffer[20];

	PRINTF("Tarea: Lcd\r\n");

	Lcd_init();

	Lcd_comando(CLEAR);

	Lcd_cadena(1, 1, "SD2 FREERTOS");

	Lcd_cadena(2, 4, "Lcd 16x2");

	vTaskDelay(pdMS_TO_TICKS(5000));

	Lcd_comando(CLEAR);

	for (;;) {
		sprintf(buffer, "Valor: %d", contador);
		Lcd_cadena(1, 1, buffer);
		Lcd_comando(HOME);
		contador++;

		vTaskDelay(pdMS_TO_TICKS(500));
	}

	vTaskDelete(NULL);

	return;
}

#elif defined(USE_NOT_FREERTOS)
static void Blocking_delay_ms(uint32_t ms) {
	// Calcula el número de ciclos necesarios
	uint32_t cycles = ms / ((100000.0/CLOCK_GetCoreSysClkFreq()) / 4.0);
//	uint32_t cycles = (CLOCK_GetCoreSysClkFreq() / 1000) * ms / 4;

	// Realiza el bucle para generar el retardo
	for (uint32_t i = 0; i < cycles; i++) {
		__NOP(); // No Operation (1 ciclo de instrucción)
	}

	return;
}
#endif
