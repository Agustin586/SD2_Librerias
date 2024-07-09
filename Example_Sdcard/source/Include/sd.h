/*
 * Date: 9/07/2024 (Inicio)
 *
 * Title: Libreria para manipular una memoria sd
 *
 * Description: Se configura para trabajar con rtos o sin rtos. Para esto
 * se hace uso de las macro USE_FREERTOS o USE_NOT_FREERTOS. Es importante notar
 * que esto cambia el uso de libreria spi. En el modo de rtos utilizaremos una libreria
 * de spi que provee una abstracción de SPI_RTOS_XTRANSFER. Mientras que en la otra utilizaremos
 * el modo bloqueante del spi. Si faltan deben agregarse dichas librerias y la abstracción.
 *
 * Author: Zuliani, Agustin
 *
 * Versiones:
 * 			v1.0
 */

#ifndef INCLUDE_SD_H_
#define INCLUDE_SD_H_

//#define USE_FREERTOS
#define USE_NOT_FREERTOS



#endif /* INCLUDE_SD_H_ */
