/*
 * Lcd.c
 *
 *  Created on: 6 jul. 2024
 *      Author: Zuliani, Agustin
 */

#include "Include/Lcd.h"

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#if  defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"

#define __delay_ms(x)	vTaskDelay(pdMS_TO_TICKS(x))

#elif	defined(USE_NOT_FREERTOS)
static void delay_ms_(uint32_t ms);

#define __delay_ms(x)	delay_ms_(x)

static void delay_ms_(uint32_t ms) {
	// Calcula el número de ciclos necesarios
	uint32_t cycles = (CLOCK_GetCoreSysClkFreq() / 1000) * ms / 4;

	// Realiza el bucle para generar el retardo
	for (uint32_t i = 0; i < cycles; i++) {
		__NOP(); // No Operation (1 ciclo de instrucción)
	}

	return;
}
#endif

static void pins_init(void);

static void pins_init(void) {
	/* Port A Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortA);
	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);
	/* Port D Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortD);
	/* Port E Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortE);

	gpio_pin_config_t gpio_config = { .pinDirection = kGPIO_DigitalOutput,
			.outputLogic = 0U };

	GPIO_PinInit(RS_BASE, RS_PIN, &gpio_config);
	GPIO_PinInit(E_BASE, E_PIN, &gpio_config);
	GPIO_PinInit(D4_BASE, D4_PIN, &gpio_config);
	GPIO_PinInit(D5_BASE, D5_PIN, &gpio_config);
	GPIO_PinInit(D6_BASE, D6_PIN, &gpio_config);
	GPIO_PinInit(D7_BASE, D7_PIN, &gpio_config);

	PORT_SetPinMux(RS_PORT, RS_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(E_PORT, E_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(D4_PORT, D4_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(D5_PORT, D5_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(D6_PORT, D6_PIN, kPORT_MuxAsGpio);
	PORT_SetPinMux(D7_PORT, D7_PIN, kPORT_MuxAsGpio);

	return;
}

/*
 * @brief	Debe inicializarse dentro de una tarea, dado que utiliza
 * 			vtaskdelay.
 * */
extern void LCD_init(void) {
	//Setea como salida los pines para el lcd
	pins_init();

	//Tiempo de inicilizacion
	__delay_ms(30);

	for (char i = 0; i < 3; i++) {
		LCD_command(0x03);         //Recomendado para inicializar el controlador
		__delay_ms(5);
	}

	//Configuraciones del LCD 16x2
	LCD_command(0x33);                  //Inicializa el controlador
	__delay_ms(1);
	LCD_command(0x32);                  //Modo de 4 bits
	__delay_ms(3);
	LCD_command(0x28);                  //Modo 4 bits,2 lineas y fuente de 5x7
	__delay_ms(1);
	LCD_command(0x0C);
	__delay_ms(1);
	LCD_command(0x06);
	__delay_ms(1);
	LCD_command(CLEAR);                 //Limpia la pantalla
	__delay_ms(5);

	return;
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_command(unsigned char cmd) {
	//Limpia puerto
	PIN_state(D4_BASE, D4_PIN, LOW);
	PIN_state(D5_BASE, D5_PIN, LOW);
	PIN_state(D6_BASE, D6_PIN, LOW);
	PIN_state(D7_BASE, D7_PIN, LOW);

	//Envia MSB
	PIN_state(D4_BASE, D4_PIN, (cmd >> 4) & 0x01);
	PIN_state(D5_BASE, D5_PIN, (cmd >> 5) & 0x01);
	PIN_state(D6_BASE, D6_PIN, (cmd >> 6) & 0x01);
	PIN_state(D7_BASE, D7_PIN, (cmd >> 7) & 0x01);

	//Configura en modo comando
	PIN_state(RS_BASE, RS_PIN, LOW);
	PIN_state(E_BASE, E_PIN, LOW);

	//Cargo los datos
	PIN_state(E_BASE, E_PIN, HIGH);
	__delay_ms(1);
	PIN_state(E_BASE, E_PIN, LOW);
	__delay_ms(1);

	//Limpia puerto
	PIN_state(D4_BASE, D4_PIN, LOW);
	PIN_state(D5_BASE, D5_PIN, LOW);
	PIN_state(D6_BASE, D6_PIN, LOW);
	PIN_state(D7_BASE, D7_PIN, LOW);

	//Envia LSB
	PIN_state(D4_BASE, D4_PIN, (cmd & 0x01));
	PIN_state(D5_BASE, D5_PIN, (cmd >> 1) & 0x01);
	PIN_state(D6_BASE, D6_PIN, (cmd >> 2) & 0x01);
	PIN_state(D7_BASE, D7_PIN, (cmd >> 3) & 0x01);

	//Modo Comando
	PIN_state(RS_BASE, RS_PIN, LOW);
	PIN_state(E_BASE, E_PIN, LOW);

	//Cargo los datos
	PIN_state(E_BASE, E_PIN, HIGH);
	__delay_ms(1);
	PIN_state(E_BASE, E_PIN, LOW);
	__delay_ms(5);

	return;
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_date(char date) {
	//Limpia puerto
	PIN_state(D4_BASE, D4_PIN, LOW);
	PIN_state(D5_BASE, D5_PIN, LOW);
	PIN_state(D6_BASE, D6_PIN, LOW);
	PIN_state(D7_BASE, D7_PIN, LOW);

	//Envia MSB
	PIN_state(D4_BASE, D4_PIN, (date >> 4) & 0x01);
	PIN_state(D5_BASE, D5_PIN, (date >> 5) & 0x01);
	PIN_state(D6_BASE, D6_PIN, (date >> 6) & 0x01);
	PIN_state(D7_BASE, D7_PIN, (date >> 7) & 0x01);

	//Modo caracteres o datos
	PIN_state(RS_BASE, RS_PIN, HIGH);
	PIN_state(E_BASE, E_PIN, LOW);

	//Cargo los datos
	PIN_state(E_BASE, E_PIN, HIGH);
	__delay_ms(1);
	PIN_state(E_BASE, E_PIN, LOW);
	__delay_ms(1);

	//Limpia puerto
	PIN_state(D4_BASE, D4_PIN, LOW);
	PIN_state(D5_BASE, D5_PIN, LOW);
	PIN_state(D6_BASE, D6_PIN, LOW);
	PIN_state(D7_BASE, D7_PIN, LOW);

	//Envia LSB
	PIN_state(D4_BASE, D4_PIN, (date & 0x01));
	PIN_state(D5_BASE, D5_PIN, (date >> 1) & 0x01);
	PIN_state(D6_BASE, D6_PIN, (date >> 2) & 0x01);
	PIN_state(D7_BASE, D7_PIN, (date >> 3) & 0x01);

	//Modo caracteres
	PIN_state(RS_BASE, RS_PIN, HIGH);
	PIN_state(E_BASE, E_PIN, LOW);

	//Cargo los datos
	PIN_state(E_BASE, E_PIN, HIGH);
	__delay_ms(1);
	PIN_state(E_BASE, E_PIN, LOW);
	__delay_ms(5);

	return;
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_array(int x, int y, char *date) {
	//Ubica el lcd
	switch (x) {
	case 1:
		LCD_command(0x80 | (0x00 + (y - 1)));
		break;
	case 2:
		LCD_command(0x80 | (0x40 + (y - 1)));
		break;
	case 3:
		LCD_command(0x80 | (0x14 + (y - 1)));
		break;
	case 4:
		LCD_command(0x80 | (0x54 + (y - 1)));
		break;
	}

	//Envia la cadena
	while (*date) {
		LCD_date(*date);
		__delay_ms(1);
		date++;
	}
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_xy(int x, int y) {
	switch (x) {
	case 1:
		LCD_command(0x80 | (0x00 + (y - 1)));
		break;
	case 2:
		LCD_command(0x80 | (0x40 + (y - 1)));
		break;
	case 3:
		LCD_command(0x80 | (0x14 + (y - 1)));
		break;
	case 4:
		LCD_command(0x80 | (0x54 + (y - 1)));
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_shift(unsigned char dir, unsigned char cant) {
	char i = 1;
	//Realiza el corrimiento de pantalla
	while (i <= cant) {
		LCD_command(dir);
		__delay_ms(TIEMPO_CORRIMIENTO);
		i++;
	}

	LCD_command(HOME);              //Vuelve a la posicion inicial
	i = 1;
}

////////////////////////////////////////////////////////////////////////////////

extern void LCD_character(unsigned char adress, char caracter[]) {
	LCD_command(0x40 + (adress * 8));
	for (char i = 0; i < 8; i++) {
		LCD_date((char)caracter[i]);
	}
}

/*Ejemplo de uso de la funcion de creacion de caracter:
 *char caracter1[8] =
 {
 0b00000000,
 0b00001010,
 0b00001010,
 0b00001010,
 0b00001010,
 0b00010001,
 0b00001110,
 0b00000000,
 };
 LCD_command(CLEAR);
 LCD_caracter(CARACTER_POS0,caracter1);
 LCD_xy(0,0);
 LCD_date(CARACTER_POS0);
 __delay_ms(1000);
 */

