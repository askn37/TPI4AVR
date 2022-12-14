/**
 * @file NVM.h
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <string.h>
#include <setjmp.h>
#include "configuration.h"

namespace NVM {
  enum tinyrc_addr_e {
      LOCK_TOP   = 0x3F00
    , SYSCFG_TOP = 0x3F40
    , CALIB_TOP  = 0x3F80
    , SIGROW_TOP = 0x3FC0
    , FLASH_TOP  = 0x4000
  };
  enum tinyrc_nvm_e {
      NVM_BUSY      = 0x80
    , NVM_NOOP      = 0x00
    , CHIP_ERASE    = 0x10
    , SECTION_ERASE = 0x14
    , WORD_WRITE    = 0x1D
  };

  bool read_memory (void);
  bool write_memory (void);

  void nvm_wait (void);
  void nvm_ctrl (const uint8_t nvmcmd);

  bool chip_erase (void);

  bool read_data_memory (uint32_t start_addr, uint8_t *data, size_t byte_count, bool type = false);
  bool write_data_memory (uint32_t start_addr, uint8_t *data, size_t byte_count, bool type = false);
}

// end of code
