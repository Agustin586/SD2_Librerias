/*
 * sd.c
 *
 *  Created on: 9 jul. 2024
 *      Author: agustin
 */

#include "Include/sd.h"
#include "Include/spi.h"
#include "fsl_debug_console.h"
#include "clock_config.h"

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#ifdef	USE_NOT_FREERTOS
static void Blocking_delay(uint8_t ms);

#define	_delay_ms(x)	Blocking_delay(x)

static void Blocking_delay(uint8_t ms) {
	uint32_t cycles = ms / ((100000.0 / CLOCK_GetCoreSysClkFreq()) / 4.0);

	// Realiza el bucle para generar el retardo
	for (uint32_t i = 0; i < cycles; i++) {
		__NOP(); // No Operation (1 ciclo de instrucción)
	}

	return;
}
#elif defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"

#define _delay_ms(x)	vtaskDelay(pdMS_TO_TICKS(x))
#endif

/*< VARIABLES >*/
typedef union {
	struct {
		unsigned START_BIT :1;
		unsigned TRANSMISION_BIT :1;
		unsigned COMMAND_INDEX :6;
		unsigned ARGUMENT :32;
		unsigned CRC7 :7;
		unsigned END_BIT :1;
	};
	struct {
		uint8_t byte1; // 4 bytes
		uint8_t byte2;
		uint8_t byte3;
		uint8_t byte4;
		uint8_t byte5; // 2 bytes
		uint8_t byte6;
	};
} Send_Command_t;

typedef enum {
	kStatus_Sd_PARAMETER_ERROR = 0,
	kStatus_Sd_ADDRESS_ERROR,
	kStatus_Sd_ERASE_SEQUENCE_ERROR,
	kStatus_Sd_COM_CRC_ERROR,
	kStatus_Sd_ILEGAL_COMMAND,
	kStatus_Sd_ERASE_ERROR,
	kStatus_Sd_IN_IDLE_STATE,
} kStatus_Sd_t;

static uint8_t SD_CardType;
uint8_t SD_ResponseToken;

/*< FUNCIONES PRIVADAS >*/
static void sd_assert_cs(void);
static void sd_deassert_cs(void);
static uint8_t sd_read_response1(void);
static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc);
static bool sd_read_response3_7(uint8_t *res);
static SD_RETURN_CODES_t sd_command_ACMD41(void);

SD_RETURN_CODES_t sd_init(void) {
	uint8_t SD_Response[5]; // array to hold response
	uint8_t cmdAttempts = 0;
	uint8_t buffer_aux;

	/*< Inicializacion del spi >*/
	spi_init();

	/*< Configura el chip select >*/
	PORT_SetPinMux(SPI_CS_PORT, SPI_CS_PIN, kPORT_MuxAsGpio);
	gpio_pin_config_t cs_config = { kGPIO_DigitalOutput, 1 };
	GPIO_PinInit(SPI_CS_GPIO, SPI_CS_PIN, &cs_config);

	/*< Tiempo de inicializacion >*/
	_delay_ms(1);

	/*
	 * Envia 80 ciclos de reloj para detectar el modo.
	 * */
	sd_deassert_cs();

	buffer_aux = 0xFF;
	for (uint8_t i = 0; i < 10; i++) {
		spi_write(&buffer_aux);
	}

	/*
	 * @brief	GO_IDLE_STATE (CMD0)
	 *
	 * Con esto reseteamos la tarjeta sd y vamos al estado de idle
	 * con lo cual la tarjeta sd responde con un formato de tipo 1
	 * en donde el lsb corresponde al idle state --> '1' si se encuentra ahi.
	 *
	 * */
//	while (SD_Response[0] != 0x01) {
	while (!IN_IDLE(SD_Response[R1_RESPONSE])) {
		sd_assert_cs();

		/*< Envia comando 0 >*/
		sd_command(GO_IDLE_STATE, CMD0_ARG, CMD0_CRC);

		/*< Lee respuesta de formato 1 >*/
		SD_Response[R1_RESPONSE] = sd_read_response1();

		/*< Envia dummy clocks y levanta el cs >*/
		sd_deassert_cs();

		cmdAttempts++;

		if (cmdAttempts > 10) {
			CS_HIGH; /*< Deshabilita el cs levantandolo >*/
			return SD_IDLE_STATE_TIMEOUT;
		}
	}

	// Send CMD8 - SEND_IF_COND (Send Interface Condition) - R7 response (or R1 for < V2 cards)
	// Sends SD Memory Card interface condition that includes host supply voltage information and asks the
	// accessed card whether card can operate in supplied voltage range.
	// Check whether the card is first generation or Version 2.00 (or later).
	// If the card is of first generation, it will respond with R1 with bit 2 set (illegal command)
	// otherwise the response will be 5 bytes long R7 type
	// Voltage Supplied (VHS) argument is set to 3.3V (0b0001)
	// Check Pattern argument is the recommended pattern '0b10101010'
	// CRC is 0b1000011 and is precalculated
	sd_assert_cs();

	/*
	 * Envia el comando 8
	 * */
	sd_command(SEND_IF_COND, CMD8_ARG, CMD8_CRC);

	if (sd_read_response3_7(SD_Response))
		PRINTF("Error: during send command 8 or "
				"sd card is not v2\r\n");

	sd_deassert_cs();

	/*
	 * NOTA: en esta altura podriamos cambiar la velocidad del spi
	 * a la maxima posible.
	 * */

	/*< Secuencia de inicializacion >*/
	if (SD_Response[R1_RESPONSE] == IDLE_STATE) /*R1*/{
		// The card is Version 2.00 (or later) or SD memory card

		/*< Byte 3 de R7. Chequea el rango de voltaje. >*/
		if (SD_Response[R7_RESPONSE_VOLTAGE_ACCEPTED_BYTE] != VOLTAGE_ACC_27_36) {
			return SD_NONCOMPATIBLE_VOLTAGE_RANGE;
		}

		/*< Byte 5. Chequea el echo de check pattern >*/
		if (SD_Response[R7_RESPONSE_CHECK_PATTERN_BYTE] != CHECK_PATTERN) {
			return SD_CHECK_PATTERN_MISMATCH;
		}

		// CMD58 - read OCR (Operation Conditions Register) - R3 response
		/*
		 * LECTURA DE OCR DESDE LA TARJETA SD
		 *
		 * Se realiza para detectar si cumple con el HCS (memoria
		 * de alta capacidad) y el rango de voltaje.
		 *  */
		sd_assert_cs();

		sd_command(CMD58, CMD58_ARG, CMD58_CRC);
		sd_read_response3_7(SD_Response);

		sd_deassert_cs();

		// ACMD41 - starts the initialization process - R1 response
		// Continue to send ACMD41 (always preceded by CMD55) until the card responds
		// with 'in_idle_state', which is R1 = 0x00.
		if (sd_command_ACMD41() > 0)
			return SD_IDLE_STATE_TIMEOUT;

		// CMD58 - read OCR (Operation Conditions Register) - R3 response
		sd_assert_cs();

		sd_command(CMD58, CMD58_ARG, CMD58_CRC);
		sd_read_response3_7(SD_Response);

		sd_deassert_cs();

		/*
		 * Verifica si la tarjeta ya realizó la inicializacion.
		 * Para esto debe leer el bit 31 de R3.
		 *
		 * */
		if (!(POWER_UP_STATUS(SD_Response[OCR_BYTE_nrm1]))) {
			return SD_POWER_UP_BIT_NOT_SET;
		}

		/*
		 * Chequea si se seteo correctamente el hcs en la memoria.
		 * Se debe verificar el bit 30 del ocr.
		 *
		 * */
		if (CCS_VAL(SD_Response[OCR_BYTE_nrm1])) {
			// SDXC or SDHC card
			SD_CardType = SD_V2_SDHC_SDXC;
		} else {
			// SDSC
			SD_CardType = SD_V2_SDSC;
		}
		/*
		 * ACA TERMINA LA INICIALIZACIÓN SI LA TARJETA ES DE
		 * VERSIÓN V2 O SUPERIOR.
		 * */

	} else if (SD_Response[R1_RESPONSE] == IDLE_STATE_AND_ILLEGAL_CMD) {
		// Response code 0x05 = Idle State + Illegal Command indicates

		/*
		 * Si ingresa aca significa que la tarjeta es de versión
		 * v1 o anterior.
		 * */
		SD_CardType = SD_V1_SDSC;

		// ACMD41
		SD_Response[R1_RESPONSE] = sd_command_ACMD41();

		if (ILLEGAL_CMD(SD_Response[R1_RESPONSE]))
			return SD_NOT_SD_CARD;
		if (SD_Response[R1_RESPONSE])
			return SD_IDLE_STATE_TIMEOUT;

	} else {
		return SD_GENERAL_ERROR;
	}

	// Read and write operations are performed on SD cards in blocks of set lengths.
	// Block length can be set in Standard Capacity SD cards using CMD16 (SET_BLOCKLEN).
	// For SDHC and SDXC cards the block length is always set to 512 bytes.

	return SD_OK;
}

uint8_t sd_write_single_block(uint32_t addr, uint8_t *buf) {
#define SD_MAX_WRITE_ATTEMPTS    0.25 * (1 / CLOCK_GetFreq((SPI0_CLK_SRC)))

	uint8_t res1;
	uint8_t aux;
	uint32_t readAttempts;

	if (SD_CardType == SD_V1_SDSC)
		addr *= 512;

	// set token to none
	SD_ResponseToken = 0xFF;

	/*< Habilita el chip select de la tarjeta >*/
	sd_assert_cs();

	/*
	 * El comando 24 indica que la escritura es de un solo bloque
	 * simple. En vez el 25 indica que la escritura es de multiples
	 * bloques, por tanto necesitará un comando que finalice la
	 * escritura.
	 * */
	sd_command(CMD24, addr, CMD24_CRC);	// Envia el comando 24

	/* Respuesta de formato R1 */
	res1 = sd_read_response1();

	/*
	 * Verifica que el dato recivido no sea con error.
	 * */
	if (res1 == SUCCESS_RESONSE_R1) {
		/* Token de inicio */
		aux = START_BLOCK_TOKEN;
		spi_write(&aux);

		/*< Envia el buffer >*/
		for (uint16_t i = 0; i < SD_BUFFER_SIZE; i++)
			spi_write(buf),buf++;// Envia el buffer entero de 512 bytes. La funcion se encarga.

		// wait for a response (timeout = 250ms)
		// maximum timeout is defined as 250 ms for all write operations
		readAttempts = 0;

		while (++readAttempts < SD_MAX_WRITE_ATTEMPTS) {
			spi_receive(&res1);
			if (res1 != BUSSY)
				break;
		}

		/* Dato aceptado */
		if ((res1 & MASK_DATA_RESPONSE) == DATA_ACCEPTED) {
			// set token to data accepted
			SD_ResponseToken = DATA_ACCEPTED;

			// wait for write to finish (timeout = 250ms)
			readAttempts = 0;

			spi_receive(&res1);

			while (res1 == SUCCESS_RESONSE_R1) {
				if (++readAttempts > SD_MAX_WRITE_ATTEMPTS) {
					SD_ResponseToken = 0x00;
					break;
				}
			}
		}
	}

	sd_deassert_cs();

	return res1;
}

uint8_t sd_write_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *count) {
#define SD_MAX_WRITE_ATTEMPTS    0.25 * (1 / CLOCK_GetFreq((SPI0_CLK_SRC)))

	uint8_t res1;
	uint8_t aux;
	uint32_t readAttempts;

	if (SD_CardType == SD_V1_SDSC)
		addr *= 512;

	// set token to none
	SD_ResponseToken = 0xFF;

	/*< Habilita el chip select de la tarjeta >*/
	sd_assert_cs();

	/*
	 * El comando 24 indica que la escritura es de un solo bloque
	 * simple. En vez el 25 indica que la escritura es de multiples
	 * bloques, por tanto necesitará un comando que finalice la
	 * escritura.
	 * */
	sd_command(CMD25, addr, CMD25_CRC);	// Envia el comando 24

	/* Respuesta de formato R1 */
	res1 = sd_read_response1();

	/*
	 * Verifica que el dato recivido no sea con error.
	 * */
	if (res1 == SUCCESS_RESONSE_R1) {
		do {
			/* Token de inicio */
			aux = START_BLOCK_TOKEN_MULTIPLE_BLOCK;
			spi_write(&aux);

			/*< Envia el buffer >*/
			for (uint16_t i = 0; i < SD_BUFFER_SIZE; i++)
				spi_write(buf),buf++;// Envia el buffer entero de 512 bytes. La funcion se encarga.

			// wait for a response (timeout = 250ms)
			// maximum timeout is defined as 250 ms for all write operations
			readAttempts = 0;

			while (++readAttempts < SD_MAX_WRITE_ATTEMPTS) {
				spi_receive(&res1);
				if (res1 != BUSSY)
					break;
			}

			/* Dato aceptado */
			if ((res1 & MASK_DATA_RESPONSE) == DATA_ACCEPTED) {
				// set token to data accepted
				SD_ResponseToken = DATA_ACCEPTED;

				// wait for write to finish (timeout = 250ms)
				readAttempts = 0;

				spi_receive(&res1);

				while (res1 == SUCCESS_RESONSE_R1) {
					if (++readAttempts > SD_MAX_WRITE_ATTEMPTS) {
						SD_ResponseToken = 0x00;
						break;
					}
				}
			}
		} while (--(*count));

		/*< Finaliza la transmisión de datos >*/
		aux = STOP_TRANSMISSION_MULTIPLE_BLOCK;
		spi_write(&aux);
	} else {
		PRINTF("Error: during writting\r\n");
	}

	sd_deassert_cs();

	return res1;
}

uint8_t sd_read_single_block(uint32_t addr, uint8_t *buf) {
#define SD_MAX_READ_ATTEMPTS    0.1 * (1 / CLOCK_GetFreq((SPI0_CLK_SRC)))

	uint8_t res1, read = 0;
	uint8_t crc[2];
	uint32_t readAttempts;

	if (SD_CardType == SD_V1_SDSC)
		addr *= 512;

	// set token to none
	SD_ResponseToken = 0xFF;

	/*< Habilita el chip select de la tarjeta sd >*/
	sd_assert_cs();

	/*< Comando para escribir en un bloque simple >*/
	sd_command(CMD17, addr, CMD17_CRC);

	/*< Lee el formato de R1 >*/
	res1 = sd_read_response1();

	/* Verifica que respondio algo */
	if (res1 != 0xFF) {
		// wait for a response token (timeout = 100ms)
		// The host should use 100ms timeout (minimum) for single and multiple read operations
		readAttempts = 0;

		while (++readAttempts != SD_MAX_READ_ATTEMPTS) {
			spi_receive(&read);
			if (read != 0xFF)
				break;
		}

		// if response token is 0xFE
		if (read == 0xFE) {
			/*< Lee los 512 bytes >*/
			for (uint16_t i = 0; i < SD_BUFFER_SIZE; i++)
				spi_receive(buf), buf++;

			/*< Pone un final de cadena '\0' = 0 >*/
			*buf = 0;

			/*< Lee el crc >*/
			spi_receive(crc);
			spi_receive(crc);
		}

		// set token to card response
		SD_ResponseToken = read;
	}

	sd_deassert_cs();

	return res1;
}

uint8_t sd_read_multiple_block(uint32_t addr, uint8_t *buf, uint8_t *count) {
#define SD_MAX_READ_ATTEMPTS    0.1 * (1 / CLOCK_GetFreq((SPI0_CLK_SRC)))

	uint8_t res1, read = 0;
	uint8_t crc[2];
	uint32_t readAttempts;

	if (SD_CardType == SD_V1_SDSC)
		addr *= 512;

	/* Seteamos un token */
	SD_ResponseToken = 0xFF;

	/*< Habilita el chip select de la tarjeta sd >*/
	sd_assert_cs();

	/*< Comando para leer en multiples bloques CMD18 >*/
	sd_command(CMD18, addr, CMD18_CRC);

	/*< Lee el formato de R1 >*/
	res1 = sd_read_response1();

	/* Verifica que respondio algo distinto de bussy 0xFF */
	if (res1 != BUSSY) {
		do {
			/*
			 * Genera una espera de 100 ms. Sale de dicha espera cuando
			 * recive algo distinto de bussy en read.
			 * */
			readAttempts = 0;
			while (++readAttempts != SD_MAX_READ_ATTEMPTS) {
				spi_receive(&read);
				if (read != BUSSY)
					break;
			}

			/* Da inicio a la recepcion de datos con el token 0xFE */
			if (read == START_BLOCK_TOKEN) {
				/*< Lee los 512 bytes de 1 byte >*/
				for (uint16_t i = 0; i < SD_BUFFER_SIZE; i++)
					spi_receive(buf), buf++;

				/*< Lee el crc >*/
				spi_receive(crc);
				spi_receive(crc);
			}

			// set token to card response
			SD_ResponseToken = read;
		} while (--(*count));

		/*< Pone un final de cadena '\0' = 0 >*/
		*buf = 0;

		/*< Detiene la lectura de datos con el comando CMD12 >*/
		sd_command(CMD12, 0, 0x00);
	}

	sd_deassert_cs();

	return res1;
}

/*
 * @brief	Afirma la linea de chip select
 * */
static void sd_assert_cs(void) {
	uint8_t buffer;

	buffer = 0xFF;

	spi_write(&buffer);
	CS_LOW;
	spi_write(&buffer);

	return;
}

/*
 * @brief	De-selecciona la tarjeta
 * */
static void sd_deassert_cs(void) {
	uint8_t buffer;

	buffer = 0xFF;

	spi_write(&buffer);
	CS_HIGH;
	spi_write(&buffer);

	return;
}

/*
 * @brief	Respuesta de formato 1
 * */
static uint8_t sd_read_response1(void) {
	uint8_t i = 0, response = 0xFF;

	/*< Toma los datos por polling >*/
	spi_receive(&response);

	while (response == 0xFF) {
		i++;

		/*< Toma 16 bytes si ninguno cambia termina >*/
		if (i > 16)
			break;

		spi_receive(&response);
	}

	if (PARAM_ERROR(response))
		PRINTF("Error: param error %d", kStatus_Sd_PARAMETER_ERROR);
	else if (ADDR_ERROR(response))
		PRINTF("Error: addr error %d", kStatus_Sd_ADDRESS_ERROR);
	else if (CRC_ERROR(response))
		PRINTF("Error: crc error %d", kStatus_Sd_COM_CRC_ERROR);
	else if (ERASE_RESET(response))
		PRINTF("Error: erase reset %d", kStatus_Sd_ERASE_ERROR);
	else if (ILLEGAL_CMD(response))
		PRINTF("Error: illgeal command %d", kStatus_Sd_ILEGAL_COMMAND);

	return response;
}

/*
 * @brief	Envia un comando hacia la tarjeta sd. Recordar que son 48 bits.
 *
 * @param	uint8_t cmd		Comando
 * 			uint8_t arg		Argumento
 * 			uint8_t crc		CRC
 * */
static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
	static Send_Command_t Format_cmd;
	/*
	 * @brief	volatile para que no se optimize y pueda ver si se encuentra
	 * escrita de manera adecuada en el debugger. En el desarrollo puede sacarse
	 * tranquilamente si no se necesita acceder.
	 * */
	Format_cmd.START_BIT = 0;
	Format_cmd.TRANSMISION_BIT = 1;
	Format_cmd.COMMAND_INDEX = cmd & 0b00111111; /*< Para que sea 6 bits >*/
	Format_cmd.ARGUMENT = arg;
	Format_cmd.CRC7 = crc & 0b01111111; /*< Solo toma los primeros 7 bits >*/
	Format_cmd.END_BIT = 1;

	/*
	 * Transmitimos todos los bytes que podemos en una sola llamada.
	 * Para esto hacemos primero uso de los 32 bits mas significativos y
	 * luego de los ultimos 16 bits.
	 * */

	// Transmit command
	spi_write(&Format_cmd.byte1);
	spi_write(&Format_cmd.byte2);
	spi_write(&Format_cmd.byte3);
	spi_write(&Format_cmd.byte4);
	spi_write(&Format_cmd.byte5);
	spi_write(&Format_cmd.byte6);

	return;
}

/*
 * @brief	Respuesta a 3 y 7
 *
 * @param	uint8_t* res	Respuesta
 * */
static bool sd_read_response3_7(uint8_t *res) {
	*res = sd_read_response1();

	if (*res > 1)
		return true;

	spi_receive((res + 1));

	return false;
}

/*
 * @brief	Envia el comando de ACMD41
 *
 * @return	SD_RETURN_CODES
 * */
static SD_RETURN_CODES_t sd_command_ACMD41(void) {
	uint8_t response;
	uint8_t i = 100;

	/*
	 * Proceso de inicializacion. Puede tomar al rededor de 1 segundo en finalizar.
	 * */
	do {
		// CMD55 - APP_CMD - R1 response
		sd_assert_cs();

		sd_command(CMD55, CMD55_ARG, CMD55_CRC);
		sd_read_response1();

		sd_deassert_cs();

		// ACMD41 - SD_SEND_OP_COND (Send Operating Condition) - R1 response
		sd_assert_cs();

		/*
		 * Envia el comando hasta que la tarjeta se inicialice.
		 * */
		if (SD_CardType == SD_V1_SDSC)
			sd_command(ACMD41, 0, ACMD41_CRC); /*< Argumento = 0x00 para versiones v1 o anteriores >*/
		else
			sd_command(ACMD41, ACMD41_ARG, ACMD41_CRC);

		/*< Respuesta desde la tarjeta sd >*/
		response = sd_read_response1();

		/*
		 * Si response es 1 significa que la tarjeta todavía no se
		 * inicializó. Debe esperarse hasta recivir un 0.
		 * */

		sd_deassert_cs();

		i--;

		_delay_ms(10);

	} while ((response != 0) && (i > 0));

	// Timeout
	if (i == 0)
		return SD_IDLE_STATE_TIMEOUT;

	return response;
}

