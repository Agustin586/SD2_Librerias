/*
 * mcp2515.c
 *
 *  Created on: 3 ago. 2024
 *      Author: agustin
 */

#include "Includes/mcp2515.h"
#include "Includes/spi.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include <string.h>

/**
 * Pines del spi.
 * Se defienen los siguiente pines para el módulo spi de la kl46z.
 * 	MISO: PTE19
 * 	MOSI: PTE18
 * 	CS:	  PTE16
 * 	SCK:  PTE17
 * */

#define CS_HIGH	GPIO_SetPinsOutput(BOARD_Cs_mcp2515_PORT,BOARD_Cs_mcp2515_PIN0)
#define CS_LOW	GPIO_ClearPinsOutput(BOARD_Cs_mcp2515_PORT,BOARD_Cs_mcp2515_PIN0)

/*
 * =============
 * IMPORTANTE!!!
 * =============
 *
 * Debe definirse el siguiente uso de del sistema. Si se utiliza
 * freertos entonces debe setearse en '1', si no se utiliza debe
 * configurarse en '0'. De esto dependerá que tipo de delay se va
 * a utilizar. Si se no se utiliza freertos el delay consistirá en
 * un bulce for de tipo bloqueante. Si se utiliza freertos el delay
 * consistirá en el típico vtaskDelay.
 * */
#define USE_FREERTOS	0	/**< Debe setearse en 1 si se utiliza o en 0 si no se utiliza.*/

/*< Variables para configurar los registros del modulo >*/
typedef union {
	struct {
		unsigned SJW :2;
		unsigned BRP :6;
	};
	uint8_t data;
} CNF1_t;

typedef union {
	struct {
		unsigned BTLMODE :1;
		unsigned SAM :1;
		unsigned PHSEG :3;
		unsigned PRSEG :3;
	};
	uint8_t data;
} CNF2_t;

typedef union {
	struct {
		unsigned SOF :1;
		unsigned WAKFIL :1;
		unsigned RESERVED :3;
		unsigned PHSEG2 :3;
	};
	uint8_t data;
} CNF3_t;

typedef union {
	struct {
		unsigned RESERVED :1;
		unsigned ABTF :1;
		unsigned MLOA :1;
		unsigned TXERR :1;
		unsigned RESERVED :1;
		unsigned TXP :2;
	};
	uint8_t data;
} TXBnCTRL_t;

typedef union {
	struct {
		unsigned MERRE :1;
		unsigned WAKIE :1;
		unsigned ERRIE :1;
		unsigned TX2IE :1;
		unsigned TX1IE :1;
		unsigned TX0IE :1;
		unsigned RX1IE :1;
		unsigned RX0IE :1;
	};
	uint8_t data;
} CANINTE_t;

typedef union {
	struct {
		unsigned REQ0P :3;
		unsigned ABAT :1;
		unsigned OSM :1;
		unsigned CLKEN :1;
		unsigned CLKPRE :2;
	};
	uint8_t data;
} CANCTRL_t;

typedef union {
	struct {
		unsigned RESERVED :1;
		unsigned RXM :2;
		unsigned RESERVED2 :1;
		unsigned RXRTR :1;
		unsigned BUKT :1;
		unsigned BUKT1 :1;
		unsigned FILGIT0 :1;
	};
	uint8_t data;
} RXB0CTRL_t;

typedef union {
	struct {
		unsigned RESERVED :1;
		unsigned RXM :2;
		unsigned RESERVED2 :1;
		unsigned RXRTR :1;
		unsigned FILHIT :3;
	};
	uint8_t data;
} RXB1CTRL_t;

typedef struct {
	const REGISTER_t reg;
	const uint8_t mask;
	const uint8_t data;
} ModifyReg_t;

typedef struct {
	const REGISTER_t reg;
	const INSTRUCTION_t instruction;
	uint8_t data;
} ReadReg_t;

#if USE_FREERTOS

#elif (!USE_FREERTOS)
#define __delay_ms(x)	delay_ms(x)

static void delay_ms(uint16_t ms) {
	// Calcula el número de ciclos necesarios
	uint32_t cycles = (CLOCK_GetCoreSysClkFreq() / 1000) * ms / 4;

	// Realiza el bucle para generar el retardo
	for (uint32_t i = 0; i < cycles; i++) {
		__NOP(); // No Operation (1 ciclo de instrucción)
	}

	return;
}
#endif

static const uint8_t CANCTRL_REQOP = 0xE0;
static const uint8_t CANCTRL_ABAT = 0x10;
static const uint8_t CANCTRL_OSM = 0x08;
static const uint8_t CANCTRL_CLKEN = 0x04;
static const uint8_t CANCTRL_CLKPRE = 0x03;

static const uint8_t CANSTAT_OPMOD = 0xE0;
static const uint8_t CANSTAT_ICOD = 0x0E;

static const uint8_t CNF3_SOF = 0x80;

static const uint8_t TXB_EXIDE_MASK = 0x08;
static const uint8_t DLC_MASK = 0x0F;
static const uint8_t RTR_MASK = 0x40;

static const uint8_t RXBnCTRL_RXM_STD = 0x20;
static const uint8_t RXBnCTRL_RXM_EXT = 0x40;
static const uint8_t RXBnCTRL_RXM_STDEXT = 0x00;
static const uint8_t RXBnCTRL_RXM_MASK = 0x60;
static const uint8_t RXBnCTRL_RTR = 0x08;
static const uint8_t RXB0CTRL_BUKT = 0x04;
static const uint8_t RXB0CTRL_FILHIT_MASK = 0x03;
static const uint8_t RXB1CTRL_FILHIT_MASK = 0x07;
static const uint8_t RXB0CTRL_FILHIT = 0x00;
static const uint8_t RXB1CTRL_FILHIT = 0x01;

static const uint8_t MCP_SIDH = 0;
static const uint8_t MCP_SIDL = 1;
static const uint8_t MCP_EID8 = 2;
static const uint8_t MCP_EID0 = 3;
static const uint8_t MCP_DLC = 4;
static const uint8_t MCP_DATA = 5;

static const uint8_t STAT_RXIF_MASK = STAT_RX0IF | STAT_RX1IF;

static const uint8_t EFLG_ERRORMASK = EFLG_RX1OVR | EFLG_RX0OVR | EFLG_TXBO
		| EFLG_TXEP | EFLG_RXEP;

static const uint32_t DEFAULT_SPI_CLOCK = 10000000; // 10MHz

static const int N_TXBUFFERS = 3;
static const int N_RXBUFFERS = 2;

static const struct TXBn_REGS {
	REGISTER_t CTRL;
	REGISTER_t SIDH;
	REGISTER_t DATA;
} TXB[N_TXBUFFERS];

static const struct RXBn_REGS {
	REGISTER CTRL;
	REGISTER SIDH;
	REGISTER DATA;
	CANINTF CANINTF_RXnIF;
} RXB[N_RXBUFFERS];

/*< Funciones privadas >*/
static void startSPI(void);
static void endSPI(void);

static ERROR_t mcp2515_setMode(const uint8_t mode);

static uint8_t mcp2515_readRegister(ReadReg_t *_readReg);
static void mcp2515_readRegisters(const REGISTER_t reg, uint8_t values[],
		const uint8_t n);
static void mcp2515_setRegister(const REGISTER_t reg, const uint8_t value);
static void mcp2515_setRegisters(const REGISTER_t reg, const uint8_t values[],
		const uint8_t n);
static void mcp2515_modifyRegister(const REGISTER_t reg, const uint8_t mask,
		const uint8_t data);

static void mcp2515_prepareId(uint8_t *buffer, const bool ext,
		const uint32_t id);

const struct TXBn_REGS MCP2515_TXB[N_TXBUFFERS] = { { MCP_TXB0CTRL,
		MCP_TXB0SIDH, MCP_TXB0DATA },
		{ MCP_TXB1CTRL, MCP_TXB1SIDH, MCP_TXB1DATA }, { MCP_TXB2CTRL,
				MCP_TXB2SIDH, MCP_TXB2DATA } };

const struct RXBn_REGS RXB[N_RXBUFFERS] = { { MCP_RXB0CTRL, MCP_RXB0SIDH,
		MCP_RXB0DATA, CANINTF_RX0IF }, { MCP_RXB1CTRL, MCP_RXB1SIDH,
		MCP_RXB1DATA, CANINTF_RX1IF } };

extern void mcp2515_init(const uint8_t _CS, const uint32_t _SPI_CLOCK =
		DEFAULT_SPI_CLOCK) {
	/**
	 * Inicializacion de pines.
	 *
	 * Configura los pines de entrada y salida para el modulo spi
	 * de la kl46z. Tengamos en cuenta que los pines se pueden configurar
	 * desde el ConfigTools, al igual que la frecuencia del reloj.
	 *
	 * MISO:PTE19
	 * MOSI:PTE18
	 * SCK:	PTE17
	 * CS:	PTE16
	 * */
	/* Port E Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortE);

	gpio_pin_config_t Cs_mcp2515_config = { .pinDirection = kGPIO_DigitalOutput,
			.outputLogic = 1U };
	/* Initialize GPIO functionality on pin PTE16 (pin 14)  */
	GPIO_PinInit(BOARD_Cs_mcp2515_GPIO, BOARD_Cs_mcp2515_PIN,
			&Cs_mcp2515_config);

	/* PORTE16 (pin 14) is configured as PTE16 */
	PORT_SetPinMux(BOARD_Cs_mcp2515_PORT, BOARD_Cs_mcp2515_PIN,
			kPORT_MuxAsGpio);

	/* PORTE17 (pin 15) is configured as SPI0_SCK */
	PORT_SetPinMux(PORTE, 17U, kPORT_MuxAlt2);

	/* PORTE18 (pin 16) is configured as SPI0_MOSI */
	PORT_SetPinMux(PORTE, 18U, kPORT_MuxAlt2);

	/* PORTE19 (pin 17) is configured as SPI0_MISO */
	PORT_SetPinMux(PORTE, 19U, kPORT_MuxAlt2);

	CS_HIGH; /*< Deselecciona el modulo mcp2515.*/

	/**
	 * Configuracion del spi.
	 *
	 * Se configura el spi para trabajar con/sin freertos. Ver las configuraciones
	 * iniciales en la librería spi.h y spi.c. Acordarse de seter o limpiar USE_FREERTOS.
	 * */
	spi_init();

	return;
}

static void startSPI(void) {
	/**
	 * Chip select bajo.
	 *
	 * Baja el chip select del modulo para seleccionarlo y luego
	 * leer o escribir.
	 * */
	CS_LOW;

	return;
}

static void endSPI(void) {
	/**
	 * Chip select alto.
	 *
	 * Libera el bus del mcp2515.
	 * */
	CS_HIGH;

	return;
}

extern ERROR_t mcp2515_reset(void) {
	INSTRUCTION_t _instruction = INSTRUCTION_RESET;
	REGISTER_t _register;
	CANINTE_t _caninte;

	/* Reinicio del modulo mcp2515 */
	/**
	 * Debemos reiniciar el modulo para configurar los parametros
	 * necesarios antes de utilizarlo en el can bus.
	 * */
	startSPI();
	spi_write(&_instruction);
	endSPI();

	__delay_ms(10); /**< Espera a que reinicie el modulo.*/

	uint8_t zeros[14];

	memset(zeros, 0, sizeof(zeros)); /**< Inicia con un vector en 0.*/

	/* Resetea todos los registros seteandolos en 0 */
	/**
	 * Registros de control de TX.
	 * */
	_register = MCP_TXB0CTRL;
	mcp2515_setRegisters(_register, zeros, 14);
	_register = MCP_TXB1CTRL;
	mcp2515_setRegisters(_register, zeros, 14);
	_register = MCP_TXB2CTRL;
	mcp2515_setRegisters(_register, zeros, 14);

	/**
	 * Registros de control de Rx.
	 * */
	_register = MCP_RXB0CTRL;
	mcp2515_setRegister(_register, 0);
	_register = MCP_RXB1CTRL;
	mcp2515_setRegister(_register, 0);

	/**
	 * Registro de interrupcion.
	 * */
	_register = MCP_CANINTE;
	_caninte.RX0IE = 1;
	_caninte.RX1IE = 1;
	_caninte.ERRIE = 1;
	_caninte.MERRE = 1;
	mcp2515_setRegister(_register, _caninte.data);

	/* Configuro los registros de control de Rx */
	// receives all valid messages using either Standard or Extended Identifiers that
	// meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
	ModifyReg_t _modifyReg = { 0 };
	RXB0CTRL_t _rxb0Ctrl = { 0 };
	RXB1CTRL_t _rxb1Ctrl = { 0 };

	/* RXB0 control */
	_register = MCP_RXB0CTRL;

	_rxb0Ctrl.BUKT = 1; /*< Permite el roll over del RXB0 al RXB1.*/

	_modifyReg.reg = _register;
	_modifyReg.mask = RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK;
	_modifyReg.data = _rxb0Ctrl.data;

	mcp2515_modifyRegister(_modifyReg.reg,/*< Envia el registro que va a modificar.*/
	_modifyReg.mask,/*< Envia la mascara.*/
	_modifyReg.data /*< Setea unicamente el BUKT(roll over).*/);

	/* RXB1 control */
	_register = MCP_RXB1CTRL;

	_rxb1Ctrl.data = RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT; /*< FILHIT1 = 1.*/

	_modifyReg.reg = _register;
	_modifyReg.mask = RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK;

	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	// clear filters and masks
	// do not filter any standard frames for RXF0 used by RXB0
	// do not filter any extended frames for RXF1 used by RXB1
	/**
	 * Limpiamos los registros del id y mask.
	 *
	 * Seteamos en cero el filtro para el id standart y el extendido. Tambien
	 * seteamos en cero la mascara. Luego se configurara mas adelante el
	 * filtro y mascara correspondiente.
	 * */
	RXF filters[] = { RXF0, RXF1, RXF2, RXF3, RXF4, RXF5 };

#define FILTER_CLEAR	0
#define MASK_CLEAR		0

	for (int i = 0; i < 6; i++) {
		bool ext = (i == 1);

		ERROR_t result = mcp2515_setFilter(filters[i], ext, FILTER_CLEAR);

		if (result != ERROR_OK) {
			return result;
		}
	}

	MASK masks[] = { MASK0, MASK1 };

	for (int i = 0; i < 2; i++) {
		ERROR_t result = mcp2515_setFilterMask(masks[i], true, MASK_CLEAR);

		if (result != ERROR_OK) {
			return result;
		}
	}

	return ERROR_OK;
}

static uint8_t mcp2515_readRegister(ReadReg_t *_readReg) {
	uint8_t nro_brytes;

	*_readReg->instruction = INSTRUCTION_READ;

	startSPI();
	spi_write(_readReg->instruction, 1);
	spi_write(_readReg->reg, 1);
	spi_receive(_readReg->data, &nro_brytes);
	endSPI();

	return;
}

extern void mcp2515_readRegisters(const REGISTER_t reg, uint8_t values[],
		const uint8_t n) {
	startSPI();
	SPIn->transfer(INSTRUCTION_READ);
	SPIn->transfer(reg);
	// mcp2515 has auto-increment of address-pointer
	for (uint8_t i = 0; i < n; i++) {
		values[i] = SPIn->transfer(0x00);
	}
	endSPI();

	return;
}

static void mcp2515_setRegister(const REGISTER_t reg, const uint8_t value) {
	INSTRUCTION_t _instruction;

	_instruction = INSTRUCTION_WRITE;

	/* Envia los datos al modulo mediante spi */
	startSPI();
	spi_write(&_instruction, 1);
	spi_write(&reg, 1);
	spi_write(&value, 1);
	endSPI();

	return;
}

static void mcp2515_setRegisters(const REGISTER_t reg, const uint8_t values[],
		const uint8_t n) {
	INSTRUCTION_t _instruction;

	_instruction = INSTRUCTION_WRITE;

	/* Envia los datos al modulo mediante spi */
	startSPI();
	spi_write(&_instruction, 1);
	spi_write(&reg, 1);
	spi_write(values, n);
	endSPI();

	return;
}

static void mcp2515_modifyRegister(const REGISTER_t reg, const uint8_t mask,
		const uint8_t data) {
	INSTRUCTION_t _instruction;

	_instruction = INSTRUCTION_BITMOD;

	startSPI();
	spi_write(&_instruction, 1);
	spi_write(&reg, 1);
	spi_write(&mask, 1);
	spi_write(&data, 1);
	endSPI();

	return;
}

extern uint8_t mcp2515_getStatus(void) {
	INSTRUCTION_t _instruction = INSTRUCTION_READ_STATUS;
	uint8_t i, n;

	startSPI();
	spi_write(&_instruction, 1);
	spi_receive(&i, &n);
	endSPI();

	return i;
}

extern ERROR_t mcp2515_setConfigMode() {
	CANCTRL_t _canctrl;

	/* Modificamos el modo del trabajo del modulo */
	/**
	 * El modulo puede trabajar en 5 modos distintos.
	 * 		1. Modo de configuracion.	--> En particular trabajamos en este durante la incializacion.
	 * 		2. Modo normal.
	 * 		3. Modo sleep.
	 * 		4. Modo de solo escucha.
	 * 		5. Modo de loopback.
	 * */
	_canctrl.data = 0; /*< Nos aseguramos de que se encuentra limpia.*/
	_canctrl.REQ0P = 0b100; /*< El modulo entra en modo de configuracion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setListenOnlyMode() {
	CANCTRL_t _canctrl = { 0 };

	_canctrl.REQ0P = 0b011; /*< Modo de solo escucha.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setSleepMode() {
	CANCTRL_t _canctrl = { 0 };

	_canctrl.REQ0P = 0b001; /*< Modo sleep de operacion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setLoopbackMode() {
	CANCTRL_t _canctrl = { 0 };

	_canctrl.REQ0P = 0b010; /*< Modo loopback de operacion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setNormalMode() {
	CANCTRL_t _canctrl = { 0 };

	_canctrl.REQ0P = 0b000; /*< Modo normal de operacion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setMode(const uint8_t mode) {
	const REGISTER_t _register = MCP_CANCTRL;
	ModifyReg_t _modifyReg;

	/* Configura el modo de operacion del modulo */
	_modifyReg.reg = _register; /*< Registro de can control.*/
	_modifyReg.mask = CANCTRL_REQOP; /*< Corresponde a REQ0P[2:0].*/
	_modifyReg.data = mode; /*< Modo de operacion del modulo.*/

	mask = mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask,
			_modifyReg.data);

	/* Verifica que se cargo correctamente */
	__delay_ms(10);

	bool modeMatch = false;
	ReadReg_t _readReg;

	_readReg.reg = MCP_CANSTAT;
	mcp2515_readRegister(&_readReg);

	_readReg.data &= CANSTAT_OPMOD;
	modeMatch = (_readReg.data == mode);

	return modeMatch ? ERROR_OK : ERROR_FAIL;
}

extern ERROR_t mcp2515_setBitrate(const CAN_SPEED canSpeed, CAN_CLOCK canClock) {
	/* Entra en modo de configuracion */
	ERROR_t error = mcp2515_setConfigMode();

	if (error != ERROR_OK) {
		return error;
	}

	/* Elije la frecuencia y bits por segundos */
	/**
	 * Por ejemplo: 16 Mhz y 125 kBps.
	 * */
	uint8_t set, cfg1, cfg2, cfg3;

	set = 1;

	switch (canClock) {
	case (MCP_8MHZ):
		switch (canSpeed) {
		case (CAN_5KBPS):                                             //   5KBPS
			cfg1 = MCP_8MHz_5kBPS_CFG1;
			cfg2 = MCP_8MHz_5kBPS_CFG2;
			cfg3 = MCP_8MHz_5kBPS_CFG3;
			break;

		case (CAN_10KBPS):                                            //  10KBPS
			cfg1 = MCP_8MHz_10kBPS_CFG1;
			cfg2 = MCP_8MHz_10kBPS_CFG2;
			cfg3 = MCP_8MHz_10kBPS_CFG3;
			break;

		case (CAN_20KBPS):                                            //  20KBPS
			cfg1 = MCP_8MHz_20kBPS_CFG1;
			cfg2 = MCP_8MHz_20kBPS_CFG2;
			cfg3 = MCP_8MHz_20kBPS_CFG3;
			break;

		case (CAN_31K25BPS):                                       //  31.25KBPS
			cfg1 = MCP_8MHz_31k25BPS_CFG1;
			cfg2 = MCP_8MHz_31k25BPS_CFG2;
			cfg3 = MCP_8MHz_31k25BPS_CFG3;
			break;

		case (CAN_33KBPS):                                        //  33.333KBPS
			cfg1 = MCP_8MHz_33k3BPS_CFG1;
			cfg2 = MCP_8MHz_33k3BPS_CFG2;
			cfg3 = MCP_8MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS):                                            //  40Kbps
			cfg1 = MCP_8MHz_40kBPS_CFG1;
			cfg2 = MCP_8MHz_40kBPS_CFG2;
			cfg3 = MCP_8MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS):                                            //  50Kbps
			cfg1 = MCP_8MHz_50kBPS_CFG1;
			cfg2 = MCP_8MHz_50kBPS_CFG2;
			cfg3 = MCP_8MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS):                                            //  80Kbps
			cfg1 = MCP_8MHz_80kBPS_CFG1;
			cfg2 = MCP_8MHz_80kBPS_CFG2;
			cfg3 = MCP_8MHz_80kBPS_CFG3;
			break;

		case (CAN_100KBPS):                                           // 100Kbps
			cfg1 = MCP_8MHz_100kBPS_CFG1;
			cfg2 = MCP_8MHz_100kBPS_CFG2;
			cfg3 = MCP_8MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS):                                           // 125Kbps
			cfg1 = MCP_8MHz_125kBPS_CFG1;
			cfg2 = MCP_8MHz_125kBPS_CFG2;
			cfg3 = MCP_8MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS):                                           // 200Kbps
			cfg1 = MCP_8MHz_200kBPS_CFG1;
			cfg2 = MCP_8MHz_200kBPS_CFG2;
			cfg3 = MCP_8MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS):                                           // 250Kbps
			cfg1 = MCP_8MHz_250kBPS_CFG1;
			cfg2 = MCP_8MHz_250kBPS_CFG2;
			cfg3 = MCP_8MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS):                                           // 500Kbps
			cfg1 = MCP_8MHz_500kBPS_CFG1;
			cfg2 = MCP_8MHz_500kBPS_CFG2;
			cfg3 = MCP_8MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS):                                          //   1Mbps
			cfg1 = MCP_8MHz_1000kBPS_CFG1;
			cfg2 = MCP_8MHz_1000kBPS_CFG2;
			cfg3 = MCP_8MHz_1000kBPS_CFG3;
			break;

		default:
			set = 0;
			break;
		}
		break;

	case (MCP_16MHZ):
		switch (canSpeed) {
		case (CAN_5KBPS):                                             //   5Kbps
			cfg1 = MCP_16MHz_5kBPS_CFG1;
			cfg2 = MCP_16MHz_5kBPS_CFG2;
			cfg3 = MCP_16MHz_5kBPS_CFG3;
			break;

		case (CAN_10KBPS):                                            //  10Kbps
			cfg1 = MCP_16MHz_10kBPS_CFG1;
			cfg2 = MCP_16MHz_10kBPS_CFG2;
			cfg3 = MCP_16MHz_10kBPS_CFG3;
			break;

		case (CAN_20KBPS):                                            //  20Kbps
			cfg1 = MCP_16MHz_20kBPS_CFG1;
			cfg2 = MCP_16MHz_20kBPS_CFG2;
			cfg3 = MCP_16MHz_20kBPS_CFG3;
			break;

		case (CAN_33KBPS):                                        //  33.333Kbps
			cfg1 = MCP_16MHz_33k3BPS_CFG1;
			cfg2 = MCP_16MHz_33k3BPS_CFG2;
			cfg3 = MCP_16MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS):                                            //  40Kbps
			cfg1 = MCP_16MHz_40kBPS_CFG1;
			cfg2 = MCP_16MHz_40kBPS_CFG2;
			cfg3 = MCP_16MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS):                                            //  50Kbps
			cfg1 = MCP_16MHz_50kBPS_CFG1;
			cfg2 = MCP_16MHz_50kBPS_CFG2;
			cfg3 = MCP_16MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS):                                            //  80Kbps
			cfg1 = MCP_16MHz_80kBPS_CFG1;
			cfg2 = MCP_16MHz_80kBPS_CFG2;
			cfg3 = MCP_16MHz_80kBPS_CFG3;
			break;

		case (CAN_83K3BPS):                                       //  83.333Kbps
			cfg1 = MCP_16MHz_83k3BPS_CFG1;
			cfg2 = MCP_16MHz_83k3BPS_CFG2;
			cfg3 = MCP_16MHz_83k3BPS_CFG3;
			break;

		case (CAN_95KBPS):                                            //  95Kbps
			cfg1 = MCP_16MHz_95kBPS_CFG1;
			cfg2 = MCP_16MHz_95kBPS_CFG2;
			cfg3 = MCP_16MHz_95kBPS_CFG3;
			break;

		case (CAN_100KBPS):                                           // 100Kbps
			cfg1 = MCP_16MHz_100kBPS_CFG1;
			cfg2 = MCP_16MHz_100kBPS_CFG2;
			cfg3 = MCP_16MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS):                                           // 125Kbps
			cfg1 = MCP_16MHz_125kBPS_CFG1;
			cfg2 = MCP_16MHz_125kBPS_CFG2;
			cfg3 = MCP_16MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS):                                           // 200Kbps
			cfg1 = MCP_16MHz_200kBPS_CFG1;
			cfg2 = MCP_16MHz_200kBPS_CFG2;
			cfg3 = MCP_16MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS):                                           // 250Kbps
			cfg1 = MCP_16MHz_250kBPS_CFG1;
			cfg2 = MCP_16MHz_250kBPS_CFG2;
			cfg3 = MCP_16MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS):                                           // 500Kbps
			cfg1 = MCP_16MHz_500kBPS_CFG1;
			cfg2 = MCP_16MHz_500kBPS_CFG2;
			cfg3 = MCP_16MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS):                                          //   1Mbps
			cfg1 = MCP_16MHz_1000kBPS_CFG1;
			cfg2 = MCP_16MHz_1000kBPS_CFG2;
			cfg3 = MCP_16MHz_1000kBPS_CFG3;
			break;

		default:
			set = 0;
			break;
		}
		break;

	case (MCP_20MHZ):
		switch (canSpeed) {
		case (CAN_33KBPS):                                        //  33.333Kbps
			cfg1 = MCP_20MHz_33k3BPS_CFG1;
			cfg2 = MCP_20MHz_33k3BPS_CFG2;
			cfg3 = MCP_20MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS):                                            //  40Kbps
			cfg1 = MCP_20MHz_40kBPS_CFG1;
			cfg2 = MCP_20MHz_40kBPS_CFG2;
			cfg3 = MCP_20MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS):                                            //  50Kbps
			cfg1 = MCP_20MHz_50kBPS_CFG1;
			cfg2 = MCP_20MHz_50kBPS_CFG2;
			cfg3 = MCP_20MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS):                                            //  80Kbps
			cfg1 = MCP_20MHz_80kBPS_CFG1;
			cfg2 = MCP_20MHz_80kBPS_CFG2;
			cfg3 = MCP_20MHz_80kBPS_CFG3;
			break;

		case (CAN_83K3BPS):                                       //  83.333Kbps
			cfg1 = MCP_20MHz_83k3BPS_CFG1;
			cfg2 = MCP_20MHz_83k3BPS_CFG2;
			cfg3 = MCP_20MHz_83k3BPS_CFG3;
			break;

		case (CAN_100KBPS):                                           // 100Kbps
			cfg1 = MCP_20MHz_100kBPS_CFG1;
			cfg2 = MCP_20MHz_100kBPS_CFG2;
			cfg3 = MCP_20MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS):                                           // 125Kbps
			cfg1 = MCP_20MHz_125kBPS_CFG1;
			cfg2 = MCP_20MHz_125kBPS_CFG2;
			cfg3 = MCP_20MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS):                                           // 200Kbps
			cfg1 = MCP_20MHz_200kBPS_CFG1;
			cfg2 = MCP_20MHz_200kBPS_CFG2;
			cfg3 = MCP_20MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS):                                           // 250Kbps
			cfg1 = MCP_20MHz_250kBPS_CFG1;
			cfg2 = MCP_20MHz_250kBPS_CFG2;
			cfg3 = MCP_20MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS):                                           // 500Kbps
			cfg1 = MCP_20MHz_500kBPS_CFG1;
			cfg2 = MCP_20MHz_500kBPS_CFG2;
			cfg3 = MCP_20MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS):                                          //   1Mbps
			cfg1 = MCP_20MHz_1000kBPS_CFG1;
			cfg2 = MCP_20MHz_1000kBPS_CFG2;
			cfg3 = MCP_20MHz_1000kBPS_CFG3;
			break;

		default:
			set = 0;
			break;
		}
		break;

	default:
		set = 0;
		break;
	}

	CNF1_t _cnf1;
	CNF2_t _cnf2;
	CNF3_t _cnf3;

	_cnf1.data = cfg1;
	_cnf2.data = cfg2;
	_cnf3.data = cfg3;

	if (set) {
		mcp2515_setRegister(MCP_CNF1, _cnf1.data);
		mcp2515_setRegister(MCP_CNF2, _cnf2.data);
		mcp2515_setRegister(MCP_CNF3, _cnf3.data);
		return ERROR_OK;
	} else {
		return ERROR_FAIL;
	}
}

extern ERROR_t mcp2515_setClkOut(const CAN_CLKOUT divisor) {
	if (divisor == CLKOUT_DISABLE) {
		/* Turn off CLKEN */
		mcp2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKEN, 0x00);

		/* Turn on CLKOUT for SOF */
		mcp2515_modifyRegister(MCP_CNF3, CNF3_SOF, CNF3_SOF);
		return ERROR_OK;
	}

	/* Set the prescaler (CLKPRE) */
	mcp2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKPRE, divisor);

	/* Turn on CLKEN */
	mcp2515_modifyRegister(MCP_CANCTRL, CANCTRL_CLKEN, CANCTRL_CLKEN);

	/* Turn off CLKOUT for SOF */
	mcp2515_modifyRegister(MCP_CNF3, CNF3_SOF, 0x00);

	return ERROR_OK;
}

extern void mcp2515_prepareId(uint8_t *buffer, const bool ext,
		const uint32_t id) {
	uint16_t canid = (uint16_t) (id & 0x0FFFF);

	if (ext) {
		buffer[MCP_EID0] = (uint8_t) (canid & 0xFF);
		buffer[MCP_EID8] = (uint8_t) (canid >> 8);
		canid = (uint16_t) (id >> 16);
		buffer[MCP_SIDL] = (uint8_t) (canid & 0x03);
		buffer[MCP_SIDL] += (uint8_t) ((canid & 0x1C) << 3);
		buffer[MCP_SIDL] |= TXB_EXIDE_MASK;
		buffer[MCP_SIDH] = (uint8_t) (canid >> 5);
	} else {
		buffer[MCP_SIDH] = (uint8_t) (canid >> 3);
		buffer[MCP_SIDL] = (uint8_t) ((canid & 0x07) << 5);
		buffer[MCP_EID0] = 0;
		buffer[MCP_EID8] = 0;
	}

	return;
}

extern ERROR_t mcp2515_setFilterMask(const MASK mask, const bool ext,
		const uint32_t ulData) {
	/* Setea al modulo en modo de configuracion */
	ERROR_t res = mcp2515_setConfigMode();

	if (res != ERROR_OK) {
		return res;
	}

	uint8_t tbufdata[4];

	mcp2515_prepareId(tbufdata, ext, ulData);

	REGISTER_t reg;

	switch (mask) {
	case MASK0:
		reg = MCP_RXM0SIDH;
		break;
	case MASK1:
		reg = MCP_RXM1SIDH;
		break;
	default:
		return ERROR_FAIL;
	}

	/* Limpia los registros de la mascara */
	/**
	 * Tenemos los siguiente registros para la mascara para RX0:
	 * 		1. RXM0SIDH.
	 * 		2. RXM0SIDL.
	 * 		3. RXM0EID8.
	 * 		4. RXM0EID0.
	 * Para RX1 lo mismo.
	 * */
	mcp2515_setRegisters(reg, tbufdata, 4);

	return ERROR_OK;
}

extern ERROR_t mcp2515_setFilter(const RXF num, const bool ext,
		const uint32_t ulData) {
	/* Configura el modo de configuracion */
	ERROR_t res = mcp2515_setConfigMode();

	if (res != ERROR_OK) {
		return res;
	}

	REGISTER_t reg;

	switch (num) {
	case RXF0:
		reg = MCP_RXF0SIDH;
		break;
	case RXF1:
		reg = MCP_RXF1SIDH;
		break;
	case RXF2:
		reg = MCP_RXF2SIDH;
		break;
	case RXF3:
		reg = MCP_RXF3SIDH;
		break;
	case RXF4:
		reg = MCP_RXF4SIDH;
		break;
	case RXF5:
		reg = MCP_RXF5SIDH;
		break;
	default:
		return ERROR_FAIL;
	}

	uint8_t tbufdata[4]; /*< Utilizado para modificar los registros sid y eid del modulo.*/

	mcp2515_prepareId(tbufdata, ext, ulData); /*< Configuramos el id de acuerdo al formato extendido o no.*/
	mcp2515_setRegisters(reg, tbufdata, 4); /*< Cargamos el id configurado.*/

	return ERROR_OK;
}

extern ERROR_t mcp2515_sendMessage(const TXBn txbn,
		const struct can_frame *frame) {
	/* Verifica que no se supere la cantidad maxima de datos */
	if (frame->can_dlc > CAN_MAX_DLEN) {
		return ERROR_FAILTX;
	}

	/* Envia la informacion */
	const struct TXBn_REGS *txbuf = &TXB[txbn];

	uint8_t data[13];

	bool ext = (frame->can_id & CAN_EFF_FLAG);
	bool rtr = (frame->can_id & CAN_RTR_FLAG);
	uint32_t id = (frame->can_id & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

	mcp2515_prepareId(data, ext, id);

	data[MCP_DLC] = rtr ? (frame->can_dlc | RTR_MASK) : frame->can_dlc;

	memcpy(&data[MCP_DATA], frame->data, frame->can_dlc);

	mcp2515_setRegisters(txbuf->SIDH, data, 5 + frame->can_dlc);

	mcp2515_modifyRegister(txbuf->CTRL, TXB_TXREQ, TXB_TXREQ);

	/* Verifica la informacion enviada */
	ReadReg_t _readReg;

	_readReg.reg = *txbuf->CTRL;

	mcp2515_readRegister(&_readReg);

	if ((_readReg.data & (TXB_ABTF | TXB_MLOA | TXB_TXERR)) != 0) {
		return ERROR_FAILTX;
	}

	return ERROR_OK;
}

//extern ERROR_t mcp2515_sendMessage(const struct can_frame *frame) {
//	if (frame->can_dlc > CAN_MAX_DLEN) {
//		return ERROR_FAILTX;
//	}
//
//	TXBn txBuffers[N_TXBUFFERS] = { TXB0, TXB1, TXB2 };
//
//	for (int i = 0; i < N_TXBUFFERS; i++) {
//		const struct TXBn_REGS *txbuf = &TXB[txBuffers[i]];
//		uint8_t ctrlval = mcp2515_readRegister(txbuf->CTRL);
//		if ((ctrlval & TXB_TXREQ) == 0) {
//			return mcp2515_sendMessage(txBuffers[i], frame);
//		}
//	}
//
//	return ERROR_ALLTXBUSY;
//}

extern ERROR_t mcp2515_readMessage(const RXBn rxbn, struct can_frame *frame) {
	const struct RXBn_REGS *rxb = &RXB[rxbn];

	uint8_t tbufdata[5];

	mcp2515_readRegisters(rxb->SIDH, tbufdata, 5);

	uint32_t id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

	if ((tbufdata[MCP_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK) {
		id = (id << 2) + (tbufdata[MCP_SIDL] & 0x03);
		id = (id << 8) + tbufdata[MCP_EID8];
		id = (id << 8) + tbufdata[MCP_EID0];
		id |= CAN_EFF_FLAG;
	}

	uint8_t dlc = (tbufdata[MCP_DLC] & DLC_MASK);
	if (dlc > CAN_MAX_DLEN) {
		return ERROR_FAIL;
	}

	uint8_t ctrl = mcp2515_readRegister(rxb->CTRL);
	if (ctrl & RXBnCTRL_RTR) {
		id |= CAN_RTR_FLAG;
	}

	frame->can_id = id;
	frame->can_dlc = dlc;

	mcp2515_readRegisters(rxb->DATA, frame->data, dlc);

	mcp2515_modifyRegister(MCP_CANINTF, rxb->CANINTF_RXnIF, 0);

	return ERROR_OK;
}

extern ERROR_t mcp2515_readMessage(struct can_frame *frame) {
	ERROR_t rc;
	uint8_t stat = mcp2515_getStatus();

	if (stat & STAT_RX0IF) {
		rc = mcp2515_readMessage(RXB0, frame);
	} else if (stat & STAT_RX1IF) {
		rc = mcp2515_readMessage(RXB1, frame);
	} else {
		rc = ERROR_NOMSG;
	}

	return rc;
}

extern bool mcp2515_checkReceive(void) {
	uint8_t res = mcp2515_getStatus();
	if (res & STAT_RXIF_MASK) {
		return true;
	} else {
		return false;
	}
}

extern bool mcp2515_checkError(void) {
	uint8_t eflg = mcp2515_getErrorFlags();

	if (eflg & EFLG_ERRORMASK) {
		return true;
	} else {
		return false;
	}
}

extern uint8_t mcp2515_getErrorFlags(void) {
	return mcp2515_readRegister(MCP_EFLG);
}

extern void mcp2515_clearRXnOVRFlags(void) {
	mcp2515_modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);

	return;
}

extern uint8_t mcp2515_getInterrupts(void) {
	return mcp2515_readRegister(MCP_CANINTF);
}

extern void mcp2515_clearInterrupts(void) {
	mcp2515_setRegister(MCP_CANINTF, 0);

	return;
}

extern uint8_t mcp2515_getInterruptMask(void) {
	return mcp2515_readRegister(MCP_CANINTE);
}

extern void mcp2515_clearTXInterrupts(void) {
	mcp2515_modifyRegister(MCP_CANINTF,
			(CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF), 0);

	return;
}

extern void mcp2515_clearRXnOVR(void) {
	uint8_t eflg = mcp2515_getErrorFlags();
	if (eflg != 0) {
		mcp2515_clearRXnOVRFlags();
		mcp2515_clearInterrupts();
		//modifyRegister(MCP_CANINTF, CANINTF_ERRIF, 0);
	}

	return;
}

extern void mcp2515_clearMERR() {
	//modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
	//clearInterrupts();
	mcp2515_modifyRegister(MCP_CANINTF, CANINTF_MERRF, 0);

	return;
}

extern void mcp2515_clearERRIF() {
	//modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
	//clearInterrupts();
	mcp2515_modifyRegister(MCP_CANINTF, CANINTF_ERRIF, 0);
}

extern uint8_t mcp2515_errorCountRX(void) {
	return mcp2515_readRegister(MCP_REC);
}

extern uint8_t mcp2515_errorCountTX(void) {
	return mcp2515_readRegister(MCP_TEC);
}
