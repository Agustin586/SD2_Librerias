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

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_clock.h"
#include <stdbool.h>

//#define USE_FREERTOS
#define USE_NOT_FREERTOS

/*
 * CONFIGURACION DEL SPI
 *
 * Descripcion: se debe configurar los pines adecuados para
 * trabajar en modo de spi. Utilizamos mosi,miso,cs,sck. Para
 * ver que hace cada uno remontese a la teoria sobre spi.
 *
 * */
#define SPI_MISO_PIN	18U
#define SPI_MISO_PORT	PORTE
#define SPI_MOSI_PIN	19U
#define SPI_MOSI_PORT	PORTE
#define SPI_CS_PIN		16U
#define SPI_CS_PORT		PORTE
#define SPI_CS_GPIO		GPIOE
#define SPI_SCK_PIN		17U
#define SPI_SCK_PORT	PORTE

#define CS_LOW	GPIO_ClearPinsOutput(SPI_CS_GPIO, 1U << SPI_CS_PIN)
#define CS_HIGH	GPIO_SetPinsOutput(SPI_CS_GPIO, 1U << CS_PSPI_CS_PININ)

/*
 * NOTA: Si bien la utilidad de definir los parametros anteriores
 * es configurar todo desde dicha libreria por el momento obviamos
 * el hecho de incluir un init de los pines del spi y utilizamos
 * el configurador grafico para configurar de manera rápida tales pines
 * definido arriba.
 *
 * */

/*
 * COMANDOS DE LA TARJETA SD
 *
 * Descripcion: aca incluimos todos los comandos que se utilizan en la tarjeta
 * sd para el modo de spi. No haremos uso de todos a lo largo de la lirberia
 * pero si necesitan utilizarse ya se encuentran definidos.
 *
 * */
#define	CMD0	0
#define CMD1	1
#define CMD6	6
#define CMD8	8
#define CMD9	9
#define CMD10	10
#define CMD12	12
#define CMD13	13
#define CMD16	16
#define CMD17	17
#define CMD18	18
#define CMD24	24
#define CMD25	25
#define CMD27	27
#define CMD28	28
#define CMD29	29
#define CMD55	55
#define CMD56	56
#define CMD58	58
#define CMD59	59

#define	GO_IDLE_STATE			CMD0
#define SEND_OP_COND			CMD1
#define SWITCH_FUNC				CMD6
#define SEND_IF_COND			CMD8
#define SEND_CSD				CMD9
#define SEND_CID				CMD10
#define STOP_TRANSMISSION		CMD12
#define SEND_STATUS				CMD13
#define SET_BLOCKLEN			CMD16
#define READ_SINGLE_BLOCK		CMD17
#define READ_MULTIPLE_BLOCK		CMD18
#define WRITE_BLOCK				CMD24
#define WRITE_MULTIPLE_BLOCK	CMD25
#define PROGRAM_CSD				CMD27
#define SET_WRITE_PROT			CMD28
#define CLR_WRITE_PROT			CMD29
#define APP_CMD					CMD55
#define GEN_CMD					CMD56
#define READ_OCR				CMD58
#define CRC_ON_OFF				CMD59

#define SD_BUFFER_SIZE							512 // CAUTION: only 512 bytes/block is implemented

/*
 * RESPUESTAS DE LA SD
 *
 * Descripcion: cuando se emite un comando la tarjeta sd
 * responde con formatos 1,2,3,7.
 *
 * */

// R1 Response
#define PARAM_ERROR(X)							X & 0b01000000
#define ADDR_ERROR(X)							X & 0b00100000
#define ERASE_SEQ_ERROR(X)						X & 0b00010000
#define CRC_ERROR(X)							X & 0b00001000
#define ILLEGAL_CMD(X)							X & 0b00000100
#define ERASE_RESET(X)							X & 0b00000010
#define IN_IDLE(X)								X & 0b00000001
#define IDLE_STATE								0x01
#define	IDLE_STATE_AND_ILLEGAL_CMD				0X05

// R3 Response
#define POWER_UP_STATUS(X)						X & 0x80
#define CCS_VAL(X)								X & 0x40			// Card Capacity Status
#define VDD_2728(X)								X & 0b10000000
#define VDD_2829(X)								X & 0b00000001
#define VDD_2930(X)								X & 0b00000010
#define VDD_3031(X)								X & 0b00000100
#define VDD_3132(X)								X & 0b00001000
#define VDD_3233(X)								X & 0b00010000
#define VDD_3334(X)								X & 0b00100000
#define VDD_3435(X)								X & 0b01000000
#define VDD_3536(X)								X & 0b10000000

#define R1_RESPONSE		0
#define OCR_BYTE_nrm1	1	/*< MSB >*/
#define OCR_BYTE_nrm2	2
#define OCR_BYTE_nrm3	3
#define OCR_BYTE_nrm4	4	/*< LSB >*/

// R7 Response
#define CMD_VER(X)								((X >> 4) & 0xF0) // command version
#define VOL_ACC(X)								(X & 0x1F)	// voltage accepted
#define VOLTAGE_ACC_27_36						0b00000001	// 2.7-3.6V
#define VOLTAGE_ACC_LOW							0b00000010	// low voltage
#define VOLTAGE_ACC_RES1						0b00000100	// reserved
#define VOLTAGE_ACC_RES2						0b00001000	// reserved

#define R7_RESPONSE_VOLTAGE_ACCEPTED_BYTE	3
#define R7_RESPONSE_CHECK_PATTERN_BYTE		4

/*
 * PARAMETROS RAPIDOS
 *
 * */
#define CMD0_CRC	0x94
#define CMD0_ARG	0x00000000

#define CHECK_PATTERN	0xAA
#define CMD8_ARG		(VOLTAGE_ACC_27_36<<8) | CHECK_PATTERN
#define CMD8_CRC		0x86 // (1000011 << 1) //0x7C me da a mi
#define CMD58_ARG		0x00000000
#define CMD58_CRC		0x00
#define CMD55_ARG				0x00000000
#define CMD55_CRC				0x00
#define CMD17_CRC               0x00
#define CMD18_CRC				0x00
#define CMD24_CRC               0x00
#define CMD25_CRC               0x00
#define SUCCESS_RESONSE_R1		0x00

/*
 * WRITE BLOCKING
 * */
#define START_BLOCK_TOKEN	0xFE
#define BUSSY				0xFF
#define	MASK_DATA_RESPONSE	0x1F
#define DATA_ACCEPTED		0x05
#define START_BLOCK_TOKEN_MULTIPLE_BLOCK	0xFC
#define STOP_TRANSMISSION_MULTIPLE_BLOCK	0xFD

// Card Type
#define SD_V1_SDSC				1
#define SD_V2_SDSC				2
#define SD_V2_SDHC_SDXC			3

#define ACMD41					41
#define ACMD41_ARG				0x40000000	/* Setea el HCS */
#define ACMD41_CRC				0x00

/*
 * PAGINA PARA CALCULAR EL CRC
 *
 * NOTA: debe tenerse en cuenta que el polinomio es 10001001
 *
 * url : https://www.ghsi.de/pages/subpages/Online%20CRC%20Calculation/index.php?Polynom=10001001&Message=1100000900
 *
 * Se debe poner los 40 bits que se envian es decir 5 bytes (es decir lo que se envia
 * sin incluir el byte del crc).
 * */

/*
 * ESTADOS DE SALIDA DE LA MEMORIA SD
 *
 * Descripcion: segun el tipo de error que se da durante
 * la incializacion se obtendra un resultado u otro.
 *
 * */
typedef enum {
	SD_OK,
	SD_IDLE_STATE_TIMEOUT,
	SD_GENERAL_ERROR,
	SD_CHECK_PATTERN_MISMATCH,
	SD_NONCOMPATIBLE_VOLTAGE_RANGE,
	SD_POWER_UP_BIT_NOT_SET,
	SD_NOT_SD_CARD,
//SD_OP_COND_TIMEOUT,
//SD_SET_BLOCKLEN_TIMEOUT,
//SD_WRITE_BLOCK_TIMEOUT,
//SD_WRITE_BLOCK_FAIL,
//SD_READ_BLOCK_TIMEOUT,
//SD_READ_BLOCK_DATA_TOKEN_MISSING,
//SD_DATA_TOKEN_TIMEOUT,
//SD_SELECT_CARD_TIMEOUT,
//SD_SET_RELATIVE_ADDR_TIMEOUT
} SD_RETURN_CODES_t;

/*
 * PROTOTIPO DE FUNCIONES
 * */
SD_RETURN_CODES_t sd_init(void);
uint8_t sd_write_single_block(uint32_t addr, uint8_t *buf);
uint8_t sd_write_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *count);
uint8_t sd_read_single_block(uint32_t addr, uint8_t *buf);
uint8_t sd_read_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *count);
bool sd_detected(void);

#endif /* INCLUDE_SD_H_ */
