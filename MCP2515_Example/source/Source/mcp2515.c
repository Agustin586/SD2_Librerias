/*
 * mcp2515.c
 *
 *  Created on: 3 ago. 2024
 *      Author: agustin
 */

#include "Includes/mcp2515.h"
#include <string.h>

#if USE_FREERTOS

#elif

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

const struct TXBn_REGS MCP2515_TXB[N_TXBUFFERS] = {
    {MCP_TXB0CTRL, MCP_TXB0SIDH, MCP_TXB0DATA},
    {MCP_TXB1CTRL, MCP_TXB1SIDH, MCP_TXB1DATA},
    {MCP_TXB2CTRL, MCP_TXB2SIDH, MCP_TXB2DATA}
};

const struct RXBn_REGS RXB[N_RXBUFFERS] = {
    {MCP_RXB0CTRL, MCP_RXB0SIDH, MCP_RXB0DATA, CANINTF_RX0IF},
    {MCP_RXB1CTRL, MCP_RXB1SIDH, MCP_RXB1DATA, CANINTF_RX1IF}
};

extern void mcp2515_init(const uint8_t _CS, const uint32_t _SPI_CLOCK = DEFAULT_SPI_CLOCK){
	/*< Configura los pines >*/

	/*< Inicializa el spi >*/


	return;
}

extern ERROR_t mcp2515_reset(void)
{
    startSPI();
    SPIn->transfer(INSTRUCTION_RESET);
    endSPI();

    /*< Reinicia el modulo >*/


    delay(10);

    uint8_t zeros[14];
    memset(zeros, 0, sizeof(zeros));
    mcp2515_setRegisters(MCP_TXB0CTRL, zeros, 14);
    mcp2515_setRegisters(MCP_TXB1CTRL, zeros, 14);
    mcp2515_setRegisters(MCP_TXB2CTRL, zeros, 14);

    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);

    mcp2515_setRegister(MCP_CANINTE, CANINTF_RX0IF | CANINTF_RX1IF | CANINTF_ERRIF | CANINTF_MERRF);

    // receives all valid messages using either Standard or Extended Identifiers that
    // meet filter criteria. RXF0 is applied for RXB0, RXF1 is applied for RXB1
    mcp2515_modifyRegister(MCP_RXB0CTRL,
                   RXBnCTRL_RXM_MASK | RXB0CTRL_BUKT | RXB0CTRL_FILHIT_MASK,
                   RXBnCTRL_RXM_STDEXT | RXB0CTRL_BUKT | RXB0CTRL_FILHIT);
    mcp2515_modifyRegister(MCP_RXB1CTRL,
                   RXBnCTRL_RXM_MASK | RXB1CTRL_FILHIT_MASK,
                   RXBnCTRL_RXM_STDEXT | RXB1CTRL_FILHIT);

    // clear filters and masks
    // do not filter any standard frames for RXF0 used by RXB0
    // do not filter any extended frames for RXF1 used by RXB1
    RXF filters[] = {RXF0, RXF1, RXF2, RXF3, RXF4, RXF5};
    for (int i=0; i<6; i++) {
        bool ext = (i == 1);
        ERROR_t result = mcp2515_setFilter(filters[i], ext, 0);
        if (result != ERROR_OK) {
            return result;
        }
    }

    MASK masks[] = {MASK0, MASK1};
    for (int i=0; i<2; i++) {
        ERROR_t result = mcp2515_setFilterMask(masks[i], true, 0);
        if (result != ERROR_OK) {
            return result;
        }
    }

    return ERROR_OK;
}







