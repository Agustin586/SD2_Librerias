/*
 * Fecha: 9/07/24
 *
 * Titulo: Spi
 *
 * Descripcion: con esta libreria nos encargamos de utilizar la versión de rtos
 * o la versión bloqueante del spi según se trabaje con una u otra. Para utilizar
 * de manera correcta se debe definir si se utiliza o no un rto. Si se utiliza freertos
 * se debe poner USE_FREERTOS 1, si no se utiliza se debe poner USE_FREERTOS 0.
 *
 * Autor: Zuliani, Agustin.
 *
 * Version:
 * 			v1.0
 */

#ifndef INCLUDE_SPI_H_
#define INCLUDE_SPI_H_

/* Archivos */
#include <stdint.h>

/* Funciones */
/**
 * @brief Inicializacion del spi
 */
extern void spi_init(void);
/**
 * @brief Escritura de datos
 * @param[in] tx_buffer buffer donde se manda la informacion
 * @param[in] n numeros de bytes de datos
 */
extern void spi_write(uint8_t *tx_buffer, uint16_t n);
/**
 * @brief Recepcion de datos
 * @param[out] rx_buffer buffer donde se cargan los datos
 * @param[in] n numeros de bytes
 */
extern void spi_receive(uint8_t *rx_buffer, uint8_t n);

#endif /* INCLUDE_SPI_H_ */
