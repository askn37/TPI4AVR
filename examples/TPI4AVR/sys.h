/**
 * @file sys.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <setjmp.h>
#include "configuration.h"

#if defined(PGEN_USE_PORTA)
  #define PGEN_PORT PORTA
#elif defined(PGEN_USE_PORTB)
  #define PGEN_PORT PORTB
#elif defined(PGEN_USE_PORTC)
  #define PGEN_PORT PORTC
#elif defined(PGEN_USE_PORTD)
  #define PGEN_PORT PORTD
#elif defined(PGEN_PIN_USE_PORTE)
  #define PGEN_PORT PORTE
#elif defined(PGEN_USE_PORTF)
  #define PGEN_PORT PORTF
#elif defined(PGEN_USE_PORTG)
  #define PGEN_PORT PORTG
#else
  /* fallback */
  #define PGEN_USE_PORTA
  #define PGEN_PORT PORTA
#endif

#if defined(MAKE_USE_PORTA)
  #define MAKE_SIG_PORT PORTA
#elif defined(MAKE_USE_PORTB)
  #define MAKE_SIG_PORT PORTB
#elif defined(MAKE_USE_PORTC)
  #define MAKE_SIG_PORT PORTC
#elif defined(MAKE_USE_PORTD)
  #define MAKE_SIG_PORT PORTD
#elif defined(MAKE_USE_PORTE)
  #define MAKE_SIG_PORT PORTE
#elif defined(MAKE_USE_PORTF)
  #define MAKE_SIG_PORT PORTF
#elif defined(MAKE_USE_PORTG)
  #define MAKE_SIG_PORT PORTG
#else
  /* fallback */
  #define MAKE_USE_PORTF
  #define MAKE_SIG_PORT PORTF
#endif

#if defined(HVEN_USE_PORTA)
  #define HVEN_PORT PORTA
#elif defined(HVENE_USE_PORTB)
  #define HVEN_PORT PORTB
#elif defined(HVENE_USE_PORTC)
  #define HVEN_PORT PORTC
#elif defined(HVNE_USE_PORTD)
  #define HVEN_PORT PORTD
#elif defined(HVENE_USE_PORTE)
  #define HVEN_PORT PORTE
#elif defined(HVENE_USE_PORTF)
  #define HVEN_PORT PORTF
#elif defined(HVENE_USE_PORTG)
  #define HVEN_PORT PORTG
#else
  /* fallback */
  #define HVEN_USE_PORTA
  #define HVEN_PORT PORTA
#endif

#if defined(HVP_PIN_USE_PORTA)
  #define HV_PWM_PORT PORTA
#elif defined(HVP_PIN_USE_PORTB)
  #define HV_PWM_PORT PORTB
#elif defined(HVP_PIN_USE_PORTC)
  #define HV_PWM_PORT PORTC
#elif defined(HVP_PIN_USE_PORTD)
  #define HV_PWM_PORT PORTD
#elif defined(HVP_PIN_USE_PORTE)
  #define HV_PWM_PORT PORTE
#elif defined(HVP_PIN_USE_PORTF)
  #define HV_PWM_PORT PORTF
#elif defined(HVP_PIN_USE_PORTG)
  #define HV_PWM_PORT PORTG
#else
  /* fallback */
  #define HVEN_USE_PORTA
  #define HVENE_PORT PORTA
#endif

#define HV_PWM_CLK (F_CPU/400000)

namespace SYS {
  void setup (void);
  void pump_timer_init (void);

  /* JTAG tri-state buffer control */
  inline void pgen_disable (void) {
    PGEN_PORT.DIRSET =
    PGEN_PORT.OUTCLR = _BV(PGEN_PIN);
  }
  inline void pgen_enable (void) {
    PGEN_PORT.DIRCLR = _BV(PGEN_PIN);
  }

  /* HV state control */
  inline void hven_disable (void) {
    HVEN_PORT.OUTCLR = _BV(HVEN_PIN);
  }
  inline void hven_enable (void) {
    HVEN_PORT.OUTSET = _BV(HVEN_PIN);
  }

  inline void hvp_enable (void) {
    #if defined(HVP_PIN_USE_OUTPUT)
    TCA0.SPLIT.CTRLA |= TCA_SPLIT_ENABLE_bm;
    #endif
  }
  inline void hvp_disable (void) {
    #if defined(HVP_PIN_USE_OUTPUT)
    TCA0.SPLIT.CTRLA &= ~(TCA_SPLIT_ENABLE_bm);
    #endif
  }
}

// end of code
