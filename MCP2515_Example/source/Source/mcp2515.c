/*
 * mcp2515.c
 *
 *  Created on: 3 ago. 2024
 *      Author: agustin
 */

#include "Includes/mcp2515.h"

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
	REGISTER CTRL;
	REGISTER SIDH;
	REGISTER DATA;
} TXB[N_TXBUFFERS];

static const struct RXBn_REGS {
	REGISTER CTRL;
	REGISTER SIDH;
	REGISTER DATA;
	CANINTF CANINTF_RXnIF;
} RXB[N_RXBUFFERS];

/*< Funciones privadas >*/
static void startSPI();
static void endSPI();

static ERROR_t mcp2515_setMode(const CANCTRL_REQOP_MODE_t mode);

static uint8_t mcp2515_readRegister(const REGISTER_t reg);
static void mcp2515_readRegisters(const REGISTER_t reg, uint8_t values[], const uint8_t n);
static void mcp2515_setRegister(const REGISTER_t reg, const uint8_t value);
static void mcp2515_setRegisters(const REGISTER_t reg, const uint8_t values[], const uint8_t n);
static void mcp2515_modifyRegister(const REGISTER_t reg, const uint8_t mask, const uint8_t data);

static void mcp2515_prepareId(uint8_t *buffer, const bool ext, const uint32_t id);

