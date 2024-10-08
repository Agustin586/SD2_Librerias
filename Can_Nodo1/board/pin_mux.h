/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*! @name PORTE29 (number 26), LED_RED
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_led_red_FGPIO FGPIOE              /*!<@brief FGPIO peripheral base pointer */
#define BOARD_led_red_GPIO GPIOE                /*!<@brief GPIO peripheral base pointer */
#define BOARD_led_red_GPIO_PIN_MASK (1U << 29U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_led_red_PORT PORTE                /*!<@brief PORT peripheral base pointer */
#define BOARD_led_red_PIN 29U                   /*!<@brief PORT pin number */
#define BOARD_led_red_PIN_MASK (1U << 29U)      /*!<@brief PORT pin mask */
                                                /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitACCEL(void);

#define SOPT5_UART0ODE_DISABLED 0x00u  /*!<@brief UART0 Open Drain Enable: Open drain is disabled on UART0. */
#define SOPT5_UART0RXSRC_UART_RX 0x00u /*!<@brief UART0 Receive Data Source Select: UART_RX pin */
#define SOPT5_UART0TXSRC_UART_TX 0x00u /*!<@brief UART0 Transmit Data Source Select: UART0_TX pin */

/*! @name PORTA2 (number 36), J1[4]/D1/UART1_TX_TGTMCU
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_DEBUG_UART_TX_PORT PORTA               /*!<@brief PORT peripheral base pointer */
#define BOARD_DEBUG_UART_TX_PIN 2U                   /*!<@brief PORT pin number */
#define BOARD_DEBUG_UART_TX_PIN_MASK (1U << 2U)      /*!<@brief PORT pin mask */
                                                     /* @} */

/*! @name PORTA1 (number 35), J1[2]/D0/UART1_RX_TGTMCU
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_DEBUG_UART_RX_PORT PORTA               /*!<@brief PORT peripheral base pointer */
#define BOARD_DEBUG_UART_RX_PIN 1U                   /*!<@brief PORT pin number */
#define BOARD_DEBUG_UART_RX_PIN_MASK (1U << 1U)      /*!<@brief PORT pin mask */
                                                     /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitDEBUG_UART(void);

/*! @name PORTC3 (number 73), SW1
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_SW1_FGPIO FGPIOC             /*!<@brief FGPIO peripheral base pointer */
#define BOARD_SW1_GPIO GPIOC               /*!<@brief GPIO peripheral base pointer */
#define BOARD_SW1_GPIO_PIN_MASK (1U << 3U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_SW1_PORT PORTC               /*!<@brief PORT peripheral base pointer */
#define BOARD_SW1_PIN 3U                   /*!<@brief PORT pin number */
#define BOARD_SW1_PIN_MASK (1U << 3U)      /*!<@brief PORT pin mask */
                                           /* @} */

/*! @name PORTC12 (number 88), SW3
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_SW3_FGPIO FGPIOC              /*!<@brief FGPIO peripheral base pointer */
#define BOARD_SW3_GPIO GPIOC                /*!<@brief GPIO peripheral base pointer */
#define BOARD_SW3_GPIO_PIN_MASK (1U << 12U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_SW3_PORT PORTC                /*!<@brief PORT peripheral base pointer */
#define BOARD_SW3_PIN 12U                   /*!<@brief PORT pin number */
#define BOARD_SW3_PIN_MASK (1U << 12U)      /*!<@brief PORT pin mask */
                                            /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitButtons(void);

/*! @name PORTE29 (number 26), LED_RED
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_LED_RED_FGPIO FGPIOE              /*!<@brief FGPIO peripheral base pointer */
#define BOARD_LED_RED_GPIO GPIOE                /*!<@brief GPIO peripheral base pointer */
#define BOARD_LED_RED_GPIO_PIN_MASK (1U << 29U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_LED_RED_PORT PORTE                /*!<@brief PORT peripheral base pointer */
#define BOARD_LED_RED_PIN 29U                   /*!<@brief PORT pin number */
#define BOARD_LED_RED_PIN_MASK (1U << 29U)      /*!<@brief PORT pin mask */
                                                /* @} */

/*! @name PORTD5 (number 98), J2[12]/D13/LED_GREEN
  @{ */

/* Symbols to be used with GPIO driver */
#define BOARD_LED_GREEN_FGPIO FGPIOD             /*!<@brief FGPIO peripheral base pointer */
#define BOARD_LED_GREEN_GPIO GPIOD               /*!<@brief GPIO peripheral base pointer */
#define BOARD_LED_GREEN_GPIO_PIN_MASK (1U << 5U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_LED_GREEN_PORT PORTD               /*!<@brief PORT peripheral base pointer */
#define BOARD_LED_GREEN_PIN 5U                   /*!<@brief PORT pin number */
#define BOARD_LED_GREEN_PIN_MASK (1U << 5U)      /*!<@brief PORT pin mask */
                                                 /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitLEDs(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitTouch(void);

/*! @name USB0_DM (number 11), J10[2]/USB_DM
  @{ */
/* @} */

/*! @name USB0_DP (number 10), J10[3]/USB_DP
  @{ */
/* @} */

/*! @name VREGIN (number 13), J7[2]/KL46Z_REGIN
  @{ */
/* @} */

/*! @name VOUT33 (number 12), C11/VOUT33
  @{ */
/* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitUSB(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitSegment_LCD(void);

/*! @name PORTA19 (number 51), Y1[3]/XTAL_8MHZ
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_XTAL0_PORT PORTA                /*!<@brief PORT peripheral base pointer */
#define BOARD_XTAL0_PIN 19U                   /*!<@brief PORT pin number */
#define BOARD_XTAL0_PIN_MASK (1U << 19U)      /*!<@brief PORT pin mask */
                                              /* @} */

/*! @name PORTA18 (number 50), Y1[1]/EXTAL_8MHZ
  @{ */

/* Symbols to be used with PORT driver */
#define BOARD_EXTAL0_PORT PORTA                /*!<@brief PORT peripheral base pointer */
#define BOARD_EXTAL0_PIN 18U                   /*!<@brief PORT pin number */
#define BOARD_EXTAL0_PIN_MASK (1U << 18U)      /*!<@brief PORT pin mask */
                                               /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitOSC(void);

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitMAG(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
