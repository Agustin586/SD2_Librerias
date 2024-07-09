/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v12.0
processor: MKL46Z256xxx4
package_id: MKL46Z256VLL4
mcu_data: ksdk2_0
processor_version: 12.0.0
board: FRDM-KL46Z
pin_labels:
- {pin_num: '35', pin_signal: TSI0_CH2/PTA1/UART0_RX/TPM2_CH0, label: 'J1[2]/D0/UART1_RX_TGTMCU', identifier: DEBUG_UART_RX}
- {pin_num: '36', pin_signal: TSI0_CH3/PTA2/UART0_TX/TPM2_CH1, label: 'J1[4]/D1/UART1_TX_TGTMCU', identifier: DEBUG_UART_TX}
- {pin_num: '96', pin_signal: LCD_P43/PTD3/SPI0_MISO/UART2_TX/TPM0_CH3/SPI0_MOSI/LCD_P43_Fault, label: 'J1[6]/D2'}
- {pin_num: '42', pin_signal: PTA12/TPM1_CH0/I2S0_TXD0, label: 'J1[8]/D3'}
- {pin_num: '38', pin_signal: TSI0_CH5/PTA4/I2C1_SDA/TPM0_CH1/NMI_b, label: 'J1[10]/D4'}
- {pin_num: '39', pin_signal: PTA5/USB_CLKIN/TPM0_CH2/I2S0_TX_BCLK, label: 'J1[12]/D5'}
- {pin_num: '84', pin_signal: LCD_P28/CMP0_IN2/PTC8/I2C0_SCL/TPM0_CH4/I2S0_MCLK/LCD_P28_Fault, label: 'J1[14]/D6'}
- {pin_num: '85', pin_signal: LCD_P29/CMP0_IN3/PTC9/I2C0_SDA/TPM0_CH5/I2S0_RX_BCLK/LCD_P29_Fault, label: 'J1[16]/D7'}
- {pin_num: '37', pin_signal: TSI0_CH4/PTA3/I2C1_SCL/TPM0_CH0/SWD_DIO, label: 'J11[2]/SWD_DIO_TGTMCU'}
- {pin_num: '95', pin_signal: LCD_P42/PTD2/SPI0_MOSI/UART2_RX/TPM0_CH2/SPI0_MISO/LCD_P42_Fault, label: 'J2[4]/D9'}
- {pin_num: '97', pin_signal: LCD_P44/PTD4/LLWU_P14/SPI1_PCS0/UART2_RX/TPM0_CH4/LCD_P44_Fault, label: 'J2[6]/D10'}
- {pin_num: '99', pin_signal: LCD_P46/ADC0_SE7b/PTD6/LLWU_P15/SPI1_MOSI/UART0_RX/SPI1_MISO/LCD_P46_Fault, label: 'J2[8]/D11'}
- {pin_num: '100', pin_signal: LCD_P47/PTD7/SPI1_MISO/UART0_TX/SPI1_MOSI/LCD_P47_Fault, label: 'J2[10]/D12'}
- {pin_num: '98', pin_signal: LCD_P45/ADC0_SE6b/PTD5/SPI1_SCK/UART2_TX/TPM0_CH5/LCD_P45_Fault, label: 'J2[12]/D13/LED_GREEN', identifier: LED_GREEN}
- {pin_num: '1', pin_signal: LCD_P48/PTE0/SPI1_MISO/UART1_TX/RTC_CLKOUT/CMP0_OUT/I2C1_SDA/LCD_P48_Fault, label: 'J2[18]/D14'}
- {pin_num: '2', pin_signal: LCD_P49/PTE1/SPI1_MOSI/UART1_RX/SPI1_MISO/I2C1_SCL/LCD_P49_Fault, label: 'J2[20]/D15'}
- {pin_num: '59', pin_signal: LCD_P9/PTB9/SPI1_SCK/LCD_P9_Fault, label: 'J2[17]'}
- {pin_num: '47', pin_signal: PTA17/SPI0_MISO/SPI0_MOSI/I2S0_MCLK, label: 'J2[15]'}
- {pin_num: '46', pin_signal: PTA16/SPI0_MOSI/SPI0_MISO/I2S0_RX_FS/I2S0_RXD0, label: 'J2[13]'}
- {pin_num: '45', pin_signal: PTA15/SPI0_SCK/UART0_RX/I2S0_RXD0, label: 'J2[11]'}
- {pin_num: '44', pin_signal: PTA14/SPI0_PCS0/UART0_TX/I2S0_RX_BCLK/I2S0_TXD0, label: 'J2[9]'}
- {pin_num: '40', pin_signal: PTA6/TPM0_CH3, label: 'J2[7]'}
- {pin_num: '41', pin_signal: PTA7/TPM0_CH4, label: 'J2[5]'}
- {pin_num: '90', pin_signal: LCD_P36/PTC16/LCD_P36_Fault, label: 'J2[3]'}
- {pin_num: '89', pin_signal: LCD_P33/PTC13/TPM_CLKIN1/LCD_P33_Fault, label: 'J2[1]'}
- {pin_num: '87', pin_signal: LCD_P31/PTC11/I2C1_SDA/I2S0_RXD0/LCD_P31_Fault, label: 'J1[15]/I2S_RXD'}
- {pin_num: '86', pin_signal: LCD_P30/PTC10/I2C1_SCL/I2S0_RX_FS/LCD_P30_Fault, label: 'J1[13]/I2S_RX_FS'}
- {pin_num: '83', pin_signal: LCD_P27/CMP0_IN1/PTC7/SPI0_MISO/USB_SOF_OUT/I2S0_RX_FS/SPI0_MOSI/LCD_P27_Fault, label: 'J1[11]/SOF_OUT'}
- {pin_num: '82', pin_signal: LCD_P26/CMP0_IN0/PTC6/LLWU_P10/SPI0_MOSI/EXTRG_IN/I2S0_RX_BCLK/SPI0_MISO/I2S0_MCLK/LCD_P26_Fault, label: 'J1[9]/I2S_RX_BCLK'}
- {pin_num: '80', pin_signal: LCD_P24/PTC4/LLWU_P8/SPI0_PCS0/UART1_TX/TPM0_CH3/I2S0_MCLK/LCD_P24_Fault, label: 'J1[7]/I2S_MCLK'}
- {pin_num: '70', pin_signal: LCD_P20/ADC0_SE14/TSI0_CH13/PTC0/EXTRG_IN/USB_SOF_OUT/CMP0_OUT/I2S0_TXD0/LCD_P20_Fault, label: 'J1[5]/I2S_TXD'}
- {pin_num: '65', pin_signal: LCD_P15/TSI0_CH12/PTB19/TPM2_CH1/I2S0_TX_FS/LCD_P15_Fault, label: 'J1[3]/I2S_TX_FS'}
- {pin_num: '64', pin_signal: LCD_P14/TSI0_CH11/PTB18/TPM2_CH0/I2S0_TX_BCLK/LCD_P14_Fault, label: 'J1[1]/I2S_TX_BCLK'}
- {pin_num: '27', pin_signal: DAC0_OUT/ADC0_SE23/CMP0_IN4/PTE30/TPM0_CH3/TPM_CLKIN1, label: 'J4[11]/DAC_OUT'}
- {pin_num: '18', pin_signal: LCD_P59/ADC0_DP0/ADC0_SE0/PTE20/TPM1_CH0/UART0_TX/LCD_P59_Fault, label: 'J4[1]/DIFF_ADC0_DP'}
- {pin_num: '21', pin_signal: ADC0_DM3/ADC0_SE7a/PTE23/TPM2_CH1/UART2_RX, label: 'J4[7]/DIFF_ADC1_DM'}
- {pin_num: '20', pin_signal: ADC0_DP3/ADC0_SE3/PTE22/TPM2_CH0/UART2_TX, label: 'J4[5]/SNS_LIGHT_ADC/DIFF_ADC1_DP', identifier: ADC_LIGHT_SNS}
- {pin_num: '19', pin_signal: LCD_P60/ADC0_DM0/ADC0_SE4a/PTE21/TPM1_CH1/UART0_RX/LCD_P60_Fault, label: 'J4[3]/DIFF_ADC0_DM'}
- {pin_num: '66', pin_signal: LCD_P16/PTB20/CMP0_OUT/LCD_P16_Fault, label: 'J4[9]/CMP_OUT'}
- {pin_num: '3', pin_signal: LCD_P50/PTE2/SPI1_SCK/LCD_P50_Fault, label: 'J3[15]'}
- {pin_num: '4', pin_signal: LCD_P51/PTE3/SPI1_MISO/SPI1_MOSI/LCD_P51_Fault, label: 'J3[13]'}
- {pin_num: '7', pin_signal: LCD_P54/PTE6/I2S0_MCLK/USB_SOF_OUT/LCD_P54_Fault, label: 'J3[11]'}
- {pin_num: '14', pin_signal: LCD_P55/ADC0_DP1/ADC0_SE1/PTE16/SPI0_PCS0/UART2_TX/TPM_CLKIN0/LCD_P55_Fault, label: 'J3[9]'}
- {pin_num: '15', pin_signal: LCD_P56/ADC0_DM1/ADC0_SE5a/PTE17/SPI0_SCK/UART2_RX/TPM_CLKIN1/LPTMR0_ALT3/LCD_P56_Fault, label: 'J3[7]'}
- {pin_num: '16', pin_signal: LCD_P57/ADC0_DP2/ADC0_SE2/PTE18/SPI0_MOSI/I2C0_SDA/SPI0_MISO/LCD_P57_Fault, label: 'J3[5]'}
- {pin_num: '17', pin_signal: LCD_P58/ADC0_DM2/ADC0_SE6a/PTE19/SPI0_MISO/I2C0_SCL/SPI0_MOSI/LCD_P58_Fault, label: 'J3[3]'}
- {pin_num: '28', pin_signal: PTE31/TPM0_CH4, label: 'J3[1]'}
- {pin_num: '53', pin_signal: LCD_P0/ADC0_SE8/TSI0_CH0/PTB0/LLWU_P5/I2C0_SCL/TPM1_CH0/LCD_P0_Fault, label: 'J4[2]/A0'}
- {pin_num: '54', pin_signal: LCD_P1/ADC0_SE9/TSI0_CH6/PTB1/I2C0_SDA/TPM1_CH1/LCD_P1_Fault, label: 'J4[4]/A1'}
- {pin_num: '55', pin_signal: LCD_P2/ADC0_SE12/TSI0_CH7/PTB2/I2C0_SCL/TPM2_CH0/LCD_P2_Fault, label: 'J4[6]/A2'}
- {pin_num: '56', pin_signal: LCD_P3/ADC0_SE13/TSI0_CH8/PTB3/I2C0_SDA/TPM2_CH1/LCD_P3_Fault, label: 'J4[8]/A3'}
- {pin_num: '72', pin_signal: LCD_P22/ADC0_SE11/TSI0_CH15/PTC2/I2C1_SDA/TPM0_CH1/I2S0_TX_FS/LCD_P22_Fault, label: 'J4[10]/A4'}
- {pin_num: '71', pin_signal: LCD_P21/ADC0_SE15/TSI0_CH14/PTC1/LLWU_P6/RTC_CLKIN/I2C1_SCL/TPM0_CH0/I2S0_TXD0/LCD_P21_Fault, label: 'J4[12]/A5'}
- {pin_num: '31', pin_signal: PTE24/TPM0_CH0/I2C0_SCL, label: 'U2[7]/U10[4]/I2C0_SDA', identifier: ACCEL_SCL;MAG_SCL}
- {pin_num: '32', pin_signal: PTE25/TPM0_CH1/I2C0_SDA, label: 'U2[6]/U10[6]/I2C0_SDA', identifier: ACCEL_SDA;MAG_SDA}
- {pin_num: '94', pin_signal: LCD_P41/ADC0_SE5b/PTD1/SPI0_SCK/TPM0_CH1/LCD_P41_Fault, label: 'U2[9]/U10[9]/INT2_ACCEL', identifier: MAG_INT2;ACCEL_INT2}
- {pin_num: '81', pin_signal: LCD_P25/PTC5/LLWU_P9/SPI0_SCK/LPTMR0_ALT2/I2S0_RXD0/CMP0_OUT/LCD_P25_Fault, label: 'U10[11]/INT1_ACCEL', identifier: MAG_INT1;ACCEL_INT1}
- {pin_num: '52', pin_signal: PTA20/RESET_b, label: 'J3[6]/J11[10]/SW2/RST_K20D50_B'}
- {pin_num: '93', pin_signal: LCD_P40/PTD0/SPI0_PCS0/TPM0_CH0/LCD_P40_Fault, label: 'DS1[1]/LCD-01', identifier: LCD_01}
- {pin_num: '5', pin_signal: LCD_P52/PTE4/SPI1_PCS0/LCD_P52_Fault, label: 'DS1[2]/LCD-02', identifier: LCD_02}
- {pin_num: '6', pin_signal: LCD_P53/PTE5/LCD_P53_Fault, label: 'DS1[9]/LCD-09', identifier: LCD_09}
- {pin_num: '92', pin_signal: LCD_P38/PTC18/LCD_P38_Fault, label: 'DS1[10]/LCD-10', identifier: LCD_10}
- {pin_num: '91', pin_signal: LCD_P37/PTC17/LCD_P37_Fault, label: 'DS1[5]/LCD-05', identifier: LCD_05}
- {pin_num: '69', pin_signal: LCD_P19/PTB23/LCD_P19_Fault, label: 'DS1[3]/LCD-03', identifier: LCD_03}
- {pin_num: '68', pin_signal: LCD_P18/PTB22/LCD_P18_Fault, label: 'DS1[4]/LCD-04', identifier: LCD_04}
- {pin_num: '67', pin_signal: LCD_P17/PTB21/LCD_P17_Fault, label: 'DS1[6]/LCD-06', identifier: LCD_06}
- {pin_num: '61', pin_signal: LCD_P11/PTB11/SPI1_SCK/LCD_P11_Fault, label: 'DS1[12]/LCD-12', identifier: LCD_12}
- {pin_num: '60', pin_signal: LCD_P10/PTB10/SPI1_PCS0/LCD_P10_Fault, label: 'DS1[11]/LCD-11', identifier: LCD_11}
- {pin_num: '58', pin_signal: LCD_P8/PTB8/SPI1_PCS0/EXTRG_IN/LCD_P8_Fault, label: 'DS1[8]/LCD-08', identifier: LCD_08}
- {pin_num: '57', pin_signal: LCD_P7/PTB7/LCD_P7_Fault, label: 'DS1[7]/LCD-07', identifier: LCD_07}
- {pin_num: '26', pin_signal: CMP0_IN5/ADC0_SE4b/PTE29/TPM0_CH2/TPM_CLKIN0, label: LED_RED, identifier: LED_RED}
- {pin_num: '10', pin_signal: USB0_DP, label: 'J10[3]/USB_DP', identifier: USB_DP}
- {pin_num: '11', pin_signal: USB0_DM, label: 'J10[2]/USB_DM', identifier: USB_DM}
- {pin_num: '43', pin_signal: PTA13/TPM1_CH1/I2S0_TX_FS, label: 'J2[2]/D8'}
- {pin_num: '34', pin_signal: TSI0_CH1/PTA0/TPM0_CH5/SWD_CLK, label: 'J11[4]/KL25_SWD_CLK'}
- {pin_num: '62', pin_signal: LCD_P12/TSI0_CH9/PTB16/SPI1_MOSI/UART0_RX/TPM_CLKIN0/SPI1_MISO/LCD_P12_Fault, label: E1/TSI_ELECTRODE1, identifier: TSI_ELECTRODE_1}
- {pin_num: '63', pin_signal: LCD_P13/TSI0_CH10/PTB17/SPI1_MISO/UART0_TX/TPM_CLKIN1/SPI1_MOSI/LCD_P13_Fault, label: E1/TSI_ELECTRODE2, identifier: TSI_ELECTRODE_2}
- {pin_num: '50', pin_signal: EXTAL0/PTA18/UART1_RX/TPM_CLKIN0, label: 'Y1[1]/EXTAL_8MHZ', identifier: EXTAL0}
- {pin_num: '51', pin_signal: XTAL0/PTA19/UART1_TX/TPM_CLKIN1/LPTMR0_ALT1, label: 'Y1[3]/XTAL_8MHZ', identifier: XTAL0}
- {pin_num: '73', pin_signal: LCD_P23/PTC3/LLWU_P7/UART1_RX/TPM0_CH2/CLKOUT/I2S0_TX_BCLK/LCD_P23_Fault, label: SW1, identifier: SW1}
- {pin_num: '88', pin_signal: LCD_P32/PTC12/TPM_CLKIN0/LCD_P32_Fault, label: SW3, identifier: SW3}
- {pin_num: '13', pin_signal: VREGIN, label: 'J7[2]/KL46Z_REGIN', identifier: USB_VREGIN}
- {pin_num: '23', pin_signal: VREFH, label: P3V3_KL46Z}
- {pin_num: '77', pin_signal: VLL1/LCD_P5/PTC21/LCD_P5_Fault, label: VLL1}
- {pin_num: '76', pin_signal: VLL2/LCD_P4/PTC20/LCD_P4_Fault, label: VLL2}
- {pin_num: '75', pin_signal: VLL3, label: 'J12[1]/VLL3'}
- {pin_num: '24', pin_signal: VREFL, label: GND}
- {pin_num: '8', pin_signal: VDD8, label: P3V3_KL46Z}
- {pin_num: '22', pin_signal: VDDA, label: P3V3_KL46Z}
- {pin_num: '30', pin_signal: VDD30, label: P3V3_KL46Z}
- {pin_num: '48', pin_signal: VDD48, label: P3V3_KL46Z}
- {pin_num: '9', pin_signal: VSS9, label: GND}
- {pin_num: '25', pin_signal: VSSA, label: GND}
- {pin_num: '29', pin_signal: VSS29, label: GND}
- {pin_num: '49', pin_signal: VSS49, label: GND}
- {pin_num: '74', pin_signal: VSS74, label: GND}
- {pin_num: '79', pin_signal: VCAP1/LCD_P39/PTC23/LCD_P39_Fault, label: C13/VCAP1}
- {pin_num: '78', pin_signal: VCAP2/LCD_P6/PTC22/LCD_P6_Fault, label: C13/VCAP2}
- {pin_num: '12', pin_signal: VOUT33, label: C11/VOUT33, identifier: VOUT33}
- {pin_num: '33', pin_signal: PTE26/TPM0_CH5/RTC_CLKOUT/USB_CLKIN, label: NC}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
    BOARD_InitDEBUG_UART();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '16', peripheral: SPI0, signal: MISO, pin_signal: LCD_P57/ADC0_DP2/ADC0_SE2/PTE18/SPI0_MOSI/I2C0_SDA/SPI0_MISO/LCD_P57_Fault}
  - {pin_num: '17', peripheral: SPI0, signal: MOSI, pin_signal: LCD_P58/ADC0_DM2/ADC0_SE6a/PTE19/SPI0_MISO/I2C0_SCL/SPI0_MOSI/LCD_P58_Fault}
  - {pin_num: '14', peripheral: SPI0, signal: PCS0, pin_signal: LCD_P55/ADC0_DP1/ADC0_SE1/PTE16/SPI0_PCS0/UART2_TX/TPM_CLKIN0/LCD_P55_Fault}
  - {pin_num: '15', peripheral: SPI0, signal: SCK, pin_signal: LCD_P56/ADC0_DM1/ADC0_SE5a/PTE17/SPI0_SCK/UART2_RX/TPM_CLKIN1/LPTMR0_ALT3/LCD_P56_Fault}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTE16 (pin 14) is configured as SPI0_PCS0 */
    PORT_SetPinMux(PORTE, 16U, kPORT_MuxAlt2);

    /* PORTE17 (pin 15) is configured as SPI0_SCK */
    PORT_SetPinMux(PORTE, 17U, kPORT_MuxAlt2);

    /* PORTE18 (pin 16) is configured as SPI0_MISO */
    PORT_SetPinMux(PORTE, 18U, kPORT_MuxAlt5);

    /* PORTE19 (pin 17) is configured as SPI0_MOSI */
    PORT_SetPinMux(PORTE, 19U, kPORT_MuxAlt5);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitACCEL:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitACCEL
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitACCEL(void)
{
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitDEBUG_UART:
- options: {callFromInitBoot: 'true', prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '36', peripheral: UART0, signal: TX, pin_signal: TSI0_CH3/PTA2/UART0_TX/TPM2_CH1, direction: OUTPUT, slew_rate: fast, open_drain: disable, pull_select: down,
    pull_enable: disable}
  - {pin_num: '35', peripheral: UART0, signal: RX, pin_signal: TSI0_CH2/PTA1/UART0_RX/TPM2_CH0, slew_rate: fast, pull_select: down, pull_enable: disable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitDEBUG_UART
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitDEBUG_UART(void)
{
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);

    const port_pin_config_t DEBUG_UART_RX = {/* Internal pull-up/down resistor is disabled */
                                             kPORT_PullDisable,
                                             /* Fast slew rate is configured */
                                             kPORT_FastSlewRate,
                                             /* Passive filter is disabled */
                                             kPORT_PassiveFilterDisable,
                                             /* Low drive strength is configured */
                                             kPORT_LowDriveStrength,
                                             /* Pin is configured as UART0_RX */
                                             kPORT_MuxAlt2};
    /* PORTA1 (pin 35) is configured as UART0_RX */
    PORT_SetPinConfig(BOARD_DEBUG_UART_RX_PORT, BOARD_DEBUG_UART_RX_PIN, &DEBUG_UART_RX);

    const port_pin_config_t DEBUG_UART_TX = {/* Internal pull-up/down resistor is disabled */
                                             kPORT_PullDisable,
                                             /* Fast slew rate is configured */
                                             kPORT_FastSlewRate,
                                             /* Passive filter is disabled */
                                             kPORT_PassiveFilterDisable,
                                             /* Low drive strength is configured */
                                             kPORT_LowDriveStrength,
                                             /* Pin is configured as UART0_TX */
                                             kPORT_MuxAlt2};
    /* PORTA2 (pin 36) is configured as UART0_TX */
    PORT_SetPinConfig(BOARD_DEBUG_UART_TX_PORT, BOARD_DEBUG_UART_TX_PIN, &DEBUG_UART_TX);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK | SIM_SOPT5_UART0RXSRC_MASK | SIM_SOPT5_UART0ODE_MASK)))

                  /* UART0 Transmit Data Source Select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX)

                  /* UART0 Receive Data Source Select: UART_RX pin. */
                  | SIM_SOPT5_UART0RXSRC(SOPT5_UART0RXSRC_UART_RX)

                  /* UART0 Open Drain Enable: Open drain is disabled on UART0. */
                  | SIM_SOPT5_UART0ODE(SOPT5_UART0ODE_DISABLED));
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitButtons:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '73', peripheral: GPIOC, signal: 'GPIO, 3', pin_signal: LCD_P23/PTC3/LLWU_P7/UART1_RX/TPM0_CH2/CLKOUT/I2S0_TX_BCLK/LCD_P23_Fault, direction: INPUT,
    slew_rate: fast, pull_select: up, pull_enable: enable}
  - {pin_num: '88', peripheral: GPIOC, signal: 'GPIO, 12', pin_signal: LCD_P32/PTC12/TPM_CLKIN0/LCD_P32_Fault, direction: INPUT, slew_rate: fast, pull_select: up,
    pull_enable: enable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitButtons
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitButtons(void)
{
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);

    gpio_pin_config_t SW1_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTC3 (pin 73)  */
    GPIO_PinInit(BOARD_SW1_GPIO, BOARD_SW1_PIN, &SW1_config);

    gpio_pin_config_t SW3_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTC12 (pin 88)  */
    GPIO_PinInit(BOARD_SW3_GPIO, BOARD_SW3_PIN, &SW3_config);

    const port_pin_config_t SW3 = {/* Internal pull-up resistor is enabled */
                                   kPORT_PullUp,
                                   /* Fast slew rate is configured */
                                   kPORT_FastSlewRate,
                                   /* Passive filter is disabled */
                                   kPORT_PassiveFilterDisable,
                                   /* Low drive strength is configured */
                                   kPORT_LowDriveStrength,
                                   /* Pin is configured as PTC12 */
                                   kPORT_MuxAsGpio};
    /* PORTC12 (pin 88) is configured as PTC12 */
    PORT_SetPinConfig(BOARD_SW3_PORT, BOARD_SW3_PIN, &SW3);

    const port_pin_config_t SW1 = {/* Internal pull-up resistor is enabled */
                                   kPORT_PullUp,
                                   /* Fast slew rate is configured */
                                   kPORT_FastSlewRate,
                                   /* Passive filter is disabled */
                                   kPORT_PassiveFilterDisable,
                                   /* Low drive strength is configured */
                                   kPORT_LowDriveStrength,
                                   /* Pin is configured as PTC3 */
                                   kPORT_MuxAsGpio};
    /* PORTC3 (pin 73) is configured as PTC3 */
    PORT_SetPinConfig(BOARD_SW1_PORT, BOARD_SW1_PIN, &SW1);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitLEDs:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '26', peripheral: GPIOE, signal: 'GPIO, 29', pin_signal: CMP0_IN5/ADC0_SE4b/PTE29/TPM0_CH2/TPM_CLKIN0, direction: OUTPUT, gpio_init_state: 'true', slew_rate: slow,
    pull_select: down, pull_enable: disable}
  - {pin_num: '98', peripheral: GPIOD, signal: 'GPIO, 5', pin_signal: LCD_P45/ADC0_SE6b/PTD5/SPI1_SCK/UART2_TX/TPM0_CH5/LCD_P45_Fault, direction: OUTPUT, gpio_init_state: 'true',
    slew_rate: slow, pull_select: down, pull_enable: disable}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitLEDs
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitLEDs(void)
{
    /* Port D Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortD);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    gpio_pin_config_t LED_GREEN_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };
    /* Initialize GPIO functionality on pin PTD5 (pin 98)  */
    GPIO_PinInit(BOARD_LED_GREEN_GPIO, BOARD_LED_GREEN_PIN, &LED_GREEN_config);

    gpio_pin_config_t LED_RED_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
    };
    /* Initialize GPIO functionality on pin PTE29 (pin 26)  */
    GPIO_PinInit(BOARD_LED_RED_GPIO, BOARD_LED_RED_PIN, &LED_RED_config);

    const port_pin_config_t LED_GREEN = {/* Internal pull-up/down resistor is disabled */
                                         kPORT_PullDisable,
                                         /* Slow slew rate is configured */
                                         kPORT_SlowSlewRate,
                                         /* Passive filter is disabled */
                                         kPORT_PassiveFilterDisable,
                                         /* Low drive strength is configured */
                                         kPORT_LowDriveStrength,
                                         /* Pin is configured as PTD5 */
                                         kPORT_MuxAsGpio};
    /* PORTD5 (pin 98) is configured as PTD5 */
    PORT_SetPinConfig(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, &LED_GREEN);

    const port_pin_config_t LED_RED = {/* Internal pull-up/down resistor is disabled */
                                       kPORT_PullDisable,
                                       /* Slow slew rate is configured */
                                       kPORT_SlowSlewRate,
                                       /* Passive filter is disabled */
                                       kPORT_PassiveFilterDisable,
                                       /* Low drive strength is configured */
                                       kPORT_LowDriveStrength,
                                       /* Pin is configured as PTE29 */
                                       kPORT_MuxAsGpio};
    /* PORTE29 (pin 26) is configured as PTE29 */
    PORT_SetPinConfig(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, &LED_RED);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitTouch:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitTouch
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitTouch(void)
{
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitUSB:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '11', peripheral: USB0, signal: DM, pin_signal: USB0_DM}
  - {pin_num: '10', peripheral: USB0, signal: DP, pin_signal: USB0_DP}
  - {pin_num: '13', peripheral: USB0, signal: VREGIN, pin_signal: VREGIN}
  - {pin_num: '12', peripheral: USB0, signal: VOUT33, pin_signal: VOUT33}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitUSB
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitUSB(void)
{
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitSegment_LCD:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitSegment_LCD
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitSegment_LCD(void)
{
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitOSC:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '51', peripheral: OSC0, signal: XTAL0, pin_signal: XTAL0/PTA19/UART1_TX/TPM_CLKIN1/LPTMR0_ALT1, slew_rate: no_init, pull_select: no_init, pull_enable: no_init}
  - {pin_num: '50', peripheral: OSC0, signal: EXTAL0, pin_signal: EXTAL0/PTA18/UART1_RX/TPM_CLKIN0, slew_rate: no_init, pull_select: no_init, pull_enable: no_init}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitOSC
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitOSC(void)
{
    /* Port A Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortA);

    /* PORTA18 (pin 50) is configured as EXTAL0 */
    PORT_SetPinMux(BOARD_EXTAL0_PORT, BOARD_EXTAL0_PIN, kPORT_PinDisabledOrAnalog);

    /* PORTA19 (pin 51) is configured as XTAL0 */
    PORT_SetPinMux(BOARD_XTAL0_PORT, BOARD_XTAL0_PIN, kPORT_PinDisabledOrAnalog);
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitMAG:
- options: {prefix: BOARD_, coreID: core0, enableClock: 'true'}
- pin_list: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitMAG
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitMAG(void)
{
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
