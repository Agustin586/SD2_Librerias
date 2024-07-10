/*
 * sd.c
 *
 *  Created on: 9 jul. 2024
 *      Author: agustin
 */

#include "Include/sd.h"
#include "Include/spi.h"

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
		unsigned START_BIT:1;
		unsigned TRANSMISION_BIT:1;
		unsigned COMMAND_INDEX:6;
		unsigned ARGUMENT:32;
		unsigned CRC7:7;
		unsigned END_BIT:1;
	};
	struct {
		uint32_t msb; // 4 bytes
		uint16_t lsb; // 2 bytes
	};
} Send_Command_t;

/*< FUNCIONES PRIVADAS >*/
static void sd_assert_cs(void);
static void sd_deassert_cs(void);
static uint8_t sd_read_response1(void);
static void sd_command(uint8_t cmd, uint32_t arg, uint8_t crc);

SD_RETURN_CODES_t sd_init(void) {
	uint8_t SD_Response[5]; // array to hold response
	uint8_t cmdAttempts = 0;

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

	for (i = 0; i < 10; i++)
		spi_write(0xFF);

	// Card powers up in SD Bus protocol mode and switch to SPI
	// when the Chip Select line is driven low and CMD0 is sent.
	// In SPI mode CRCs are ignored but because they start in SD Bus mode
	// a correct checksum must be provided.

	/*
	 * @brief	GO_IDLE_STATE (CMD0)
	 *
	 * Con esto reseteamos la tarjeta sd y vamos al estado de idle
	 * con lo cual la tarjeta sd responde con un formato de tipo 1
	 * en donde el lsb corresponde al idle state --> '1' si se encuentra ahi.
	 *
	 * */
//	while (SD_Response[0] != 0x01) {
	while (!IN_IDLE(SD_Response[0])) {
		sd_assert_cs();

		/*< Envia comando 0 >*/
		sd_command(GO_IDLE_STATE, CMD0_ARG, CMD0_CRC);

		/*< Lee respuesta de formato 1 >*/
		SD_Response[0] = sd_read_response1();

		/*< Envia dummy clocks y levanta el cs >*/
		sd_deassert_cs();

		cmdAttempts++;

		if (cmdAttempts > 10) {
			CS_HIGH;	/*< Deshabilita el cs levantandolo >*/
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
	sd_command(CMD8, CMD8_ARG, CMD8_CRC);
	sd_read_response3_7(SD_Response);
	sd_deassert_cs();

	// Enable maximum SPI speed
#if AVR_CLASS_SPI == 0
	_SPSR = (1 << SPI2X); // set clock rate fosc/2
	_SPCR &= ~(1 << SPR1);
#else
			SPI0.CTRLA &= ~SPI_PRESC_gm; // fosc/4
			SPI0.CTRLA |= SPI_CLK2X_bm; // SPI speed (SCK frequency) is doubled in Master mode
		#endif

	// Select initialization sequence path
	if (SD_Response[0] == 0x01) {
		// The card is Version 2.00 (or later) or SD memory card
		// Check voltage range
		if (SD_Response[3] != 0x01) {
			return SD_NONCOMPATIBLE_VOLTAGE_RANGE;
		}

		// Check echo pattern
		if (SD_Response[4] != 0xAA) {
			return SD_CHECK_PATTERN_MISMATCH;
		}

		// CMD58 - read OCR (Operation Conditions Register) - R3 response
		// Reads the OCR register of a card
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

		// Check if the card is ready
		// Read bit OCR 31 in R3
		if (!(SD_Response[1] & 0x80)) {
			return SD_POWER_UP_BIT_NOT_SET;
		}

		// Read CCS bit OCR 30 in R3
		if (SD_Response[1] & 0x40) {
			// SDXC or SDHC card
			SD_CardType = SD_V2_SDHC_SDXC;
		} else {
			// SDSC
			SD_CardType = SD_V2_SDSC;
		}

	} else if (SD_Response[0] == 0x05) {
		// Response code 0x05 = Idle State + Illegal Command indicates
		// the card is of first generation. SD or MMC card.
		SD_CardType = SD_V1_SDSC;

		// ACMD41
		SD_Response[0] = sd_command_ACMD41();
		if (ILLEGAL_CMD(SD_Response[0]))
			return SD_NOT_SD_CARD;
		if (SD_Response[0])
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

}

uint8_t sd_read_single_block(uint32_t addr, uint8_t *buf) {

}

bool sd_detected(void) {

}

/*
 * @brief	Afirma la linea de chip select
 * */
static void sd_assert_cs(void) {
	spi_write(0xFF);
	CS_LOW;
	spi_write(0xFF);

	return;
}

/*
 * @brief	De-selecciona la tarjeta
 * */
static void sd_deassert_cs(void) {
	spi_write(0xFF);
	CS_HIGH;
	spi_write(0xFF);

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
	volatile static Send_Command_t Format_cmd;
	/*
	 * @brief	volatile para que no se optimize y pueda ver si se encuentra
	 * escrita de manera adecuada en el debugger. En el desarrollo puede sacarse
	 * tranquilamente si no se necesita acceder.
	 * */
	Format_cmd.START_BIT = 0;
	Format_cmd.TRANSMISION_BIT = 1;
	Format_cmd.COMMAND_INDEX = cmd & 0b00111111;	/*< Para que sea 6 bits >*/
	Format_cmd.ARGUMENT = arg;
	Format_cmd.CRC7 = crc & 0b01111111;			/*< Solo toma los primeros 7 bits >*/
	Format_cmd.END_BIT = 1;

	/*
	 * Transmitimos todos los bytes que podemos en una sola llamada.
	 * Para esto hacemos primero uso de los 32 bits mas significativos y
	 * luego de los ultimos 16 bits.
	 * */

	// Transmit command
	spi_write(Format_cmd.msb);
	spi_write(Format_cmd.lsb);

	return;
}

