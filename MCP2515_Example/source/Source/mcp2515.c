/**
 * @file mcp2515.c
 * @brief Libreria para manipulacion del modulo can mcp2515.
 * @author Zuliani, Agustin.
 * @date 03/08/24
 *
 * @note Para utilizar dicha libreria debera incluirse de antemano
 * la libreria correspondiente del modulo spi para trabajar con la
 * kl46z. En adicion esa libreria debera corresponder a la que permite
 * el uso de freertos si fuera el caso. Para nuestro caso utilizamos
 * spi.h y spi.c para trabajar.
 */

#include "Includes/mcp2515.h"
#include "Includes/spi.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_port.h"
#include <string.h>

/*
 * Pines del spi.
 * Se defienen los siguiente pines para el módulo spi de la kl46z.
 * 	MISO: PTE19
 * 	MOSI: PTE18
 * 	CS:	  PTE16
 * 	SCK:  PTE17
 * */

/**
 * @def Chip select en estado alto
 * @brief Pone en alto el pin de chip select. Esto hace
 * que se finilize la comunicacion con el dispositivo.
 */
#define CS_HIGH GPIO_SetPinsOutput((GPIO_Type *)GPIOE_BASE, BOARD_Cs_mcp2515_PIN_MASK)
/**
 * @def Chip select en estado bajo
 * @brief Pone en bajo el pin de chip select. Esto hace
 * que se inicie la comunicacion con el dispositivo.
 */
#define CS_LOW GPIO_ClearPinsOutput((GPIO_Type *)GPIOE_BASE, BOARD_Cs_mcp2515_PIN_MASK)

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

/**
 * @brief Registro de configuracion 1
 *
 * Utilizado para manejar a nivel de bits el registro
 * de configuracion 1 del modulo.
 */
typedef union
{
	struct
	{
		/**
		 * @brief Baud rate prescaler bits
		 * */
		unsigned BRP : 6;
		/**
		 * @brief Synchronization jump width lenght bits
		 * */
		unsigned SJW : 2;
	};
	uint8_t data;
} CNF1_t;

/**
 * @brief Registro de configuracion 2
 */
typedef union
{
	struct
	{
		/**
		 * @brief Propagation segment lenght bits
		 * */
		unsigned PRSEG : 3;
		/**
		 * @brief PS1 lenght bits
		 */
		unsigned PHSEG : 3;
		/**
		 * @brief Sample point configuration bits
		 */
		unsigned SAM : 1;
		/**
		 * @brief PS2 bit time length bit
		 */
		unsigned BTLMODE : 1;
	};
	uint8_t data;
} CNF2_t;

/**
 * @brief Registro de configuracion 3
 */
typedef union
{
	struct
	{
		/**
		 * @brief PS2 length bits
		 */
		unsigned PHSEG2 : 3;
		/**
		 * @brief Reservados
		 */
		unsigned RESERVED : 3;
		/**
		 * @brief Wake-up filter bit
		 */
		unsigned WAKFIL : 1;
		/**
		 * @brief Start-of-frame signal bit
		 */
		unsigned SOF : 1;
	};
	uint8_t data;
} CNF3_t;

/**
 * @brief Registro de control de transimision
 */
typedef union
{
	struct
	{
		/** @brief transmit buffer priority bits */
		unsigned TXP : 2;
		unsigned RESERVED2 : 1;
		/** @brief message transmit request bit */
		unsigned TXREQ : 1;
		/** @brief transmission error detected bit */
		unsigned TXERR : 1;
		/** @brief message lost arbitration bit */
		unsigned MLOA : 1;
		/** @brief message aborted flag bit */
		unsigned ABTF : 1;
		unsigned RESERVED : 1;
	};
	uint8_t data;
} TXBnCTRL_t;

/**
 * @brief Registro de interrupciones
 */
typedef union
{
	struct
	{
		/** @brief Receive buffer 0 full interrupt enable bit */
		unsigned RX0IE : 1;
		/** @brief Receive buffer 1 full interrupt enable bit */
		unsigned RX1IE : 1;
		/** @brief Transmit buffer 0 empty interrupt enable bit */
		unsigned TX0IE : 1;
		/** @brief Transmit buffer 1 empty interrupt enable bit */
		unsigned TX1IE : 1;
		/** @brief Transmit buffer 2 empty interrupt enable bit */
		unsigned TX2IE : 1;
		/** @brief Error interrupt enable bit */
		unsigned ERRIE : 1;
		/** @brief Wake-up interrupt enable */
		unsigned WAKIE : 1;
		/** @brief Message error interrupt enable */
		unsigned MERRE : 1;
	};
	uint8_t data;
} CANINTE_t;

/**
 * @brief Registro CAN INTERRUPT FLAG
 */
typedef union
{
	struct
	{
		/** @brief Receive buffer 0 full interrupt flag bit */
		unsigned RX0IF : 1;
		/** @brief Receive buffer 1 full interrupt flag bit */
		unsigned RX1IF : 1;
		/** @brief Transmit buffer 0 empty interrupt flag bit */
		unsigned TX0IF : 1;
		/** @brief Transmit buffer 1 empty interrupt flag bit */
		unsigned TX1IF : 1;
		/** @brief Transmit buffer 2 empty interrupt flag bit */
		unsigned TX2IF : 1;
		/** @brief Error interrupt flag bit */
		unsigned ERRIF : 1;
		/** @brief Wake-up interrupt flag bit */
		unsigned WAKIF : 1;
		/** @brief Message error interrupt flag bit */
		unsigned MERRF : 1;
	};
	uint8_t data;
} CANINTF_t;

/**
 * @brief Registro de control de can
 */
typedef union
{
	struct
	{
		/**
		 * @brief CLKOUT pin prescaler bits
		 */
		unsigned CLKPRE : 2;
		/**
		 * @brief CLKOUT pin enable bit
		 */
		unsigned CLKEN : 1;
		/**
		 * @brief One-shot mode bit
		 */
		unsigned OSM : 1;
		/**
		 * @brief Abort all pending transmissions bit
		 */
		unsigned ABAT : 1;
		/**
		 * @brief Request operation mode bits
		 */
		unsigned REQ0P : 3;
	};
	uint8_t data;
} CANCTRL_t;

/**
 * @brief Registro ERROR FLAG
 */
typedef union
{
	struct
	{
		/** @brief error warning flag */
		unsigned EWARN : 1;
		/** @brief receive error warning flag */
		unsigned RXWAR : 1;
		/** @brief transmit error warning flag */
		unsigned TXWAR : 1;
		/** @brief receive error-passive flag */
		unsigned RXEP : 1;
		/** @brief transmit error-passive flag */
		unsigned TXEP : 1;
		/** @brief bus-off error flag */
		unsigned TXBO : 1;
		/** @brief Receive buffer 0 overflow */
		unsigned RX0OVR : 1;
		/** @brief Receive buffer 1 overflow */
		unsigned RX1OVR : 1;
	};
	uint8_t data;

} EFLG_t;

/**
 * @brief Registro de control de rxb0
 */
typedef union
{
	struct
	{
		/**
		 * @brief Filter hit bit
		 */
		unsigned FILHIT0 : 1;
		/**
		 * @brief Read-only copy of BUKT bit
		 */
		unsigned BUKT1 : 1;
		/**
		 * @brief Rollver enable bit
		 */
		unsigned BUKT : 1;
		/**
		 * @brief Received remote transfer request bit
		 */
		unsigned RXRTR : 1;
		unsigned RESERVED2 : 1;
		/**
		 * @brief Receive buffer operating mode bits
		 */
		unsigned RXM : 2;
		unsigned RESERVED : 1;
	};
	uint8_t data;
} RXB0CTRL_t;

/**
 * @brief Registro de control de rxb1
 */
typedef union
{
	struct
	{
		/**
		 * @brief Filter hit bits
		 */
		unsigned FILHIT : 3;
		/**
		 * @brief Received remote transfer request bit
		 */
		unsigned RXRTR : 1;
		unsigned RESERVED2 : 1;
		/**
		 * @brief Receive buffer operating mode bits
		 */
		unsigned RXM : 2;
		unsigned RESERVED : 1;
	};
	uint8_t data;
} RXB1CTRL_t;

/**
 * @brief Registro de pin control y estado
 */
typedef union
{
	struct
	{
		/**
		 * @brief ~{TX0RTS} pin mode bit
		 */
		unsigned B0RTSM : 1;
		/**
		 * @brief ~{TX1RTS} pin mode bit
		 */
		unsigned B1RTSM : 1;
		/**
		 * @brief ~{TX2RTS} pin mode bit
		 */
		unsigned B2RTSM : 1;
		/**
		 * @brief ~{TX0RTS} pin state bit
		 */
		unsigned B0RTS : 1;
		/**
		 * @brief ~{TX1RTS} pin state bit
		 */
		unsigned B1RTS : 1;
		/**
		 * @brief ~{TX2RTS} pin state bit
		 */
		unsigned B2RTS : 1;
		unsigned RESERVED2 : 1;
		unsigned RESERVED : 1;
	};
	uint8_t data;
} TXRTSCTRL_t;

/**
 * @brief Registro de id parte alta
 */
typedef union
{
	struct
	{
		/**
		 * @brief Standart identifier bits
		 */
		unsigned SID : 8;
	};
	uint8_t data;
} TXBnSIDH_t;

/**
 * @brief Registro de id parte baja
 */
typedef union
{
	struct
	{
		/**
		 * @brief Extended identifier bits
		 */
		unsigned EID : 2;
		unsigned RESERVED2 : 1;
		/**
		 * @brief Extended identifier enable bit
		 */
		unsigned EXIDE : 1;
		unsigned RESERVED : 1;
		/**
		 * @brief Standard identifier bits
		 */
		unsigned SID : 3;
	};
	uint8_t data;
} TXBnSIDL_t;

/**
 * @brief Registro de id extendida parte alta
 */
typedef union
{
	struct
	{
		/**
		 * @brief Extended identifier bits
		 */
		unsigned EID : 8;
	};
	uint8_t data;
} TXBnEID8_t;

/**
 * @brief Registro de id extendida parte baja
 */
typedef union
{
	struct
	{
		/**
		 * @brief Extended identifier bits
		 */
		unsigned EID : 8;
	};
	uint8_t data;
} TXBnEID0_t;

/**
 * @brief Registro de longitud de datos
 */
typedef union
{
	struct
	{
		/**
		 * @brief Data length code bits
		 */
		unsigned DLC : 4;
		unsigned RESERVED2 : 2;
		/**
		 * @brief Remote transmission request bit
		 */
		unsigned RTR : 1;
		unsigned RESERVED : 1;
	};
	uint8_t data;
} TXBnDLC_t;

/**
 * @brief Registro de datos
 */
typedef union
{
	struct
	{
		/**
		 * @brief Transmit buffer n data field byte m bits
		 */
		unsigned TXBnDm : 8;
	};
	uint8_t data;
} TXBnDm_t;

/**
 * @brief Modificar registro
 *
 * Se carga todo lo necesario en los campos para modificar
 * en el registro determinado. Se debe cargar el registro, la mascara
 * y el dato nuevo.
 */
typedef struct
{
	/**
	 * @brief Nombre del registro a modificar.
	 */
	const REGISTER_t reg;
	/**
	 * @brief Mascara.
	 */
	uint8_t mask;
	/**
	 * @brief Dato a modificar.
	 */
	const uint8_t data;
} ModifyReg_t;

/**
 * @brief Registros de lectura de datos
 *
 * Utilizada antes de leer sobre un registro en particular.
 * */
typedef struct
{
	/** @brief Registro a leer. */
	REGISTER_t reg;
	/** @brief Instruccion de lectura. */
	INSTRUCTION_t instruction;
	/** @brief Datos a cargar. */
	uint8_t data;
} ReadReg_t;

#define MAX_DATA_readREG 10

/**
 * @brief Estructura para la lectura de registros.
 *
 * Esta estructura se utiliza para leer registros especificos
 * con una instruccion asociada y almacenar los datos leidos.
 */
typedef struct
{
	/** @brief Registro en el que se lee.*/
	REGISTER_t reg;
	/** @brief Instruccion.*/
	INSTRUCTION_t instruction;
	/** @brief Datos leidos.*/
	uint8_t values[MAX_DATA_readREG];
	/** @brief Longitud de la cadena.*/
	uint8_t n;
} ReadRegs_t;

#if USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"

#define __delay_ms(ms) vTaskDelay(pdMS_TO_TICKS(ms))
#elif (!USE_FREERTOS)
#define __delay_ms(x) delay_ms(x)

static void delay_ms(uint16_t ms);

static void delay_ms(uint16_t ms)
{
	// Calcula el número de ciclos necesarios
	uint32_t cycles = (CLOCK_GetCoreSysClkFreq() / 1000) * ms / 4;

	// Realiza el bucle para generar el retardo
	for (uint32_t i = 0; i < cycles; i++)
	{
		__NOP(); // No Operation (1 ciclo de instrucción)
	}

	return;
}
#endif

static const uint8_t CANCTRL_REQOP = 0xE0;
// static const uint8_t CANCTRL_ABAT = 0x10;
// static const uint8_t CANCTRL_OSM = 0x08;
static const uint8_t CANCTRL_CLKEN = 0x04;
static const uint8_t CANCTRL_CLKPRE = 0x03;

static const uint8_t CANSTAT_OPMOD = 0xE0;
// static const uint8_t CANSTAT_ICOD = 0x0E;

static const uint8_t CNF3_SOF = 0x80;

static const uint8_t TXB_EXIDE_MASK = 0x08;
static const uint8_t DLC_MASK = 0x0F;
static const uint8_t RTR_MASK = 0x40;

// static const uint8_t RXBnCTRL_RXM_STD = 0x20;
// static const uint8_t RXBnCTRL_RXM_EXT = 0x40;
static const uint8_t RXBnCTRL_RXM_STDEXT = 0x00;
static const uint8_t RXBnCTRL_RXM_MASK = 0x60;
static const uint8_t RXBnCTRL_RTR = 0x08;
static const uint8_t RXB0CTRL_BUKT = 0x04;
static const uint8_t RXB0CTRL_FILHIT_MASK = 0x03;
static const uint8_t RXB1CTRL_FILHIT_MASK = 0x07;
// static const uint8_t RXB0CTRL_FILHIT = 0x00;
static const uint8_t RXB1CTRL_FILHIT = 0x01;

static const uint8_t MCP_SIDH = 0;
static const uint8_t MCP_SIDL = 1;
static const uint8_t MCP_EID8 = 2;
static const uint8_t MCP_EID0 = 3;
static const uint8_t MCP_DLC = 4;
static const uint8_t MCP_DATA = 5;

static const uint8_t STAT_RXIF_MASK = STAT_RX0IF | STAT_RX1IF;

static const uint8_t EFLG_ERRORMASK = EFLG_RX1OVR | EFLG_RX0OVR | EFLG_TXBO | EFLG_TXEP | EFLG_RXEP;

#define N_TXBUFFERS 3
#define N_RXBUFFERS 2

static const struct TXBn_REGS
{
	REGISTER_t CTRL;
	REGISTER_t SIDH;
	REGISTER_t DATA;
} TXB[N_TXBUFFERS];

static const struct RXBn_REGS
{
	REGISTER_t CTRL;
	REGISTER_t SIDH;
	REGISTER_t DATA;
	CANINTF_enum CANINTF_RXnIF;
} RXB[N_RXBUFFERS];

/**
 * @brief Funciones privadas
 * @{
 */
/**
 * @brief Incia la comunicacion spi
 */
static void startSPI(void);
/**
 * @brief Finaliza la comunicacion spi
 */
static void endSPI(void);
/**
 * @brief Seteado el modo de trabajo.
 * @param[in] mode Modo de trabajo
 */
static ERROR_t mcp2515_setMode(const uint8_t mode);
/**
 * @brief Lee un solo registro a la vez
 * @param[in,out] _readReg Puntero al tipo de dato ReadReg_t
 * @return Devuelve el estado de la transferencia
 */
static ERROR_t mcp2515_readRegister(ReadReg_t *readReg);
/**
 * @brief Lee multiples registros a la vez
 * @param[in,out] _readRegs Puntero al tipo de dato
 * @return Devuelve el estado de la transferencia
 */
static ERROR_t mcp2515_readRegisters(ReadRegs_t *_readRegs);
/**
 * @brief Setea un registro
 * @param[in] reg registro
 * @param[in] value dato
 * @return Devuelve el estado de la transmision
 */
static ERROR_t mcp2515_setRegister(const REGISTER_t reg, const uint8_t value);
/**
 * @brief Setea multiples registros
 * @param[in] reg registro incial donde setear datos
 * @param[in] values datos
 * @param[in] n cantidad de datos
 * @return Devuelve el estado de la transmision
 */
static ERROR_t mcp2515_setRegisters(const REGISTER_t reg, const uint8_t values[],
								 const uint8_t n);
/**
 * @brief Modifica un registro en particular
 * @param[in] reg registro a modificar
 * @param[in] mask bits que se van a modificar
 * @param[in] data valor que se va a cargar
 * @return Devuelve el estado de la modificacion
 */
static ERROR_t mcp2515_modifyRegister(const REGISTER_t reg, const uint8_t mask,
								   const uint8_t data);
/**
 * @brief Configura el id para una accion en particular
 * @param[out] buffer lugar donde se va a cargar el resultado
 * @param[in] ext indica si es de formato extendido
 * @param[in] id valor del id
 */
static void mcp2515_prepareId(uint8_t *buffer, const bool ext,
							  const uint32_t id);
/**
 * @}
 */

// static const struct TXBn_REGS MCP2515_TXB[N_TXBUFFERS] = {
//	{MCP_TXB0CTRL, MCP_TXB0SIDH, MCP_TXB0DATA},
//	{MCP_TXB1CTRL, MCP_TXB1SIDH, MCP_TXB1DATA},
//	{MCP_TXB2CTRL, MCP_TXB2SIDH, MCP_TXB2DATA},
// };

static const struct RXBn_REGS RXB[N_RXBUFFERS] = {
	{MCP_RXB0CTRL, MCP_RXB0SIDH, MCP_RXB0DATA, CANINTF_RX0IF},
	{MCP_RXB1CTRL, MCP_RXB1SIDH, MCP_RXB1DATA, CANINTF_RX1IF},
};

extern void mcp2515_init(void)
{
	/*
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

	gpio_pin_config_t Cs_mcp2515_config = {.pinDirection = kGPIO_DigitalOutput,
										   .outputLogic = 1U};
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

	CS_HIGH; /* Deselecciona el modulo mcp2515.*/

	/*
	 * Configuracion del spi.
	 *
	 * Se configura el spi para trabajar con/sin freertos. Ver las configuraciones
	 * iniciales en la librería spi.h y spi.c. Acordarse de seter o limpiar USE_FREERTOS.
	 * */
	spi_init();

	return;
}

static void startSPI(void)
{
	/*
	 * Chip select bajo.
	 *
	 * Baja el chip select del modulo para seleccionarlo y luego
	 * leer o escribir.
	 * */
	CS_LOW;

	return;
}

static void endSPI(void)
{
	/*
	 * Chip select alto.
	 *
	 * Libera el bus del mcp2515.
	 * */
	CS_HIGH;

	return;
}

extern ERROR_t mcp2515_reset(void)
{
	INSTRUCTION_t instruction = INSTRUCTION_RESET;
	REGISTER_t _register;
	CANINTE_t _caninte;

	/* Reinicio del modulo mcp2515 */
	/*
	 * Debemos reiniciar el modulo para configurar los parametros
	 * necesarios antes de utilizarlo en el can bus.
	 * */
	startSPI();
	spi_write(&instruction, 1);
	endSPI();

	__delay_ms(200); /* Espera a que reinicie el modulo.*/

	uint8_t zeros[14];

	memset(zeros, 0, sizeof(zeros)); /* Inicia con un vector en 0.*/

	/* Resetea todos los registros seteandolos en 0 */
	/*
	 * Registros de control de TX.
	 * */
	_register = MCP_TXB0CTRL; /* Registro de control tx del buffer 0 */
	mcp2515_setRegisters(_register, zeros, 14);

	_register = MCP_TXB1CTRL; /* Registro de control tx del buffer 1 */
	mcp2515_setRegisters(_register, zeros, 14);

	_register = MCP_TXB2CTRL; /* Registro de control tx del buffer 2 */
	mcp2515_setRegisters(_register, zeros, 14);

	/* Registros de control de Rx. */
	_register = MCP_RXB0CTRL; /* Buffer 0 */
	mcp2515_setRegister(_register, 0);

	_register = MCP_RXB1CTRL; /* Buffer 1 */
	mcp2515_setRegister(_register, 0);

	/* Registro de interrupcion. */
	_register = MCP_CANINTE; /* Registro de habilitacion de interrupcion */
	_caninte.RX0IE = 1;
	_caninte.RX1IE = 1;
	_caninte.ERRIE = 1;
	_caninte.MERRE = 1;
	mcp2515_setRegister(_register, _caninte.data);

	/*
	 * Debemos verificar que se activaron con un read.
	 * */

	/* Configuro los registros de control de Rx */
	// receives all valid messages using either Standard or Extended Identifiers that
	// meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
	/* RXB0 control */
	RXB0CTRL_t _rxb0Ctrl = {0};
	RXB1CTRL_t _rxb1Ctrl = {0};

	_register = MCP_RXB0CTRL;
	_rxb0Ctrl.BUKT = 1; /* Permite el roll over del RXB0 al RXB1.*/

	ModifyReg_t _modifyRegRXB0 = {
		.reg = _register,
		.mask = RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
		.data = _rxb0Ctrl.data,
	};

	mcp2515_modifyRegister(_modifyRegRXB0.reg,	/* Envia el registro que va a modificar.*/
						   _modifyRegRXB0.mask, /* Envia la mascara.*/
						   _modifyRegRXB0.data /* Setea unicamente el BUKT(roll over).*/);

	/*
	 * Debe verificar que se cargo correctamente.
	 */

	/* RXB1 control */
	_register = MCP_RXB1CTRL;
	_rxb1Ctrl.data = RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT; /* FILHIT1 = 1.*/

	ModifyReg_t _modifyRegRXB1 = {
		.reg = _register,
		.mask = RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
		.data = _rxb1Ctrl.data,
	};

	mcp2515_modifyRegister(_modifyRegRXB1.reg, _modifyRegRXB1.mask, _modifyRegRXB1.data);

	// clear filters and masks
	// do not filter any standard frames for RXF0 used by RXB0
	// do not filter any extended frames for RXF1 used by RXB1

	/*
	 * Limpiamos los registros del id y mask.
	 *
	 * Seteamos en cero el filtro para el id standart y el extendido. Tambien
	 * seteamos en cero la mascara. Luego se configurara mas adelante el
	 * filtro y mascara correspondiente.
	 * */
	RXF filters[] = {RXF0, RXF1, RXF2, RXF3, RXF4, RXF5};

#define FILTER_CLEAR 0
#define MASK_CLEAR 0

	for (int i = 0; i < 6; i++)
	{
		bool ext = (i == 1);

		ERROR_t result = mcp2515_setFilter(filters[i], ext, FILTER_CLEAR);

		if (result != ERROR_OK)
		{
			return result;
		}
	}

	MASK masks[] = {MASK0, MASK1};

	for (int i = 0; i < 2; i++)
	{
		ERROR_t result = mcp2515_setFilterMask(masks[i], true, MASK_CLEAR);

		if (result != ERROR_OK)
		{
			return result;
		}
	}

	return ERROR_OK;
}

static ERROR_t mcp2515_readRegister(ReadReg_t *readReg)
{
	status_t status;

	readReg->instruction = INSTRUCTION_READ;

	startSPI();
	status = spi_write(&readReg->instruction, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write(&readReg->reg, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_receive(&readReg->data, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_READ;
	}
	endSPI();

	return ERROR_OK;
}

static ERROR_t mcp2515_readRegisters(ReadRegs_t *readRegs)
{
	status_t status;

	startSPI();
	status = spi_write(&readRegs->instruction, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write(&readRegs->reg, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_receive(readRegs->values, readRegs->n);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_READ;
	}
	endSPI();

	return ERROR_OK;
}

static ERROR_t mcp2515_setRegister(const REGISTER_t reg, const uint8_t value)
{
	status_t status;
	INSTRUCTION_t instruction = INSTRUCTION_WRITE;

	/* Envia los datos al modulo mediante spi */
	startSPI();
	status = spi_write(&instruction, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write((uint8_t *)&reg, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write((uint8_t *)&value, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}
	endSPI();

	return ERROR_OK;
}

static ERROR_t mcp2515_setRegisters(const REGISTER_t reg, const uint8_t values[],
								 const uint8_t n)
{
	status_t status;
	INSTRUCTION_t _instruction = INSTRUCTION_WRITE;

	/* Envia los datos al modulo mediante spi */
	startSPI();
	status = spi_write(&_instruction, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write((uint8_t *)&reg, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write((uint8_t *)values, n);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}
	endSPI();

	return ERROR_OK;
}

static ERROR_t mcp2515_modifyRegister(const REGISTER_t reg, const uint8_t mask,
								   const uint8_t data)
{
	ERROR_t error = ERROR_OK;
	INSTRUCTION_t _instruction = INSTRUCTION_BITMOD;
	status_t status;

	startSPI();
	status = spi_write(&_instruction, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}

	status = spi_write((uint8_t *)&reg, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}
	status = spi_write((uint8_t *)&mask, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}
	status = spi_write((uint8_t *)&data, 1);
	if (status != kStatus_Success){
		endSPI();
		return ERROR_SPI_WRITE;
	}
	endSPI();

	/* Verificamos si se modifico correctamente */
#define MAX_INTENTOS	3

	ReadReg_t readReg = {
		.instruction = INSTRUCTION_READ,
		.reg = reg,
		.data = 0,
	};

	for (uint8_t i = 0; i < MAX_INTENTOS; i++){
		error = mcp2515_readRegister(&readReg);
		if (error != ERROR_OK)
			return error;
		if ((readReg.data & mask) == data)
			return ERROR_OK;
	}

	return ERROR_MODIFY_REG;
}

extern uint8_t mcp2515_getStatus(void)
{
	INSTRUCTION_t _instruction = INSTRUCTION_READ_STATUS;
	uint8_t i, n = 1;

	startSPI();
	spi_write(&_instruction, 1);
	spi_receive(&i, n);
	endSPI();

	return i;
}

extern ERROR_t mcp2515_setConfigMode()
{
	CANCTRL_t canctrl = {0};

	/* Modificamos el modo del trabajo del modulo */
	/*
	 * El modulo puede trabajar en 5 modos distintos.
	 * 		1. Modo de configuracion.	--> En particular trabajamos en este durante la incializacion.
	 * 		2. Modo normal.
	 * 		3. Modo sleep.
	 * 		4. Modo de solo escucha.
	 * 		5. Modo de loopback.
	 * */
	canctrl.REQ0P = 0b100; /* El modulo entra en modo de configuracion.*/

	return mcp2515_setMode(canctrl.data);
}

extern ERROR_t mcp2515_setListenOnlyMode()
{
	CANCTRL_t canctrl = {0};

	canctrl.REQ0P = 0b011; /*< Modo de solo escucha.*/

	return mcp2515_setMode(canctrl.data);
}

extern ERROR_t mcp2515_setSleepMode()
{
	CANCTRL_t canctrl = {0};

	canctrl.REQ0P = 0b001; /*< Modo sleep de operacion.*/

	return mcp2515_setMode(canctrl.data);
}

extern ERROR_t mcp2515_setLoopbackMode()
{
	CANCTRL_t _canctrl = {0};

	_canctrl.REQ0P = 0b010; /*< Modo loopback de operacion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setNormalMode()
{
	CANCTRL_t _canctrl = {0};

	_canctrl.REQ0P = 0b000; /* Modo normal de operacion.*/

	return mcp2515_setMode(_canctrl.data);
}

extern ERROR_t mcp2515_setMode(const uint8_t mode)
{
	const REGISTER_t reg = MCP_CANCTRL;

	ModifyReg_t modifyReg = {
		.reg = reg,			   /* Registro de can control.*/
		.mask = CANCTRL_REQOP, /* Corresponde a REQ0P[2:0].*/
		.data = mode,		   /* Modo de operacion del modulo.*/
	};

	/* Configura el modo de operacion del modulo */
	mcp2515_modifyRegister(modifyReg.reg, modifyReg.mask,
						   modifyReg.data);

	/* Verifica que se cargo correctamente */
	__delay_ms(200);

	bool modeMatch = false;

	ReadReg_t readReg = {
		.instruction = INSTRUCTION_READ,
		.reg = MCP_CANSTAT,
		.data = 0,
	};

	mcp2515_readRegister(&readReg); /* Lee el registro para corroborar la informacion. */

	readReg.data &= CANSTAT_OPMOD;
	modeMatch = (readReg.data == mode);

	return modeMatch ? ERROR_OK : ERROR_FAIL;
}

extern ERROR_t mcp2515_setBitrate(const CAN_SPEED canSpeed, CAN_CLOCK canClock)
{
	/* Entra en modo de configuracion */
	ERROR_t error = mcp2515_setConfigMode();

	if (error != ERROR_OK)
	{
		return error;
	}

	/* Elije la frecuencia y bits por segundos */
	/*
	 * Por ejemplo: 16 Mhz y 125 kBps.
	 * */
	uint8_t set, cfg1, cfg2, cfg3;

	set = 1;

	switch (canClock)
	{
	case (MCP_8MHZ):
		switch (canSpeed)
		{
		case (CAN_5KBPS): //   5KBPS
			cfg1 = MCP_8MHz_5kBPS_CFG1;
			cfg2 = MCP_8MHz_5kBPS_CFG2;
			cfg3 = MCP_8MHz_5kBPS_CFG3;
			break;

		case (CAN_10KBPS): //  10KBPS
			cfg1 = MCP_8MHz_10kBPS_CFG1;
			cfg2 = MCP_8MHz_10kBPS_CFG2;
			cfg3 = MCP_8MHz_10kBPS_CFG3;
			break;

		case (CAN_20KBPS): //  20KBPS
			cfg1 = MCP_8MHz_20kBPS_CFG1;
			cfg2 = MCP_8MHz_20kBPS_CFG2;
			cfg3 = MCP_8MHz_20kBPS_CFG3;
			break;

		case (CAN_31K25BPS): //  31.25KBPS
			cfg1 = MCP_8MHz_31k25BPS_CFG1;
			cfg2 = MCP_8MHz_31k25BPS_CFG2;
			cfg3 = MCP_8MHz_31k25BPS_CFG3;
			break;

		case (CAN_33KBPS): //  33.333KBPS
			cfg1 = MCP_8MHz_33k3BPS_CFG1;
			cfg2 = MCP_8MHz_33k3BPS_CFG2;
			cfg3 = MCP_8MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS): //  40Kbps
			cfg1 = MCP_8MHz_40kBPS_CFG1;
			cfg2 = MCP_8MHz_40kBPS_CFG2;
			cfg3 = MCP_8MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS): //  50Kbps
			cfg1 = MCP_8MHz_50kBPS_CFG1;
			cfg2 = MCP_8MHz_50kBPS_CFG2;
			cfg3 = MCP_8MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS): //  80Kbps
			cfg1 = MCP_8MHz_80kBPS_CFG1;
			cfg2 = MCP_8MHz_80kBPS_CFG2;
			cfg3 = MCP_8MHz_80kBPS_CFG3;
			break;

		case (CAN_100KBPS): // 100Kbps
			cfg1 = MCP_8MHz_100kBPS_CFG1;
			cfg2 = MCP_8MHz_100kBPS_CFG2;
			cfg3 = MCP_8MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS): // 125Kbps
			cfg1 = MCP_8MHz_125kBPS_CFG1;
			cfg2 = MCP_8MHz_125kBPS_CFG2;
			cfg3 = MCP_8MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS): // 200Kbps
			cfg1 = MCP_8MHz_200kBPS_CFG1;
			cfg2 = MCP_8MHz_200kBPS_CFG2;
			cfg3 = MCP_8MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS): // 250Kbps
			cfg1 = MCP_8MHz_250kBPS_CFG1;
			cfg2 = MCP_8MHz_250kBPS_CFG2;
			cfg3 = MCP_8MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS): // 500Kbps
			cfg1 = MCP_8MHz_500kBPS_CFG1;
			cfg2 = MCP_8MHz_500kBPS_CFG2;
			cfg3 = MCP_8MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS): //   1Mbps
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
		switch (canSpeed)
		{
		case (CAN_5KBPS): //   5Kbps
			cfg1 = MCP_16MHz_5kBPS_CFG1;
			cfg2 = MCP_16MHz_5kBPS_CFG2;
			cfg3 = MCP_16MHz_5kBPS_CFG3;
			break;

		case (CAN_10KBPS): //  10Kbps
			cfg1 = MCP_16MHz_10kBPS_CFG1;
			cfg2 = MCP_16MHz_10kBPS_CFG2;
			cfg3 = MCP_16MHz_10kBPS_CFG3;
			break;

		case (CAN_20KBPS): //  20Kbps
			cfg1 = MCP_16MHz_20kBPS_CFG1;
			cfg2 = MCP_16MHz_20kBPS_CFG2;
			cfg3 = MCP_16MHz_20kBPS_CFG3;
			break;

		case (CAN_33KBPS): //  33.333Kbps
			cfg1 = MCP_16MHz_33k3BPS_CFG1;
			cfg2 = MCP_16MHz_33k3BPS_CFG2;
			cfg3 = MCP_16MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS): //  40Kbps
			cfg1 = MCP_16MHz_40kBPS_CFG1;
			cfg2 = MCP_16MHz_40kBPS_CFG2;
			cfg3 = MCP_16MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS): //  50Kbps
			cfg1 = MCP_16MHz_50kBPS_CFG1;
			cfg2 = MCP_16MHz_50kBPS_CFG2;
			cfg3 = MCP_16MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS): //  80Kbps
			cfg1 = MCP_16MHz_80kBPS_CFG1;
			cfg2 = MCP_16MHz_80kBPS_CFG2;
			cfg3 = MCP_16MHz_80kBPS_CFG3;
			break;

		case (CAN_83K3BPS): //  83.333Kbps
			cfg1 = MCP_16MHz_83k3BPS_CFG1;
			cfg2 = MCP_16MHz_83k3BPS_CFG2;
			cfg3 = MCP_16MHz_83k3BPS_CFG3;
			break;

		case (CAN_95KBPS): //  95Kbps
			cfg1 = MCP_16MHz_95kBPS_CFG1;
			cfg2 = MCP_16MHz_95kBPS_CFG2;
			cfg3 = MCP_16MHz_95kBPS_CFG3;
			break;

		case (CAN_100KBPS): // 100Kbps
			cfg1 = MCP_16MHz_100kBPS_CFG1;
			cfg2 = MCP_16MHz_100kBPS_CFG2;
			cfg3 = MCP_16MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS): // 125Kbps
			cfg1 = MCP_16MHz_125kBPS_CFG1;
			cfg2 = MCP_16MHz_125kBPS_CFG2;
			cfg3 = MCP_16MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS): // 200Kbps
			cfg1 = MCP_16MHz_200kBPS_CFG1;
			cfg2 = MCP_16MHz_200kBPS_CFG2;
			cfg3 = MCP_16MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS): // 250Kbps
			cfg1 = MCP_16MHz_250kBPS_CFG1;
			cfg2 = MCP_16MHz_250kBPS_CFG2;
			cfg3 = MCP_16MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS): // 500Kbps
			cfg1 = MCP_16MHz_500kBPS_CFG1;
			cfg2 = MCP_16MHz_500kBPS_CFG2;
			cfg3 = MCP_16MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS): //   1Mbps
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
		switch (canSpeed)
		{
		case (CAN_33KBPS): //  33.333Kbps
			cfg1 = MCP_20MHz_33k3BPS_CFG1;
			cfg2 = MCP_20MHz_33k3BPS_CFG2;
			cfg3 = MCP_20MHz_33k3BPS_CFG3;
			break;

		case (CAN_40KBPS): //  40Kbps
			cfg1 = MCP_20MHz_40kBPS_CFG1;
			cfg2 = MCP_20MHz_40kBPS_CFG2;
			cfg3 = MCP_20MHz_40kBPS_CFG3;
			break;

		case (CAN_50KBPS): //  50Kbps
			cfg1 = MCP_20MHz_50kBPS_CFG1;
			cfg2 = MCP_20MHz_50kBPS_CFG2;
			cfg3 = MCP_20MHz_50kBPS_CFG3;
			break;

		case (CAN_80KBPS): //  80Kbps
			cfg1 = MCP_20MHz_80kBPS_CFG1;
			cfg2 = MCP_20MHz_80kBPS_CFG2;
			cfg3 = MCP_20MHz_80kBPS_CFG3;
			break;

		case (CAN_83K3BPS): //  83.333Kbps
			cfg1 = MCP_20MHz_83k3BPS_CFG1;
			cfg2 = MCP_20MHz_83k3BPS_CFG2;
			cfg3 = MCP_20MHz_83k3BPS_CFG3;
			break;

		case (CAN_100KBPS): // 100Kbps
			cfg1 = MCP_20MHz_100kBPS_CFG1;
			cfg2 = MCP_20MHz_100kBPS_CFG2;
			cfg3 = MCP_20MHz_100kBPS_CFG3;
			break;

		case (CAN_125KBPS): // 125Kbps
			cfg1 = MCP_20MHz_125kBPS_CFG1;
			cfg2 = MCP_20MHz_125kBPS_CFG2;
			cfg3 = MCP_20MHz_125kBPS_CFG3;
			break;

		case (CAN_200KBPS): // 200Kbps
			cfg1 = MCP_20MHz_200kBPS_CFG1;
			cfg2 = MCP_20MHz_200kBPS_CFG2;
			cfg3 = MCP_20MHz_200kBPS_CFG3;
			break;

		case (CAN_250KBPS): // 250Kbps
			cfg1 = MCP_20MHz_250kBPS_CFG1;
			cfg2 = MCP_20MHz_250kBPS_CFG2;
			cfg3 = MCP_20MHz_250kBPS_CFG3;
			break;

		case (CAN_500KBPS): // 500Kbps
			cfg1 = MCP_20MHz_500kBPS_CFG1;
			cfg2 = MCP_20MHz_500kBPS_CFG2;
			cfg3 = MCP_20MHz_500kBPS_CFG3;
			break;

		case (CAN_1000KBPS): //   1Mbps
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

	/* Seteamos los cambios en el modulo */
	CNF1_t _cnf1 = {.data = 0};
	CNF2_t _cnf2 = {.data = 0};
	CNF3_t _cnf3 = {.data = 0};

	_cnf1.data = cfg1;
	_cnf2.data = cfg2;
	_cnf3.data = cfg3;

	if (set)
	{
		mcp2515_setRegister(MCP_CNF1, _cnf1.data);
		mcp2515_setRegister(MCP_CNF2, _cnf2.data);
		mcp2515_setRegister(MCP_CNF3, _cnf3.data);
		return ERROR_OK;
	}
	else
	{
		return ERROR_FAIL;
	}
}

extern ERROR_t mcp2515_setClkOut(const CAN_CLKOUT divisor)
{
	if (divisor == CLKOUT_DISABLE)
	{
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
							  const uint32_t id)
{
	uint16_t canid = (uint16_t)(id & 0x0FFFF); /*< Extrae los 16 bits menos significativos del ID.*/

	if (ext)
	{
		buffer[MCP_EID0] = (uint8_t)(canid & 0xFF); /*< Primeros 8 bits bajos en EID0.*/
		buffer[MCP_EID8] = (uint8_t)(canid >> 8);	/*< Segundos 8 bits altos en EID8.*/
		canid = (uint16_t)(id >> 16);
		buffer[MCP_SIDL] = (uint8_t)(canid & 0x03); /*< Bits 16 y 17 del id extendido.*/
		buffer[MCP_SIDL] += (uint8_t)((canid & 0x1C) << 3);
		buffer[MCP_SIDL] |= TXB_EXIDE_MASK;		  /*< Pone en '1' el bit EXIDE.*/
		buffer[MCP_SIDH] = (uint8_t)(canid >> 5); /*< Obtiene los ultimos 5 bits de la parte alta.*/
												  /*
												   * El formato de SIDL queda como sigue: SID2 SID1 SID0 - EXIDE - EID17 EID16,
												   * donde '-' corresponde a un bit reservado. --> Esto se carga en TXBnSIDL.
												   * */
	}
	else
	{
		buffer[MCP_SIDH] = (uint8_t)(canid >> 3);		   /*< Bits del 3-10.*/
		buffer[MCP_SIDL] = (uint8_t)((canid & 0x07) << 5); /*< Bits del 0-2.*/
		buffer[MCP_EID0] = 0;
		buffer[MCP_EID8] = 0;
	}

	return;
}

extern ERROR_t mcp2515_setFilterMask(const MASK mask, const bool ext,
									 const uint32_t ulData)
{
	/* Setea al modulo en modo de configuracion */
	ERROR_t res = mcp2515_setConfigMode();

	if (res != ERROR_OK)
	{
		return res;
	}

	uint8_t tbufdata[4];

	mcp2515_prepareId(tbufdata, ext, ulData);

	REGISTER_t reg;

	switch (mask)
	{
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
	/*
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
								 const uint32_t ulData)
{
	/* Configura el modo de configuracion */
	ERROR_t res = mcp2515_setConfigMode();

	if (res != ERROR_OK)
	{
		return res;
	}

	REGISTER_t reg;

	switch (num)
	{
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

	uint8_t tbufdata[4]; /* Utilizado para modificar los registros sid y eid del modulo.*/

	mcp2515_prepareId(tbufdata, ext, ulData); /* Configuramos el id de acuerdo al formato extendido o no.*/
	mcp2515_setRegisters(reg, tbufdata, 4);	  /* Cargamos el id configurado.*/

	return ERROR_OK;
}

extern ERROR_t mcp2515_sendMessageWithBufferId(const TXBn txbn,
											   const struct can_frame *frame)
{
	/* Verifica que no se supere la cantidad maxima de datos */
	if (frame->can_dlc > CAN_MAX_DLEN)
	{
		return ERROR_FAILTX;
	}

	/* Envia la informacion */
	const struct TXBn_REGS *txbuf = &TXB[txbn];

	uint8_t data[13];
	/*
	 * Formato de data[13]:
	 * 		byte 0: SIDH
	 * 		byte 1: SIDL
	 * 		byte 2: EID8
	 * 		byte 3: EID0
	 * 		byte 4: TXBnDLC
	 * 		byte 5-13: Data frame
	 * */

	bool ext = (frame->can_id & CAN_EFF_FLAG); /*< Determina si es de formato extendido.*/
	bool rtr = (frame->can_id & CAN_RTR_FLAG); /*< Determina si RTR se encuentra en '1'.*/

	uint32_t id = (frame->can_id & (ext ? CAN_EFF_MASK : CAN_SFF_MASK));

	/* Cargo los bytes del 0 al 3 */
	mcp2515_prepareId(data, ext, id); /*< Le da el formato adecuado.*/

	/* Cargo el byte 4 */
	data[MCP_DLC] = rtr ? (frame->can_dlc | RTR_MASK) : frame->can_dlc;

	/* Cargos los bytes del 5 al 13 */
	memcpy(&data[MCP_DATA], frame->data, frame->can_dlc);

	/* Envia los datos */
	mcp2515_setRegisters(txbuf->SIDH, data, 5 + frame->can_dlc);

	mcp2515_modifyRegister(txbuf->CTRL, TXB_TXREQ, TXB_TXREQ);

	/* Verifica la informacion enviada */
	ReadReg_t readReg = {
		.instruction = INSTRUCTION_READ,
		.reg = txbuf->CTRL,
		.data = 0,
	};

	mcp2515_readRegister(&readReg);

	if ((readReg.data & (TXB_ABTF	/*< Message aborted flag bit.*/
						 | TXB_MLOA /*< Message lost arbitration bit.*/
						 | TXB_TXERR /*< Transmission error detected bit.*/)) != 0)
	{
		return ERROR_FAILTX;
	}

	return ERROR_OK;
}

extern ERROR_t mcp2515_sendMessage(const struct can_frame *frame)
{
	/* Verifica que no supera la cantidad maxima de bytes */
	if (frame->can_dlc > CAN_MAX_DLEN)
	{
		return ERROR_FAILTX;
	}

	/* Verificamos que exista lugar disponible en algun buffer (0,1,2) */
	TXBn txBuffers[N_TXBUFFERS] = {TXB0, TXB1, TXB2};

	TXBnCTRL_t txbnCtrl[N_TXBUFFERS] = {
		{.data = 0},
		{.data = 0},
		{.data = 0},
	};

	ReadReg_t readReg[N_TXBUFFERS] = {
		{
			.reg = MCP_TXB0CTRL, /*< Cargamos el registro de control.*/
		},
		{
			.reg = MCP_TXB1CTRL, /*< Idem.*/
		},
		{
			.reg = MCP_TXB2CTRL, /*< Idem.*/
		},
	};

	for (int i = 0; i < N_TXBUFFERS; i++)
	{
		/* Verifica que el TXBnCTRL --> TXREQ == 1 */
		/*
		 * TXREQ: verifica que el registro este libre para poder
		 * cargar un dato y luego transmitirlo.
		 *
		 * Recordar: existen 3 buffers de transmisión por tanto
		 * si uno se encuentra pendiente para transmitir el TXREQ
		 * se setea en '1', y luego se carga el mensaje en el siguiente
		 * buffer (donde se vuelve a verificar que TXREQ == 0). Esto
		 * se realiza 3 buffer, iterando entre los 3 que existen.
		 * Si todos los buffers estan pendientes para transmitir
		 * se emite un mensaje de error.
		 * */
		mcp2515_readRegister(&readReg[i]);

		txbnCtrl[i].data = readReg[i].data; /*< Carga la lectura en _txbnCtrl.*/

		if (txbnCtrl[i].TXREQ == 0)
		{
			return mcp2515_sendMessageWithBufferId(txBuffers[i], frame);
		}
	}

	/* Solo si los 3 buffers estan ocupados */
	return ERROR_ALLTXBUSY;
}

extern ERROR_t mcp2515_readMessageWithBufferId(const RXBn rxbn,
											   struct can_frame *frame)
{
	const struct RXBn_REGS *rxb = &RXB[rxbn];

	ReadRegs_t readRegs = {
		.reg = rxb->SIDH,
		.instruction = INSTRUCTION_READ,
		.n = 5,
	};

	/* Lee las id de formato estandar y extendido */
	/*
	 * Se carga el valor de la id estandar y extendida
	 * al tipo de dato. Para esto debemos cargar el
	 * registro donde vamos a leer. Este registro
	 * puede ser al buffer 0 o al 1.
	 * */
	mcp2515_readRegisters(&readRegs);

	/* Tomamos el valor del id */
	/*
	 * Leemos desde el registro sidh. Por tanto tenemos
	 * SIDH, SIDL, EXID0, EXID8, ...
	 *
	 * id = SIDH(8 bits) + SIDL(3 bits estandart + 2 bits extendido) + EXID8(8 bits ext) + EXID0()
	 * */
	uint32_t id = (readRegs.values[MCP_SIDH] << 3) + (readRegs.values[MCP_SIDL] >> 5);

	/* Verifica si se enuentra en formato extendido */
	if ((readRegs.values[MCP_SIDL] & TXB_EXIDE_MASK) == TXB_EXIDE_MASK)
	{
		id = (id << 2) + (readRegs.values[MCP_SIDL] & 0x03); /*< Agarra los bits 16 y 17 del exid.*/
		id = (id << 8) + readRegs.values[MCP_EID8];			 /*< 8 bits de la parte alta.*/
		id = (id << 8) + readRegs.values[MCP_EID0];			 /*< 8 bits de la parte baja.*/
		id |= CAN_EFF_FLAG;
	}

	/* Determina la cantidad de data frame a recibir */
	uint8_t dlc = (readRegs.values[MCP_DLC] & DLC_MASK);

	if (dlc > CAN_MAX_DLEN)
	{
		return ERROR_FAIL;
	}

	/* Leemos el registro de control */
	ReadReg_t readRegCTRL = {
		.instruction = INSTRUCTION_READ,
		.reg = rxb->CTRL,
		.data = 0,
	};

	mcp2515_readRegister(&readRegCTRL);

	if (readRegCTRL.data & RXBnCTRL_RTR)
	{
		id |= CAN_RTR_FLAG;
	}

	frame->can_id = id;
	frame->can_dlc = dlc;

	ReadRegs_t readRegsDATA = {
		.instruction = INSTRUCTION_READ,
		.reg = rxb->DATA,
		.n = dlc,
	};

	mcp2515_readRegisters(&readRegsDATA);

	/* Modificamos el registro de interrupciones can */
	ModifyReg_t _modifyReg = {
		.reg = MCP_CANINTF,
		.mask = rxb->CANINTF_RXnIF,
		.data = 0,
	};

	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	return ERROR_OK;
}

extern ERROR_t mcp2515_readMessage(struct can_frame *frame)
{
	ERROR_t error;
	uint8_t stat = mcp2515_getStatus();

	if (stat & STAT_RX0IF)
	{
		error = mcp2515_readMessageWithBufferId(RXB0, frame);
	}
	else if (stat & STAT_RX1IF)
	{
		error = mcp2515_readMessageWithBufferId(RXB1, frame);
	}
	else
	{
		error = ERROR_NOMSG;
	}

	return error;
}

extern bool mcp2515_checkReceive(void)
{
	uint8_t res = mcp2515_getStatus();

	if (res & STAT_RXIF_MASK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

extern bool mcp2515_checkError(void)
{
	uint8_t eflg = mcp2515_getErrorFlags();

	if (eflg & EFLG_ERRORMASK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

extern uint8_t mcp2515_getErrorFlags(void)
{
	/*
		Para detectar errores podemos acceder al registro ERROR FLAG,
		corresponde al valor de 0x2D = 45. Dentro de esto tenemos
		errores por: overflow de buffer de rx, errores en transmision,
		errores en recepcion, etc.
	*/

	ReadReg_t _readReg = {
		.instruction = INSTRUCTION_READ,
		.reg = MCP_EFLG,
	};

	mcp2515_readRegister(&_readReg);

	return _readReg.data;
}

extern void mcp2515_clearRXnOVRFlags(void)
{
	EFLG_t eflg;

	eflg.RX0OVR = 1;
	eflg.RX1OVR = 1;

	ModifyReg_t _modifyReg = {
		.reg = MCP_EFLG,
		.mask = eflg.data, /*EFLG_RX0OVR | EFLG_RX1OVR*/
		.data = 0,
	};

	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	return;
}

extern uint8_t mcp2515_getInterrupts(void)
{
	ReadReg_t readReg = {
		.reg = MCP_CANINTF,
		.instruction = INSTRUCTION_READ,
		.data = 0,
	};

	mcp2515_readRegister(&readReg);

	return readReg.data;
}

extern void mcp2515_clearInterrupts(void)
{
	mcp2515_setRegister(MCP_CANINTF, 0);

	return;
}

extern uint8_t mcp2515_getInterruptMask(void)
{
	ReadReg_t readReg = {
		.reg = MCP_CANINTE,
		.instruction = INSTRUCTION_READ,
	};

	mcp2515_readRegister(&readReg);

	return readReg.data;
}

extern void mcp2515_clearTXInterrupts(void)
{
	CANINTF_t canintf = {
		.data = 0,
	};

	canintf.TX0IF = 1;
	canintf.TX1IF = 1;
	canintf.TX2IF = 1;

	ModifyReg_t _modifyReg = {
		.reg = MCP_CANINTF,
		.mask = canintf.data, /*(CANINTF_TX0IF | CANINTF_TX1IF | CANINTF_TX2IF),*/
		.data = 0,
	};

	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	return;
}

extern void mcp2515_clearRXnOVR(void)
{
	uint8_t eflg = mcp2515_getErrorFlags();

	if (eflg != 0)
	{
		mcp2515_clearRXnOVRFlags(); /* Detecta si existe un overflow */
		mcp2515_clearInterrupts();	/* Limpia todas las interrupciones */
									// modifyRegister(MCP_CANINTF, CANINTF_ERRIF, 0);
	}

	return;
}

extern void mcp2515_clearMERR()
{
	CANINTF_t canintf = {
		.data = 0,
	};

	canintf.MERRF = 1;

	ModifyReg_t _modifyReg = {
		.reg = MCP_CANINTF,
		.mask = canintf.data, // CANINTF_MERRF
		.data = 0,
	};

	// modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
	// clearInterrupts();
	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	return;
}

extern void mcp2515_clearERRIF()
{
	CANINTF_t canintf = {
		.data = 0,
	};

	canintf.ERRIF = 1;

	ModifyReg_t _modifyReg = {
		.reg = MCP_CANINTF,
		.mask = canintf.data, // CANINTF_MERRF
		.data = 0,
	};

	// modifyRegister(MCP_EFLG, EFLG_RX0OVR | EFLG_RX1OVR, 0);
	// clearInterrupts();
	mcp2515_modifyRegister(_modifyReg.reg, _modifyReg.mask, _modifyReg.data);

	return;
}

extern uint8_t mcp2515_errorCountRX(void)
{
	ReadReg_t readReg = {
		.reg = MCP_REC,
		.instruction = INSTRUCTION_READ,
		.data = 0,
	};

	mcp2515_readRegister(&readReg);

	return readReg.data;
}

extern uint8_t mcp2515_errorCountTX(void)
{
	ReadReg_t readReg = {
		.reg = MCP_TEC,
		.instruction = INSTRUCTION_READ,
		.data = 0,
	};

	mcp2515_readRegister(&readReg);

	return readReg.data;
}
