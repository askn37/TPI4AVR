/**
 * @file TPI4AVR.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "configuration.h"
#include "JTAG2.h"
#include "TPI.h"
#include "sys.h"
#include "timer.h"
#include "abort.h"
#include "dbg.h"

// Prototypes
namespace {
  void setup (void);
  void loop (void);
  void process_command (void);
}

namespace {
  inline void setup (void) {
    SYS::setup();
    TIMER::setup();

    /* global interrupt enable */
    sei();

    #ifdef DEBUG_USE_USART
    DBG::setup();
    DBG::print("<<< TPI4AVR >>>");
    DBG::print("F_CPU:");
    DBG::print_dec(F_CPU);
    #endif

    ABORT::setup();
    TPI::setup();
    JTAG2::setup();
    ABORT::stop_timer();

    // #define DEBUG_PWM
    #if defined(DEBUG_PWM)
    SYS::pgen_enable();
    SYS::hvp_enable();
    TPI::trst_disable();
    if (setjmp(ABORT::CONTEXT) == 0) {
      ABORT::set_make_interrupt(ABORT::CONTEXT);
      while (1) {
        TIMER::delay(2000);
        SYS::hven_disable();
        TIMER::delay(2000);
        SYS::hven_enable();
        #ifdef DEBUG_USE_USART
        DBG::newline();
        DBG::print_dec(TIMER::millis());
        #endif
      }
    }
    else {
      SYS::pgen_disable();
      _PROTECTED_WRITE(RSTCTRL.SWRR,
        #if defined(RSTCTRL_SWRE_bm)
        RSTCTRL_SWRE_bm
        #elif defined(RSTCTRL_SWRST_bm)
        RSTCTRL_SWRST_bm
        #else
        #assert "This RSTCTRL defined is not supported"
        #endif
      );
    }
    #endif

    for (uint8_t i = 0; i < 2; i++) {
      SYS::pgen_disable();
      TIMER::delay(100);
      SYS::pgen_enable();
      TIMER::delay(100);
    }

    SYS::pgen_disable();
  }

  inline void loop (void) {
    static uint8_t sig_interrupt = 0;
    volatile uint8_t abort_result;

    #ifdef DEBUG_USE_USART
    if (!JTAG2::CONTROL) DBG::print("Ready");
    #endif

    /* sensing JTAG2 recieve */
    if ((abort_result = setjmp(ABORT::CONTEXT)) == 0) {
      if (sig_interrupt) ABORT::start_timer(ABORT::CONTEXT, MAKE_SIGNAL_SR_MS);
      ABORT::set_make_interrupt(ABORT::CONTEXT);

      /* JTAG2 packet receive and parse */
      while (!JTAG2::packet_receive());
      sig_interrupt = 0;
      SYS::pgen_enable();

      /* run JTAG2 command */
      process_command();
    }

    /* RTS/DTR signal abort */
    else if (abort_result == 1) {
      ABORT::stop_timer();
      sig_interrupt = 1;

      #ifdef DEBUG_USE_USART
      DBG::print("!MAKE");  // RTS interrupt abort
      #endif

      TPI::CONTROL = JTAG2::CONTROL = 0x0;
      JTAG2::sign_off();
      JTAG2::change_baudrate(false);
      SYS::pgen_enable();
      return;
    }

    /* JTAG2 timeout abort */
    else {
      ABORT::stop_timer();

      /* interrupt ? */
      if (sig_interrupt) {
        if (bit_is_clear(MAKE_SIG_PORT.IN, MAKE_PIN)) {
          _PROTECTED_WRITE(RSTCTRL.SWRR,
            #if defined(RSTCTRL_SWRE_bm)
            RSTCTRL_SWRE_bm
            #elif defined(RSTCTRL_SWRST_bm)
            RSTCTRL_SWRST_bm
            #else
            #assert "This RSTCTRL defined is not supported"
            #endif
          );
          while (true);
        }
        TPI::runtime(TPI::TPI_CMD_TARGET_RESET);
      }

      if (JTAG2::CONTROL & JTAG2::HOST_SIGN_ON) {
        #ifdef DEBUG_USE_USART
        DBG::print("!H_TO");  // HOST timeout
        #endif
        JTAG2::set_response(JTAG2::RSP_FAILED);
        JTAG2::answer_transfer();
      }
      JTAG2::sign_off();
      if (JTAG2::CONTROL & JTAG2::CHANGE_BAUD) {
        JTAG2::change_baudrate(false);
        JTAG2::clear_control(JTAG2::CHANGE_BAUD);
      }

      JTAG2::answer_after_change();
      JTAG2::CONTROL = TPI::CONTROL = sig_interrupt = 0;
      SYS::pgen_disable();
      TPI::trst_disable();
    }
  }

  /* JTAG2 command to UPDI action convert */
  inline void process_command (void) {
    #ifdef DEBUG_USE_USART
    DBG::print("%");
    DBG::print_dec(JTAG2::packet.number);
    #endif
    switch (JTAG2::packet.body[0]) {
      case JTAG2::CMND_SIGN_OFF : {
        ABORT::stop_timer();
        #ifdef DEBUG_USE_USART
        DBG::print(">CMND_SIGN_OFF", false);
        #endif
        JTAG2::sign_off();
        JTAG2::set_response(JTAG2::RSP_OK);
        break;
      }
      case JTAG2::CMND_GET_SIGN_ON : {
        #ifdef DEBUG_USE_USART
        DBG::print(">GET_SIGN_ON", false);
        #endif
        JTAG2::sign_on();
        ABORT::stop_timer();
        break;
      }
      case JTAG2::CMND_SET_PARAMETER : {
        #ifdef DEBUG_USE_USART
        DBG::print(">SET_P", false);
        #endif
        JTAG2::set_parameter(); break;
      }
      case JTAG2::CMND_GET_PARAMETER : {
        #ifdef DEBUG_USE_USART
        DBG::print(">GET_P", false);
        #endif
        JTAG2::get_parameter(); break;
      }
      case JTAG2::CMND_SET_DEVICE_DESCRIPTOR : {
        #ifdef DEBUG_USE_USART
        DBG::print(">SET_DEV", false);
        #endif
        JTAG2::set_device_descriptor(); break;
      }
      case JTAG2::CMND_ENTER_PROGMODE : {
        #ifdef DEBUG_USE_USART
        DBG::print(">E_PRG", false);
        #endif
        JTAG2::set_response(
          TPI::runtime(TPI::TPI_CMD_ENTER)
          ? JTAG2::RSP_OK
          : JTAG2::RSP_ILLEGAL_MCU_STATE
        ); break;
      }
      case JTAG2::CMND_LEAVE_PROGMODE : {
        #ifdef DEBUG_USE_USART
        DBG::print(">L_PRG", false);
        #endif
        JTAG2::set_response(
          TPI::runtime(TPI::TPI_CMD_LEAVE)
          ? JTAG2::RSP_OK
          : JTAG2::RSP_ILLEGAL_MCU_STATE
        ); break;
      }
      case JTAG2::CMND_READ_MEMORY : {
        #ifdef DEBUG_USE_USART
        DBG::print(">R_MEM", false);
        #endif
        if (JTAG2::check_sig()) break;
        if (!TPI::runtime(TPI::TPI_CMD_READ_MEMORY)) {
          JTAG2::set_response(JTAG2::RSP_ILLEGAL_MCU_STATE);
        }
        break;
      }
      case JTAG2::CMND_WRITE_MEMORY : {
        #ifdef DEBUG_USE_USART
        DBG::print(">W_MEM", false);
        #endif
        JTAG2::set_response(
          TPI::runtime(TPI::TPI_CMD_WRITE_MEMORY)
          ? JTAG2::RSP_OK
          : JTAG2::RSP_ILLEGAL_MEMORY_TYPE
        ); break;
      }
      case JTAG2::CMND_XMEGA_ERASE : {
        #ifdef DEBUG_USE_USART
        DBG::print(">X_ERA", false);
        DBG::print(" ET=", false);
        DBG::write_hex(JTAG2::packet.body[1]);
        DBG::print(" SA=", false);
        DBG::print_hex(*((uint32_t*)&JTAG2::packet.body[2]));
        #endif
        JTAG2::set_response(
          TPI::runtime(TPI::TPI_CMD_ERASE)
          ? JTAG2::RSP_OK
          : JTAG2::RSP_ILLEGAL_MCU_STATE
        ); break;
      }
      case JTAG2::CMND_RESET : {
        #ifdef DEBUG_USE_USART
        DBG::print(">RST=", false);
        DBG::write_hex(JTAG2::packet.body[1]);
        #endif
        JTAG2::set_response(
          TPI::runtime(TPI::TPI_CMD_TARGET_RESET)
          ? JTAG2::RSP_OK
          : JTAG2::RSP_ILLEGAL_MCU_STATE
        ); break;
      }

      /* no support command, dummy response, all ok */
      case JTAG2::CMND_GET_SYNC :
      case JTAG2::CMND_GO : {
        #ifdef DEBUG_USE_USART
        if (JTAG2::packet.body[0] == JTAG2::CMND_GET_SYNC) DBG::print(">GET_SYNC", false);
        else if (JTAG2::packet.body[0] == JTAG2::CMND_GO)  DBG::print(">GO", false);
        #endif
        JTAG2::set_response(JTAG2::RSP_OK); break;
      }

      /* undefined command ignore, not response */
      default : {
        #ifdef DEBUG_USE_USART
        DBG::print(">!?", false);
        DBG::write_hex(JTAG2::packet.body[0]);
        #endif
        return;
      }
    }

    /* JTAG2 command response */
    JTAG2::answer_transfer();

    /* response after change action */
    SYS::pgen_disable();
    JTAG2::answer_after_change();
  }
}

int main (void) {
  setup();
  for (;;) loop();
}

// end of code
