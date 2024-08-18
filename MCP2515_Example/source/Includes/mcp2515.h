/*
 * mcp2515.h
 *
 *
 *  Created on: 3 ago. 2024
 *      Author: agustin
 */

/**
 * @file mcp2515.h
 *
 * @brief Liberia del modulo CAN MCP2515, utilizada para la placa de desarrollo KL46Z.
 * Realizada como parte de la adcripcion de la materia de Sistemas digitales II.
 *
 * @author Zuliani, Agustin.
 *
 * @date 03/08/24
 */

#ifndef INCLUDES_MCP2515_H_
#define INCLUDES_MCP2515_H_

#include "can.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Tipo de sistema a utilizar.
 *
 * Se debe definir si se utiliza el sistema de tiempo real freertos o
 * se utiliza el sistema de baremetal.
 *
 * @example
 * Si definimos USE_FREERTOS 1 entonces estamos utilizando el sistema en tiempo real de freertos
 * Si definimos USE_FREERTOS 0 entonces estamos utilizando el sistema de baremetal.
 */
#define USE_FREERTOS 0

/*
 * @brief Speed 8M.
 *
 * Valores calculados para la velocidad de bits por segundo.
 * Tales valores son cargamos en los registros de configuración del
 * modulo.
 */
#define MCP_8MHz_1000kBPS_CFG1 (0x00)
#define MCP_8MHz_1000kBPS_CFG2 (0x80)
#define MCP_8MHz_1000kBPS_CFG3 (0x80)

#define MCP_8MHz_500kBPS_CFG1 (0x00)
#define MCP_8MHz_500kBPS_CFG2 (0x90)
#define MCP_8MHz_500kBPS_CFG3 (0x82)

#define MCP_8MHz_250kBPS_CFG1 (0x00)
#define MCP_8MHz_250kBPS_CFG2 (0xB1)
#define MCP_8MHz_250kBPS_CFG3 (0x85)

#define MCP_8MHz_200kBPS_CFG1 (0x00)
#define MCP_8MHz_200kBPS_CFG2 (0xB4)
#define MCP_8MHz_200kBPS_CFG3 (0x86)

#define MCP_8MHz_125kBPS_CFG1 (0x01)
#define MCP_8MHz_125kBPS_CFG2 (0xB1)
#define MCP_8MHz_125kBPS_CFG3 (0x85)

#define MCP_8MHz_100kBPS_CFG1 (0x01)
#define MCP_8MHz_100kBPS_CFG2 (0xB4)
#define MCP_8MHz_100kBPS_CFG3 (0x86)

#define MCP_8MHz_80kBPS_CFG1 (0x01)
#define MCP_8MHz_80kBPS_CFG2 (0xBF)
#define MCP_8MHz_80kBPS_CFG3 (0x87)

#define MCP_8MHz_50kBPS_CFG1 (0x03)
#define MCP_8MHz_50kBPS_CFG2 (0xB4)
#define MCP_8MHz_50kBPS_CFG3 (0x86)

#define MCP_8MHz_40kBPS_CFG1 (0x03)
#define MCP_8MHz_40kBPS_CFG2 (0xBF)
#define MCP_8MHz_40kBPS_CFG3 (0x87)

#define MCP_8MHz_33k3BPS_CFG1 (0x47)
#define MCP_8MHz_33k3BPS_CFG2 (0xE2)
#define MCP_8MHz_33k3BPS_CFG3 (0x85)

#define MCP_8MHz_31k25BPS_CFG1 (0x07)
#define MCP_8MHz_31k25BPS_CFG2 (0xA4)
#define MCP_8MHz_31k25BPS_CFG3 (0x84)

#define MCP_8MHz_20kBPS_CFG1 (0x07)
#define MCP_8MHz_20kBPS_CFG2 (0xBF)
#define MCP_8MHz_20kBPS_CFG3 (0x87)

#define MCP_8MHz_10kBPS_CFG1 (0x0F)
#define MCP_8MHz_10kBPS_CFG2 (0xBF)
#define MCP_8MHz_10kBPS_CFG3 (0x87)

#define MCP_8MHz_5kBPS_CFG1 (0x1F)
#define MCP_8MHz_5kBPS_CFG2 (0xBF)
#define MCP_8MHz_5kBPS_CFG3 (0x87)

/*
 * @brief Speed 16M.
 *
 * Configuraciones para 16 MHz.
 */
#define MCP_16MHz_1000kBPS_CFG1 (0x00)
#define MCP_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP_16MHz_1000kBPS_CFG3 (0x82)

#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)

#define MCP_16MHz_250kBPS_CFG1 (0x41)
#define MCP_16MHz_250kBPS_CFG2 (0xF1)
#define MCP_16MHz_250kBPS_CFG3 (0x85)

#define MCP_16MHz_200kBPS_CFG1 (0x01)
#define MCP_16MHz_200kBPS_CFG2 (0xFA)
#define MCP_16MHz_200kBPS_CFG3 (0x87)

#define MCP_16MHz_125kBPS_CFG1 (0x03)
#define MCP_16MHz_125kBPS_CFG2 (0xF0)
#define MCP_16MHz_125kBPS_CFG3 (0x86)

#define MCP_16MHz_100kBPS_CFG1 (0x03)
#define MCP_16MHz_100kBPS_CFG2 (0xFA)
#define MCP_16MHz_100kBPS_CFG3 (0x87)

#define MCP_16MHz_95kBPS_CFG1 (0x03)
#define MCP_16MHz_95kBPS_CFG2 (0xAD)
#define MCP_16MHz_95kBPS_CFG3 (0x07)

#define MCP_16MHz_83k3BPS_CFG1 (0x03)
#define MCP_16MHz_83k3BPS_CFG2 (0xBE)
#define MCP_16MHz_83k3BPS_CFG3 (0x07)

#define MCP_16MHz_80kBPS_CFG1 (0x03)
#define MCP_16MHz_80kBPS_CFG2 (0xFF)
#define MCP_16MHz_80kBPS_CFG3 (0x87)

#define MCP_16MHz_50kBPS_CFG1 (0x07)
#define MCP_16MHz_50kBPS_CFG2 (0xFA)
#define MCP_16MHz_50kBPS_CFG3 (0x87)

#define MCP_16MHz_40kBPS_CFG1 (0x07)
#define MCP_16MHz_40kBPS_CFG2 (0xFF)
#define MCP_16MHz_40kBPS_CFG3 (0x87)

#define MCP_16MHz_33k3BPS_CFG1 (0x4E)
#define MCP_16MHz_33k3BPS_CFG2 (0xF1)
#define MCP_16MHz_33k3BPS_CFG3 (0x85)

#define MCP_16MHz_20kBPS_CFG1 (0x0F)
#define MCP_16MHz_20kBPS_CFG2 (0xFF)
#define MCP_16MHz_20kBPS_CFG3 (0x87)

#define MCP_16MHz_10kBPS_CFG1 (0x1F)
#define MCP_16MHz_10kBPS_CFG2 (0xFF)
#define MCP_16MHz_10kBPS_CFG3 (0x87)

#define MCP_16MHz_5kBPS_CFG1 (0x3F)
#define MCP_16MHz_5kBPS_CFG2 (0xFF)
#define MCP_16MHz_5kBPS_CFG3 (0x87)

/*
 * @brief Speed 20M.
 *
 * Configuraciones para 20MHz.
 */
#define MCP_20MHz_1000kBPS_CFG1 (0x00)
#define MCP_20MHz_1000kBPS_CFG2 (0xD9)
#define MCP_20MHz_1000kBPS_CFG3 (0x82)

#define MCP_20MHz_500kBPS_CFG1 (0x00)
#define MCP_20MHz_500kBPS_CFG2 (0xFA)
#define MCP_20MHz_500kBPS_CFG3 (0x87)

#define MCP_20MHz_250kBPS_CFG1 (0x41)
#define MCP_20MHz_250kBPS_CFG2 (0xFB)
#define MCP_20MHz_250kBPS_CFG3 (0x86)

#define MCP_20MHz_200kBPS_CFG1 (0x01)
#define MCP_20MHz_200kBPS_CFG2 (0xFF)
#define MCP_20MHz_200kBPS_CFG3 (0x87)

#define MCP_20MHz_125kBPS_CFG1 (0x03)
#define MCP_20MHz_125kBPS_CFG2 (0xFA)
#define MCP_20MHz_125kBPS_CFG3 (0x87)

#define MCP_20MHz_100kBPS_CFG1 (0x04)
#define MCP_20MHz_100kBPS_CFG2 (0xFA)
#define MCP_20MHz_100kBPS_CFG3 (0x87)

#define MCP_20MHz_83k3BPS_CFG1 (0x04)
#define MCP_20MHz_83k3BPS_CFG2 (0xFE)
#define MCP_20MHz_83k3BPS_CFG3 (0x87)

#define MCP_20MHz_80kBPS_CFG1 (0x04)
#define MCP_20MHz_80kBPS_CFG2 (0xFF)
#define MCP_20MHz_80kBPS_CFG3 (0x87)

#define MCP_20MHz_50kBPS_CFG1 (0x09)
#define MCP_20MHz_50kBPS_CFG2 (0xFA)
#define MCP_20MHz_50kBPS_CFG3 (0x87)

#define MCP_20MHz_40kBPS_CFG1 (0x09)
#define MCP_20MHz_40kBPS_CFG2 (0xFF)
#define MCP_20MHz_40kBPS_CFG3 (0x87)

#define MCP_20MHz_33k3BPS_CFG1 (0x0B)
#define MCP_20MHz_33k3BPS_CFG2 (0xFF)
#define MCP_20MHz_33k3BPS_CFG3 (0x87)

/**
 * @brief Velocidad del reloj del modulo.
 *
 * Para el caso actual se selecciona la velocidad de 8 MHz ya que es
 * el tipo de oscilador que viene fisicamente en el modulo can.
 */
typedef enum
{
	MCP_20MHZ,
	MCP_16MHZ,
	MCP_8MHZ,
} CAN_CLOCK;

/**
 * @brief Velocidad en baudios del modulo.
 *
 * Se puede seleccionar el tipo de velocidad cuando se llama a la funcion
 * mcp2515_setBitrate(). Por defecto se puede elegir en 125KBps a 8MHz,
 * que es el caso para este modulo.
 */
typedef enum
{
	CAN_5KBPS,
	CAN_10KBPS,
	CAN_20KBPS,
	CAN_31K25BPS,
	CAN_33KBPS,
	CAN_40KBPS,
	CAN_50KBPS,
	CAN_80KBPS,
	CAN_83K3BPS,
	CAN_95KBPS,
	CAN_100KBPS,
	CAN_125KBPS,
	CAN_200KBPS,
	CAN_250KBPS,
	CAN_500KBPS,
	CAN_1000KBPS,
} CAN_SPEED;

typedef enum
{
	CLKOUT_DISABLE = -1,
	CLKOUT_DIV1 = 0x0,
	CLKOUT_DIV2 = 0x1,
	CLKOUT_DIV4 = 0x2,
	CLKOUT_DIV8 = 0x3,
} CAN_CLKOUT;

/**
 * @brief Enumeracion con los tipos de errores que aparecen.
 *
 * Manejo de errores al enviar, recibir mensajes.
 */
typedef enum
{
	/**
	 * @brief Sin errores.
	 */
	ERROR_OK = 0,
	/**
	 * @brief Fallo.
	 */
	ERROR_FAIL = 1,
	/**
	 * @brief Todos los buffer estan llenos.
	 */
	ERROR_ALLTXBUSY = 2,
	/**
	 * @brief Fallo en la inicializacion.
	 */
	ERROR_FAILINIT = 3,
	/**
	 * @brief Fallo en tx.
	 */
	ERROR_FAILTX = 4,
	/**
	 * @brief Sin mensajes.
	 */
	ERROR_NOMSG = 5,
	/**
	 * @brief Error en spi write
	 */
	ERROR_SPI_WRITE,
	/**
	 * @brief Error en spi read
	 */
	ERROR_SPI_READ,
	/**
	 * @brief Error en verificacion de registro
	 */
	ERROR_VERIFICAION_MODIFY_REG,
	/**
	 * @brief Error en verificacion de registro cargado
	 */
	ERROR_VERIFICACION_SET_REGISTER,
} ERROR_t;

/**
 * @brief Mascara para ID
 */
typedef enum
{
	MASK0,
	MASK1
} MASK;

/**
 * @brief Filtros para ID
 */
typedef enum
{
	RXF0 = 0,
	RXF1 = 1,
	RXF2 = 2,
	RXF3 = 3,
	RXF4 = 4,
	RXF5 = 5
} RXF;

/**
 * @brief Referencia al buffer de recepcion
 */
typedef enum
{
	RXB0 = 0,
	RXB1 = 1
} RXBn;

/**
 * @brief Referencia al buffer de transmision
 */
typedef enum
{
	TXB0 = 0,
	TXB1 = 1,
	TXB2 = 2
} TXBn;

/**
 * @brief Interrupciones del modulo can.
 *
 * Configuraciones del registros de interrupcion.
 */
typedef enum
{
	/**
	 * @brief Bandera de interrupcion por recepcion del buffer 0.
	 */
	CANINTF_RX0IF = 0x01,
	/**
	 * @brief Bandera de interrupcion por recepcion del buffer 1.
	 */
	CANINTF_RX1IF = 0x02,
	/**
	 * @brief Bandera de interrupcion por transmision del buffer 0.
	 */
	CANINTF_TX0IF = 0x04,
	/**
	 * @brief Bandera de interrupcion por transmision del buffer 1.
	 */
	CANINTF_TX1IF = 0x08,
	/**
	 * @brief Bandera de interrupcion por transmision del buffer 2.
	 */
	CANINTF_TX2IF = 0x10,
	/**
	 * @brief Bandera de interrupcion de error.
	 */
	CANINTF_ERRIF = 0x20,
	/**
	 * @brief Bandera de interrupcion de wake-up.
	 */
	CANINTF_WAKIF = 0x40,
	/**
	 * @brief Bandera de interrupcion de error del mensaje.
	 */
	CANINTF_MERRF = 0x80,
} CANINTF_enum;

typedef enum
{
	EFLG_RX1OVR = (1 << 7),
	EFLG_RX0OVR = (1 << 6),
	EFLG_TXBO = (1 << 5),
	EFLG_TXEP = (1 << 4),
	EFLG_RXEP = (1 << 3),
	EFLG_TXWAR = (1 << 2),
	EFLG_RXWAR = (1 << 1),
	EFLG_EWARN = (1 << 0),
} EFLG_enum;

typedef enum
{
	CANCTRL_REQOP_NORMAL = 0x00,
	CANCTRL_REQOP_SLEEP = 0x20,
	CANCTRL_REQOP_LOOPBACK = 0x40,
	CANCTRL_REQOP_LISTENONLY = 0x60,
	CANCTRL_REQOP_CONFIG = 0x80,
	CANCTRL_REQOP_POWERUP = 0xE0
} CANCTRL_REQOP_MODE_t;

typedef enum
{
	STAT_RX0IF = (1 << 0),
	STAT_RX1IF = (1 << 1),
} STAT_t;

typedef enum
{
	TXB_ABTF = 0x40,
	TXB_MLOA = 0x20,
	TXB_TXERR = 0x10,
	TXB_TXREQ = 0x08,
	TXB_TXIE = 0x04,
	TXB_TXP = 0x03,
} TXBnCTRL_enum;

/**
 * @brief Intrucciones permitidas en el modulo.
 *
 * Utilizadas para decirle al modulo que tipo de informacion
 * vamos a mandarle.
 */
typedef enum
{
	/**
	 * @brief Instruccion de escritura de datos.
	 *
	 * Utilizamos esta instruccion cuando queremos escribir sobre
	 * algun registro en particular. Antes de enviar el registro
	 * debemos decirle al modulo que accion queremos realizar, en este
	 * caso de escritura.
	 */
	INSTRUCTION_WRITE = 0x02,
	/**
	 * @brief Instruccion de lectura de datos.
	 */
	INSTRUCTION_READ = 0x03,
	INSTRUCTION_BITMOD = 0x05,
	INSTRUCTION_LOAD_TX0 = 0x40,
	INSTRUCTION_LOAD_TX1 = 0x42,
	INSTRUCTION_LOAD_TX2 = 0x44,
	INSTRUCTION_RTS_TX0 = 0x81,
	INSTRUCTION_RTS_TX1 = 0x82,
	INSTRUCTION_RTS_TX2 = 0x84,
	INSTRUCTION_RTS_ALL = 0x87,
	INSTRUCTION_READ_RX0 = 0x90,
	INSTRUCTION_READ_RX1 = 0x94,
	/**
	 * @brief Instruccion de lectura de estados del modulo.
	 */
	INSTRUCTION_READ_STATUS = 0xA0,
	INSTRUCTION_RX_STATUS = 0xB0,
	/**
	 * @brief Instruccion de reseteo del modulo.
	 */
	INSTRUCTION_RESET = 0xC0
} INSTRUCTION_t;

/**
 * @brief Ubicacion de los registros.
 *
 * Utilizamos para cargarlo en la transmision del spi y decirle al
 * modulo donde queremos modificar algo.
 */
typedef enum
{
	MCP_RXF0SIDH = 0x00,
	MCP_RXF0SIDL = 0x01,
	MCP_RXF0EID8 = 0x02,
	MCP_RXF0EID0 = 0x03,
	MCP_RXF1SIDH = 0x04,
	MCP_RXF1SIDL = 0x05,
	MCP_RXF1EID8 = 0x06,
	MCP_RXF1EID0 = 0x07,
	MCP_RXF2SIDH = 0x08,
	MCP_RXF2SIDL = 0x09,
	MCP_RXF2EID8 = 0x0A,
	MCP_RXF2EID0 = 0x0B,
	MCP_CANSTAT = 0x0E,
	MCP_CANCTRL = 0x0F,
	MCP_RXF3SIDH = 0x10,
	MCP_RXF3SIDL = 0x11,
	MCP_RXF3EID8 = 0x12,
	MCP_RXF3EID0 = 0x13,
	MCP_RXF4SIDH = 0x14,
	MCP_RXF4SIDL = 0x15,
	MCP_RXF4EID8 = 0x16,
	MCP_RXF4EID0 = 0x17,
	MCP_RXF5SIDH = 0x18,
	MCP_RXF5SIDL = 0x19,
	MCP_RXF5EID8 = 0x1A,
	MCP_RXF5EID0 = 0x1B,
	MCP_TEC = 0x1C,
	MCP_REC = 0x1D,
	MCP_RXM0SIDH = 0x20,
	MCP_RXM0SIDL = 0x21,
	MCP_RXM0EID8 = 0x22,
	MCP_RXM0EID0 = 0x23,
	MCP_RXM1SIDH = 0x24,
	MCP_RXM1SIDL = 0x25,
	MCP_RXM1EID8 = 0x26,
	MCP_RXM1EID0 = 0x27,
	MCP_CNF3 = 0x28,
	MCP_CNF2 = 0x29,
	MCP_CNF1 = 0x2A,
	MCP_CANINTE = 0x2B,
	MCP_CANINTF = 0x2C,
	MCP_EFLG = 0x2D,
	MCP_TXB0CTRL = 0x30,
	MCP_TXB0SIDH = 0x31,
	MCP_TXB0SIDL = 0x32,
	MCP_TXB0EID8 = 0x33,
	MCP_TXB0EID0 = 0x34,
	MCP_TXB0DLC = 0x35,
	MCP_TXB0DATA = 0x36,
	MCP_TXB1CTRL = 0x40,
	MCP_TXB1SIDH = 0x41,
	MCP_TXB1SIDL = 0x42,
	MCP_TXB1EID8 = 0x43,
	MCP_TXB1EID0 = 0x44,
	MCP_TXB1DLC = 0x45,
	MCP_TXB1DATA = 0x46,
	MCP_TXB2CTRL = 0x50,
	MCP_TXB2SIDH = 0x51,
	MCP_TXB2SIDL = 0x52,
	MCP_TXB2EID8 = 0x53,
	MCP_TXB2EID0 = 0x54,
	MCP_TXB2DLC = 0x55,
	MCP_TXB2DATA = 0x56,
	MCP_RXB0CTRL = 0x60,
	MCP_RXB0SIDH = 0x61,
	MCP_RXB0SIDL = 0x62,
	MCP_RXB0EID8 = 0x63,
	MCP_RXB0EID0 = 0x64,
	MCP_RXB0DLC = 0x65,
	MCP_RXB0DATA = 0x66,
	MCP_RXB1CTRL = 0x70,
	MCP_RXB1SIDH = 0x71,
	MCP_RXB1SIDL = 0x72,
	MCP_RXB1EID8 = 0x73,
	MCP_RXB1EID0 = 0x74,
	MCP_RXB1DLC = 0x75,
	MCP_RXB1DATA = 0x76
} REGISTER_t;

/**
 * @brief Funciones publicas.
 * @{
 */

/**
 * @brief Inicializa el modulo mcp2515.
 * @note En esta funcion ya se realiza la inicializacion del modulo de spi.
 */
extern void mcp2515_init(void);
/**
 * @brief Resetea el modulo.
 */
extern ERROR_t mcp2515_reset(void);
/**
 * @brief Setea el modo de configuracion.
 */
extern ERROR_t mcp2515_setConfigMode();
/**
 * @brief Setea el modo de solo escucha.
 */
extern ERROR_t mcp2515_setListenOnlyMode();
/**
 * @brief Setea el modo sleep.
 */
extern ERROR_t mcp2515_setSleepMode();
/**
 * @brief Setea el modo de loopback.
 */
extern ERROR_t mcp2515_setLoopbackMode();
/**
 * @brief Setea el modo normal de trabajo.
 */
extern ERROR_t mcp2515_setNormalMode();
/**
 * @brief Hay que ver que hace.
 */
extern ERROR_t mcp2515_setClkOut(const CAN_CLKOUT divisor);
/**
 * @brief Setea el baud rate.
 *
 * @param[in] canSpeed velocidad en baudios.
 * @param[in] canClock velocidad del reloj.
 */
extern ERROR_t mcp2515_setBitrate(const CAN_SPEED canSpeed,
								  const CAN_CLOCK canClock);
/**
 * @brief Setea el filtro y la mascara.
 * Se configuran dichos datos en los registros del propio modulo.
 *
 * @param[in] num mascara.
 * @param[in] ext formato extendido.
 * @param[in] ulData id.
 */
extern ERROR_t mcp2515_setFilterMask(const MASK num, const bool ext,
									 const uint32_t ulData);
/**
 * @brief Se configura el filtro.
 *
 * @param[in] num filtro de rx.
 * @param[in] ext formato extendido.
 * @param[in] ulData id.
 */
extern ERROR_t mcp2515_setFilter(const RXF num, const bool ext,
								 const uint32_t ulData);
/**
 * @brief Envio de mensaje con el buffer.
 *
 * @param[in] txbn Buffer en el que se carga la informacion.
 * @param[in] frame informacion a transmitir.
 */
extern ERROR_t mcp2515_sendMessageWithBufferId(const TXBn txbn,
											   const struct can_frame *frame);
/**
 * @brief Envio de mensaje.
 *
 * @param[in] frame informacion a transmitir.
 */
extern ERROR_t mcp2515_sendMessage(const struct can_frame *frame);
/**
 * @brief Lee mensaje con el buffer indicado.
 *
 * @param[in] rxbn buffer de recepcion.
 * @param[out] frame lugar donde carga la informacion.
 */
extern ERROR_t mcp2515_readMessageWithBufferId(const RXBn rxbn, struct can_frame *frame);
/**
 * @brief Lee mensaje.
 *
 * @param[out] frame lugar donde carga la informacion.
 */
extern ERROR_t mcp2515_readMessage(struct can_frame *frame);
/**
 * @brief Chequea la recepcion de los datos.
 *
 * @return Estado de la recepcion.
 */
extern bool mcp2515_checkReceive(void);
/**
 * @brief Chequea los errores informados desde el modulo.
 *
 * @return Estado del error.
 */
extern bool mcp2515_checkError(void);
/**
 * @brief Obtiene el estado de error de modulo can.
 *
 * @return dato con los errores.
 */
extern uint8_t mcp2515_getErrorFlags(void);
/**
 * @brief Limpia la bandera de overflow de los buffer de rx.
 */
extern void mcp2515_clearRXnOVRFlags(void);
/**
 * @brief Obtiene las interrupciones del modulo can.
 * Desde el registro de interrupt flags.
 *
 * @return Devuelve las banderas de interrupcion que se presentaron.
 */
extern uint8_t mcp2515_getInterrupts(void);
/**
 * @brief Obtiene las interrupciones habilitadas desde el modulo can.
 *
 * @return Devuelve las interrupciones que se encuentran habilitadas.
 */
extern uint8_t mcp2515_getInterruptMask(void);
/**
 * @brief Limpia las banderas de interrupcion.
 */
extern void mcp2515_clearInterrupts(void);
/**
 * @brief Limpia las banderas de interrupcion de tx.
 */
extern void mcp2515_clearTXInterrupts(void);
/**
 * @brief Obtiene los estados solicitados de lectura.
 *
 * @return Devuelve el dato del registro de estados.
 */
extern uint8_t mcp2515_getStatus(void);
/**
 * @brief Limpia la bandera de overflow de los buffers de rx.
 */
extern void mcp2515_clearRXnOVR(void);
/**
 * @brief Limpia la bandera de merr.
 */
extern void mcp2515_clearMERR();
/**
 * @brief Limpia la bandera de errif.
 */
extern void mcp2515_clearERRIF();
/**
 * @brief Chequa el contador de error de recepcion.
 *
 * @return Devuelve el valor del contador.
 */
extern uint8_t mcp2515_errorCountRX(void);
/**
 * @brief Chequea el contador de error de transmision.
 *
 * @return Devuelve el valor del contador.
 */
extern uint8_t mcp2515_errorCountTX(void);

/**
 * @}
 */

#endif /* INCLUDES_MCP2515_H_ */
