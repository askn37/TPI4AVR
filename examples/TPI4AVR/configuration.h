/**
 * @file configuration.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "mcu_model.h"

/**************************************
 * PLEASE SELECT TARGET BOARAD PINOUT *
 **************************************/

#if defined(ARDUINO_AVR_UNO_WIFI_REV2)    /* ATmega4809 */
  #define BOARD_ARUDUINO_UNO_WIFI_R2
#elif defined(ARDUINO_AVR_NANO_EVERY)     /* ATmega4809 */
  #define BOARD_ARUDUINO_NANO_EVERY
#elif defined(__AVR_TINY_2X__)
  #define BOARD_ZINNIA_TINYA_2X
#else /* OTHER */
  #define BOARD_ZINNIA_DUINO_AVRDX
#endif

/***********************
 * Using Signal Option *
 * *********************/

/* MAKE signal input PF6/RESET or other PIN */
#define ENABLE_MAKE_SIGNAL_OVER_RESET

/*********************************
 * DEBUG mode using triple USART *
 *********************************/

#ifndef DEBUG
  // #define DEBUG
#endif

/*************************
 * DEBUG mode sub-module *
 *************************/

#ifdef DEBUG
  /* enable dump memory sub-module debug */
  // #define DEBUG_DUMP_MEMORY

  /* enable TPI tx/rx sub-module debug */
  // #define DEBUG_TPI_LOOPBACK

  #define DEBUG_USART_BAUDRATE (230400L / 4)
#endif

/*****************
 * SELECT TIMING *
 *****************/

/* TPI speed max=2000000L default=100000L */
#define TPI_USART_BAUDRATE (250000L)
#define TPI_ABORT_MS 600
#define JTAG_ABORT_MS 12000
#define HVP_STARTUP_DELAY_MS 100
#define MAKE_SIGNAL_SR_MS 700

/*********************
 * HARDWARE SETTINGS *
 *********************/

/*-------------------------------------*
 *                                     *
 * ATtiny824/1624/3224 etc (tinyAVR-2) *
 *                                     *
 *-------------------------------------*/

#if defined(BOARD_ZINNIA_TINYA_2X)

  /***
   * Using Zinnia Tinya bords pinout
   *
   * ATtiny824/1624/3224 etc (tinyAVR-2)
   *
   * Using PORTA PORTB
   *
   *  JTTX : PA1 --> Onboard CH340N RX / target MCU RX
   *  JTRX : PA2 <-- Onboard CH340N TX / target MCU TX
   *  HVP1 : PA4 --> Charge pump drive 1 (optional)
   *  HVP2 : PA5 --> Charge pump drive 2 (optional)
   *  HVEN : PA6 --> HV output enabler   : LOW:Disable, HIGH:Enable
   *  PGEN : PA7 --> TPI reset output
   *  TCLK : PB1 --> TPI clock output
   *  TDAT : PB2 <-> TPI to target MCU   : one-wire transfer/receiver
   *  TRST : PB3 --> Target /RESET control
   *  MAKE : PA3 <-- Reset client signal : LOW:Active, HIGH:Deactive
   *
   * ATtiny10
   *  1:PB0 -- TDAT (PB2)
   *  2:GND
   *  3:PB1 -- TCLK (PB1)
   *  4:PB2
   *  5:VCC
   *  6:PB3 -- TRST (PB3)
   */

  /* Disable DEBUG mode */
  #undef DEBUG

  #define PGEN_USE_PORTA
  #define PGEN_PIN 7
  // #define PGEN_PIN_INVERT

  #define HVEN_USE_PORTA
  #define HVEN_PIN 6
  // #define HVEN_PIN_INVERT

  /* HV pin group using TCA0 split timer */
  #define HVP_PIN_USE_OUTPUT
  #define HVP_PIN_USE_PORTA
  #define HVP1_PIN 4
  #define HVP2_PIN 5

  #define MAKE_USE_PORTA
  #define MAKE_PIN 3

  #define TPI_USART_MODULE USART0
  #define TPI_USART_USE_PORT PORTB
  #define TRST_PIN 3
  #define TDAT_PIN 2
  #define TCLK_PIN 1

  #define JTAG_UART_MODULE USART1
  #define JTAG_UART_USE_PORT PORTA
  #define JTTX_PIN 1
  #define JTRX_PIN 2

  #define ABORT_USE_TIMERB1
  #define MILLIS_USE_TIMERB0

#endif  /* defined(BOARD_ZINNIA_TINYA_2X) */

/*-----------------------------*
 *                             *
 * ATmega4808, AVR128DB32, etc *
 *                             *
 *-----------------------------*/

#if defined(BOARD_ZINNIA_DUINO_AVRDX)

  /***
   * Using Zinnia Duino bords pinout
   *
   * ATmega4808, AVR128DB32, etc
   *
   * Using PORTA PORTC PORTD PORTF
   *
   *  JTRX  : D0:PA1  <-- Onboard CH340N TX / target MCU TX
   *  JTTX  : D1:PA0  --> Onboard CH340N RX / target MCU RX
   *  DBGTX : D3:PF4  --> Outher UART RX
   *  TCLK  : D5:PC2  --> TDI clock output
   *  TRST  : D6:PC1  --> Target /RESET control
   *  TDAT  : D7:PC0  <-> TPI to target MCU             : one-wire transfer/receiver
   *  PGEN  : D10:PA7 --> Programing state              : LOW:Disable, HIGH:Enable
   *  MAKE  : RST:PF6 <-- Reset client signal (default) : LOW:Active, HIGH:Deactive (sense RTS)
   *     or : A0:PD1  <-- Reset client signal (altanate): LOW:Active, HIGH:Deactive (user switch)
   *  HVEN  : A1:PD2  --> HV output enabler             : LOW:Disable, HIGH:Enable
   *  HVP1  : A2:PD3  --> Charge pump drive 1 (optional)
   *  HVP2  : A3:PD4  --> Charge pump drive 2 (optional)
   *
   * ATtiny10
   *  1:PB0 -- TDAT (D7:PC0)
   *  2:GND
   *  3:PB1 -- TCLK (D5:PC2)
   *  4:PB2
   *  5:VCC
   *  6:PB3 -- TRST (D6:PC1)
   */

  #define PGEN_USE_PORTA
  #define PGEN_PIN 7
  // #define PGEN_PIN_INVERT

  #define HVNE_USE_PORTD
  #define HVEN_PIN 2
  // #define HVEN_PIN_INVERT

  /* HV pin group using TCA0 split timer */
  /* using PWM pinnumber 0 ~ 5 (bad 6,7)*/
  /* and AVR DB bad D0 (not output circuit) */
  #define HVP_PIN_USE_OUTPUT
  #define HVP_PIN_USE_PORTD
  #define HVP1_PIN 3
  #define HVP2_PIN 4

  /* Sense RTS/DTR signal input intrrupt */
  #ifdef ENABLE_MAKE_SIGNAL_OVER_RESET
    #define MAKE_USE_PORTF
    #define MAKE_PIN 6
  #else
    #define MAKE_USE_PORTD
    #define MAKE_PIN 1
  #endif

  #define TPI_USART_MODULE USART1
  #define TPI_USART_USE_PORT PORTC
  #define TDAT_PIN 0
  #define TRST_PIN 1
  #define TCLK_PIN 2

  #define JTAG_UART_MODULE USART0
  // #define JTAG_UART_MODULE_PORTMUX (PORTMUX_USART0_DEFAULT_gc)
  #define JTAG_UART_USE_PORT PORTA
  #define JTTX_PIN 0
  #define JTRX_PIN 1

  #ifdef DEBUG
    #define DEBUG_USE_USART
    #define DEBUG_USART_MODULE USART2
    #define DEBUG_USART_MODULE_PORTMUX (PORTMUX_USART2_ALT1_gc)
    #define DEBUG_USART_USE_PORT PORTF
    #define DBGTX_PIN 4
  #endif /* DEBUG */

  #define ABORT_USE_TIMERB1

  #undef  MILLIS_USE_TIMERB2
  #define MILLIS_USE_TIMERB2

#endif  /* defined(BOARD_ZINNIA_DUINO_AVRDX) */

// end of code
