/*
 * Lcd.h
 *
 *  Created on: 6 jul. 2024
 *      Author: Zuliani, Agustin
 */

#ifndef INCLUDE_LCD_H_
#define INCLUDE_LCD_H_
/*------------------------------------------------------------------------------
 PROGRAM DEFINES
 -----------------------------------------------------------------------------*/
#include <stdint.h>
#include "pin_mux.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

/*
 * @brief	Define el tipo de sistema que vamos a utilizar.
 * */
#define USE_NOT_FREERTOS
//#define USE_FREERTOS

//Configuraciones de pines
//#define BACKLIGHT_PIN   PORTBbits.RB0			/*< Pines para backlight >*/
//#define BACKLIGHT_BASE  BASEBbits.BASEB0
#define RS_PIN      13U
#define RS_BASE     GPIOA
#define RS_PORT		PORTA
#define E_PIN       9U
#define E_BASE      GPIOC
#define	E_PORT		PORTC
#define D4_PIN      2U
#define D4_BASE     GPIOD
#define	D4_PORT		PORTD
#define D5_PIN      4U
#define D5_BASE     GPIOD
#define D5_PORT		PORTD
#define D6_PIN      6U
#define D6_BASE     GPIOD
#define D6_PORT		PORTD
#define D7_PIN      7U
#define D7_BASE     GPIOD
#define D7_PORT		PORTD
//#define BITS_PIN    PORTB
//#define BITS_BASE   BASEB

//Seteo de pines
#define HIGH	1
#define LOW		0
#define PIN_state(base,pin,state)	GPIO_WritePinOutput(base,pin,state)

//Funciones del lcd
#define CLEAR           0x01        //Limpiar el display
#define CURSOR_OFF      0x0C        //Desactiva el cursor
#define CURSOR_ON       0x0E        //Actva el cursor solo
#define CURSOR_BLINK_ON 0x0F        //Activa el cursor y el destello
#define HOME            0x02        //Vuelve a la posicion home 0,0
#define DISPLAY_RIGHT   0x1C        //Corrimiento de pantalla a la derecha
#define DISPLAY_LEFT    0x18        //Corrimiento de pantalla a la izquierda
#define CURSOR_RIGHT    0x14        //Movimiento del cursor a la derecha
#define CURSOR_LEFT     0x10        //Movimiento del cursor a la izquierda
#define PRIMERA_FILA    0x02        //Ubica en la primera fila
#define SEGUNDA_FILA    0xC0        //Ubica en la segunda fila
#define TERCERA_FILA    0x94        //Ubica en la tercera fila
#define CUARTA_FILA     0xD4        //Ubica en la cuarta fila

//Generacion de caracter
#define CARACTER_POS0 0x00          //Caracter posicion 0
#define CARACTER_POS1 0x01          //Caracter posicion 1
#define CARACTER_POS2 0x02          //Caracter posicion 2
#define CARACTER_POS3 0x03          //Caracter posicion 3
#define CARACTER_POS4 0x04          //Caracter posicion 4
#define CARACTER_POS5 0x05          //Caracter posicion 5
#define CARACTER_POS6 0x06          //Caracter posicion 6
#define CARACTER_POS7 0x07          //Caracter posicion 7

//Otras
#define TIEMPO_CORRIMIENTO	250		//Tiempo de desplazamiento o corriemiento en ms

//Definiciones de funciones
#define Lcd_init        LCD_init
#define Lcd_comando     LCD_command
#define Lcd_cadena      LCD_array
#define Lcd_posicion    LCD_xy
#define Lcd_dato        LCD_date
#define Lcd_corrimiento LCD_shift
#define Lcd_caracter    LCD_character

//Generacion de numero grandes
#define JUST_RIGHT 0x10  //Numeros con justificacion derecha de la pantalla
#define JUST_LEFT 0x11   //Numeros con justificacion izquierda de la pantalla

/*------------------------------------------------------------------------------
 FUNCTION PROTOTYPES
 -----------------------------------------------------------------------------*/
extern void LCD_init(void);                                			// Permite inicializa el lcd
extern void LCD_command(unsigned char cmd);                  		// Envia un comando al lcd
extern void LCD_array(int x, int y, char *date); 					// Envia una cadena de caracteres al lcd
extern void LCD_xy(int x, int y);                           		// Envia la posicion al lcd
extern void LCD_date(char date);                            		// Envia un caracter al lcd
extern void LCD_shift(unsigned char dir, unsigned char cant); 		// Realiza un corrimiento
extern void LCD_character(unsigned char adress, char caracter[]); 	// Genera un caracter especial

#endif /* INCLUDE_LCD_H_ */
