/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PERIPHERALS_H_
#define _PERIPHERALS_H_

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/
/* Definitions for BOARD_InitButtonsPeripheral functional group */
/* Alias for GPIOC peripheral */
#define BOARD_SW1_SW3_GPIO GPIOC

/* Definitions for BOARD_InitLEDsPeripheral functional group */
/* Alias for GPIOE peripheral */
#define BOARD_LED_RED_GPIO GPIOE
/* Alias for GPIOD peripheral */
#define BOARD_LED_GREEN_GPIO GPIOD

/* Definitions for BOARD_InitACCELPeripheral functional group */
/* Alias for GPIOC peripheral */
#define BOARD_INT1_GPIO GPIOC
/* Alias for GPIOD peripheral */
#define BOARD_INT2_GPIO GPIOD

/* Definitions for BOARD_InitMAGPeripheral functional group */
/* Alias for GPIOD peripheral */
#define INT2_GPIO GPIOD

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/

void BOARD_InitPeripherals(void);

void BOARD_InitButtonsPeripheral(void);

void BOARD_InitLEDsPeripheral(void);

void BOARD_InitACCELPeripheral(void);

void BOARD_InitMAGPeripheral(void);

void BOARD_InitDEBUG_UARTPeripheral(void);

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void);

#if defined(__cplusplus)
}
#endif

#endif /* _PERIPHERALS_H_ */
