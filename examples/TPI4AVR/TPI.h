/**
 * @file TPI.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include <string.h>
#include "configuration.h"
#include "sys.h"

namespace TPI {
  extern uint8_t CONTROL;
  extern volatile uint8_t LASTL;
  extern volatile uint8_t LASTH;

  /* TPI::CONTROL flags */
  enum tpi_control_e {
      TPI_ACTIVE   = 0x01
    , ENABLE_NVMPG = 0x02
    , CHIP_ERASE   = 0x04
    , HV_ENABLE    = 0x08
    , TPI_TIMEOUT  = 0x40
    , TPI_FALT     = 0x80
  };
  enum tpi_command_e {
      TPI_CMD_ENTER = 1
    , TPI_CMD_LEAVE
    , TPI_CMD_ENTER_PROG
    , TPI_CMD_READ_MEMORY
    , TPI_CMD_WRITE_MEMORY
    , TPI_CMD_TARGET_RESET
    , TPI_CMD_ERASE
  };
  enum tpi_operate_e {
    /* TPI opcode */
      TPI_NULL       = 0b00000000
    , TPI_BREAK      = 0b00000000
    , TPI_SLD_IMD    = 0b00100000
    , TPI_SLD_INC    = 0b00100100
    , TPI_SST_IMD    = 0b01100000
    , TPI_SST_INC    = 0b01100100
    , TPI_SSTPR_LOW  = 0b01101000
    , TPI_SSTPR_HIGH = 0b01101001
    , TPI_SIN        = 0b00010000
    , TPI_SOUT       = 0b10010000
    , TPI_SLDCS      = 0b10000000
    , TPI_SSTCS      = 0b11000000
    , TPI_SKEY       = 0b11100000
    , NVMCSR_REG     = 0x62   /* $32 */
    , NVMCMD_REG     = 0x63   /* $33 */
  };
  enum tpi_status_e {
    /* TPI CS REGISTER */
      TPISR          = 0b00000000
    , TPIPCR         = 0b00000010
    , TPIIR          = 0b00001111
  };
  enum tpi_bitset_e {
      TPIIR_TPIIC    = 0b10000000
    , TPIPCR_GT128   = 0b00000000
    , TPIPCR_GT64    = 0b00000001
    , TPIPCR_GT32    = 0b00000010
    , TPIPCR_GT16    = 0b00000011
    , TPIPCR_GT8     = 0b00000100
    , TPIPCR_GT4     = 0b00000101
    , TPIPCR_GT2     = 0b00000110
    , TPIPCR_GT0     = 0b00000111
    , TPIISR_NVMEN   = 0b00000010
  };

  inline void trst_disable (void) {
    TPI_USART_USE_PORT.OUTSET =
    TPI_USART_USE_PORT.DIRCLR = _BV(TRST_PIN);
  }
  inline void trst_enable (void) {
    TPI_USART_USE_PORT.OUTCLR =
    TPI_USART_USE_PORT.DIRSET = _BV(TRST_PIN);
  }
  inline void trst_high (void) {
    TPI_USART_USE_PORT.OUTSET =
    TPI_USART_USE_PORT.DIRSET = _BV(TRST_PIN);
  }

  inline void tpi_enable (void) {
    TPI_USART_USE_PORT.OUTSET =
    TPI_USART_USE_PORT.DIRSET = _BV(TCLK_PIN);
  }

  inline void tpi_disable (void) {
    TPI_USART_USE_PORT.OUTCLR =
    TPI_USART_USE_PORT.DIRCLR = _BV(TCLK_PIN);
  }

  void setup (void);
  void idle_clock (const uint16_t clock);

  inline uint8_t is_control (uint8_t value) {
    return TPI::CONTROL & value;
  }
  inline void set_control (uint8_t value) {
    TPI::CONTROL |= value;
  }
  inline void clear_control (uint8_t value) {
    TPI::CONTROL &= ~(value);
  }

  bool BREAK (void);
  bool SEND (const uint8_t data);
  uint8_t RECV (void);

  uint8_t SLDCS (const uint8_t addr);
  bool SSTCS (const uint8_t addr, const uint8_t data);

  bool SOUT (const uint8_t addr, const uint8_t data);
  uint8_t SIN (const uint8_t addr);

  bool SSTPR (const uint16_t addr);
  uint8_t SLD (void);
  bool SST (const uint8_t data);

  bool enter_nvmprog (void);
  bool tpi_activate (bool use_hv = false);
  bool enter_tpi (bool use_hv = false);
  bool leave_tpi (void);
  bool target_reset (void);

  bool runtime (uint8_t tpi_cmnd);
}

// end of code
