/*
 * Fecha: 9/07/24
 *
 * Titulo: Spi
 *
 * Descripcion: con esta libreria nos encargamos de utilizar la versión de rtos
 * o la versión bloqueante del spi según se trabaje con una u otra. Es necesario
 * definir alguna de las macros USE_FREERTOS o USE_NOT_FREERTOS para determinar que
 * librerias se deben incluir en el spi.
 *
 * Autor: Zuliani, Agustin.
 *
 * Version:
* 			v1.0
 */

#ifndef INCLUDE_SPI_H_
#define INCLUDE_SPI_H_

//#define USE_FREERTOS
#define USE_NOT_FREERTOS

/*< Archivos >*/
#include <stdint.h>

/*< Funciones >*/
extern void spi_init(void);
extern void spi_write(uint8_t *tx_buffer);
extern void spi_receive(uint8_t *rx_buffer);
#endif /* INCLUDE_SPI_H_ */
