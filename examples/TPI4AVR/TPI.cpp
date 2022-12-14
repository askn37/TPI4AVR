/**
 * @file TPI.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <avr/interrupt.h>
#include <setjmp.h>
#include "sys.h"
#include "TPI.h"
#include "JTAG2.h"
#include "NVM.h"
#include "usart.h"
#include "timer.h"
#include "abort.h"
#include "dbg.h"

uint8_t TPI::CONTROL;

namespace TPI {
  const static uint8_t nvmprog_key[9] = { TPI::TPI_SKEY, 0xFF, 0x88, 0xD8, 0xCD, 0x45, 0xAB, 0x89, 0x12 };
  volatile uint8_t LASTL;
  volatile uint8_t LASTH;
}

void TPI::setup (void) {
  TPI::CONTROL = 0x00;

  #ifdef TPI_USART_USE_PORTMUX
  PORTMUX.USARTROUTEA |= TPI_USART_USE_PORTMUX;
  #endif

  PIN_CTRL(TPI_USART_USE_PORT,TCLK_PIN) =
  PIN_CTRL(TPI_USART_USE_PORT,TDAT_PIN) = PORT_PULLUPEN_bm | PORT_ISC_INTDISABLE_gc;

  PIN_CTRL(TPI_USART_USE_PORT,TRST_PIN) = PORT_PULLUPEN_bm | PORT_ISC_INPUT_DISABLE_gc;
  TPI_USART_USE_PORT.OUTCLR =
  TPI_USART_USE_PORT.DIRCLR = _BV(TRST_PIN);

  USART::setup(
    &TPI_USART_MODULE,
    USART::calc_baudrate_synchronous(TPI_USART_BAUDRATE),
    (USART_LBME_bm | 0x03),
    (USART_TXEN_bm | USART_RXEN_bm | USART_ODME_bm | USART_RXMODE_NORMAL_gc),
    (USART_CHSIZE_8BIT_gc | USART_PMODE_EVEN_gc | USART_CMODE_SYNCHRONOUS_gc | USART_SBMODE_2BIT_gc)
  );
}

void TPI::idle_clock (const uint16_t clock) {
  for (uint16_t i = 0; i < clock; i++) {
    loop_until_bit_is_set(TPI_USART_USE_PORT.IN, TCLK_PIN);
    loop_until_bit_is_clear(TPI_USART_USE_PORT.IN, TCLK_PIN);
  }
}

bool TPI::BREAK (void) {
  loop_until_bit_is_set(TPI_USART_MODULE.STATUS, USART_DREIF_bp);

  TPI_USART_USE_PORT.OUTCLR = _BV(TDAT_PIN);
  TPI_USART_USE_PORT.DIRSET = _BV(TDAT_PIN);
  TPI::idle_clock(24);
  TPI_USART_USE_PORT.DIRCLR = _BV(TDAT_PIN);
  TPI::idle_clock(12);

  /* drop receive buffer */
  while (bit_is_set(TPI_USART_MODULE.STATUS, USART_RXCIF_bp)) {
    TPI::LASTL = TPI_USART_MODULE.RXDATAL;
  }
  #ifdef DEBUG_USE_USART
  DBG::print("[BRK]", false);
  #endif
  return true;
}

uint8_t TPI::RECV (void) {
  /* receive symbol */
  loop_until_bit_is_set(TPI_USART_MODULE.STATUS, USART_RXCIF_bp);
  TPI::LASTH = TPI_USART_MODULE.RXDATAH ^ 0x80;

  #ifdef DEBUG_TPI_LOOPBACK

  TPI::LASTL = TPI_USART_MODULE.RXDATAL;
  DBG::write('<'); DBG::write_hex(TPI::LASTL);
  return TPI::LASTL;

  #else

  return TPI::LASTL = TPI_USART_MODULE.RXDATAL;

  #endif
}

bool TPI::SEND (const uint8_t data) {
  loop_until_bit_is_set(TPI_USART_MODULE.STATUS, USART_DREIF_bp);

  #ifdef DEBUG_TPI_LOOPBACK
  DBG::write('>');
  DBG::write_hex(data);
  #endif

  /* sending symbol */
  TPI_USART_MODULE.STATUS |= USART_TXCIF_bm;
  TPI_USART_MODULE.TXDATAL = data;
  loop_until_bit_is_set(TPI_USART_MODULE.STATUS, USART_TXCIF_bp);
  // TPI::idle_clock(2);

  /* loopback symbol verify */
  bool _r = data == TPI::RECV();
  if (!_r) TPI::LASTH |= 0x20;
  return _r;
}

uint8_t TPI::SLDCS (const uint8_t addr) {
  TPI::SEND(TPI::TPI_SLDCS | addr);
  uint8_t data = TPI::RECV();
  return data;
}

bool TPI::SSTCS (const uint8_t addr, const uint8_t data) {
  return TPI::SEND(TPI::TPI_SSTCS | addr) && TPI::SEND(data);
}

bool TPI::SOUT (const uint8_t addr, const uint8_t data) {
  return TPI::SEND(TPI::TPI_SOUT | addr) && TPI::SEND(data);
}

uint8_t TPI::SIN (const uint8_t addr) {
  TPI::SEND(TPI::TPI_SIN | addr);
  uint8_t data = TPI::RECV();
  return data;
}

bool TPI::SSTPR (const uint16_t addr) {
  return TPI::SEND(TPI::TPI_SSTPR_LOW)
    && TPI::SEND(addr & 0xFF)
    && TPI::SEND(TPI::TPI_SSTPR_HIGH)
    && TPI::SEND(addr >> 8);
}

uint8_t TPI::SLD (void) {
  TPI::SEND(TPI::TPI_SLD_INC);
  uint8_t data = TPI::RECV();
  return data;
}

bool TPI::SST (const uint8_t data) {
  return TPI::SEND(TPI::TPI_SST_INC) && TPI::SEND(data);
}

bool TPI::enter_nvmprog (void) {
  /* activate NVMPROG mode */
  while (TPI::SLDCS(TPI::TPISR) != 0x02) {
    #ifdef DEBUG_USE_USART
    DBG::print("[KEY]", false);
    #endif
    for (uint8_t i = 0; i < (uint8_t)sizeof(nvmprog_key); i++) {
      if (!TPI::SEND(nvmprog_key[i])) {
        #ifdef DEBUG_USE_USART
        DBG::print("[!KEY]", false);
        #endif
        return false;
      }
      TPI::idle_clock(4);
    }
  };
  #ifdef DEBUG_USE_USART
  DBG::print("[NVM]", false);
  #endif
  TPI::set_control(TPI::ENABLE_NVMPG);
  return true;
}

bool TPI::tpi_activate (bool use_hv) {
  /* Switch active TPI interface */
  if (!TPI::is_control(TPI::TPI_ACTIVE) || !TPI::is_control(TPI::ENABLE_NVMPG)) {

    SYS::hven_disable();
    SYS::pgen_enable();
    TPI::clear_control(TPI::HV_ENABLE);

    #ifdef DEBUG_USE_USART
    DBG::print("[TRST_ON]", false);
    #endif

    /* Target Reset HIGH */
    TPI::trst_enable();
    TPI::trst_disable();
    TPI::trst_high();
    TIMER::delay(8);

    /* Target Reset LOW */
    TPI::trst_enable();
    TIMER::delay_us(4);

    // use_hv = true;
    if (use_hv) {
      #ifdef DEBUG_USE_USART
      DBG::print("[HV_ON]", false);
      #endif

      SYS::hvp_enable();
      #if defined(HVP_STARTUP_DELAY_MS) && HVP_STARTUP_DELAY_MS > 0
      TIMER::delay(HVP_STARTUP_DELAY_MS);
      #endif
      TPI::trst_disable();
      SYS::hven_enable();
      TPI::set_control(TPI::HV_ENABLE);
      JTAG2::clear_control(JTAG2::ANS_FAILED);
    }

    /* IDLE */
    TPI::tpi_enable();
    TPI::idle_clock(32);

    while (TPI::SLDCS(TPI::TPIIR) != 0x80);

    if (!TPI::SSTCS(TPI::TPIPCR, TPI::TPIPCR_GT2)) {
      #ifdef DEBUG_USE_USART
      DBG::print("[!GT]", false);
      #endif
      return false;
    }

    #ifdef DEBUG_USE_USART
    DBG::print("[T_ON]", false);
    #endif

    TPI::set_control(TPI::TPI_ACTIVE);
  }

  return TPI::enter_nvmprog();
}

bool TPI::enter_tpi (bool use_hv) {
  volatile bool _result = false;
  for (uint8_t i = 0; i <= 2; i++) {
    if (setjmp(ABORT::CONTEXT) == 0) {
      ABORT::start_timer(ABORT::CONTEXT, TPI_ABORT_MS);
      _result = TPI::tpi_activate(use_hv);
      if (_result) break;
    }
    ABORT::stop_timer();
  }
  return _result;
}

bool TPI::leave_tpi (void) {
  if (TPI::is_control(TPI::ENABLE_NVMPG)) {
    TPI::clear_control(TPI::ENABLE_NVMPG);
    TPI::SSTCS(TPI::TPISR, 0x00);
    #ifdef DEBUG_USE_USART
    DBG::print("[T_OF]", false);
    #endif
  }
  return TPI::target_reset();
}

bool TPI::target_reset (void) {
  TPI::clear_control(TPI::TPI_ACTIVE | TPI::HV_ENABLE);
  SYS::hven_disable();
  SYS::hvp_disable();
  TPI::tpi_disable();
  TPI::trst_high();
  TIMER::delay(8);
  TPI::trst_enable();
  TIMER::delay_us(4);
  TPI::trst_disable();
  return true;
}

/* TPI action */
bool TPI::runtime (uint8_t tpi_cmd) {
  volatile bool _result = false;
  ABORT::stop_timer();
  TPI::clear_control(TPI::TPI_FALT | TPI::TPI_TIMEOUT);
  if (setjmp(ABORT::CONTEXT) == 0) {
    ABORT::start_timer(ABORT::CONTEXT, TPI_ABORT_MS);
    switch (tpi_cmd) {
      case TPI::TPI_CMD_ENTER : {
        _result = TPI::enter_tpi(); break;
      }
      case TPI::TPI_CMD_LEAVE : {
        _result = TPI::leave_tpi(); break;
      }
      case TPI::TPI_CMD_READ_MEMORY : {
        _result = NVM::read_memory(); break;
      }
      case TPI::TPI_CMD_WRITE_MEMORY : {
        _result = NVM::write_memory(); break;
      }
      case TPI::TPI_CMD_ERASE : {
        _result = NVM::chip_erase(); break;
      }
      case TPI::TPI_CMD_TARGET_RESET : {
        _result = TPI::target_reset(); break;
      }
    }
    ABORT::stop_timer();
  }
  else {
    ABORT::stop_timer();
    #ifdef DEBUG_USE_USART
    DBG::print("(T_TO)"); // TPI TIMEOUT
    #endif
    TPI::set_control(TPI::TPI_TIMEOUT);
  }
  if (!_result) TPI::set_control(TPI::TPI_FALT);

  #ifdef DEBUG_USE_USART
  if (!_result) {
    DBG::print("(T_NG)", false);
  }
  else {
    DBG::print("(T_OK)", false);
  }
  #endif

  return _result;
}

// end of code
