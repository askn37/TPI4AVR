/**
 * @file NVM.cpp
 * @author askn (K.Sato) multix.jp
 * @brief
 * @version 0.1
 * @date 2022-12-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "NVM.h"
#include "JTAG2.h"
#include "TPI.h"
#include "usart.h"
#include "sys.h"
#include "dbg.h"

bool NVM::read_memory (void) {
  uint8_t  mem_type   = JTAG2::packet.body[1];
  uint32_t byte_count = *((uint32_t*)&JTAG2::packet.body[2]);
  uint32_t start_addr = *((uint32_t*)&JTAG2::packet.body[6]);
  #ifdef DEBUG_USE_USART
  DBG::print(" MT=", false); DBG::write_hex(mem_type);
  DBG::print(" BC=", false); DBG::print_dec(byte_count);
  DBG::print(" SA=", false); DBG::print_hex(start_addr);
  #endif
  JTAG2::packet.body[0] = JTAG2::RSP_MEMORY;
  JTAG2::packet.size = byte_count + 1;
  switch (mem_type) {
    case JTAG2::MTYPE_BOOT_FLASH :
    case JTAG2::MTYPE_XMEGA_FLASH : {
      return NVM::read_data_memory(start_addr, &JTAG2::packet.body[1], byte_count, false);
    }
    case JTAG2::MTYPE_EEPROM :
    case JTAG2::MTYPE_FUSE_BITS :
    case JTAG2::MTYPE_LOCK_BITS :
    case JTAG2::MTYPE_SIGN_JTAG :
    case JTAG2::MTYPE_OSCCAL_BYTE :
    case JTAG2::MTYPE_EEPROM_XMEGA :
    case JTAG2::MTYPE_USERSIG :
    case JTAG2::MTYPE_PRODSIG : {
      return NVM::read_data_memory(start_addr, &JTAG2::packet.body[1], byte_count, true);
    }
  }
  return false;
}

bool NVM::write_memory (void) {
  uint8_t  mem_type   = JTAG2::packet.body[1];
  uint32_t byte_count = *((uint32_t*)&JTAG2::packet.body[2]);
  uint32_t start_addr = *((uint32_t*)&JTAG2::packet.body[6]);
  #ifdef DEBUG_USE_USART
  DBG::print(" MT=", false); DBG::write_hex(mem_type);
  DBG::print(" BC=", false); DBG::print_dec(byte_count);
  DBG::print(" SA=", false); DBG::print_hex(start_addr);
  #endif
  switch (mem_type) {
    case JTAG2::MTYPE_BOOT_FLASH :
    case JTAG2::MTYPE_XMEGA_FLASH : {
      return NVM::write_data_memory(start_addr, &JTAG2::packet.body[10], byte_count, false);
    }
    case JTAG2::MTYPE_FUSE_BITS :
    case JTAG2::MTYPE_LOCK_BITS :
    case JTAG2::MTYPE_EEPROM :
    case JTAG2::MTYPE_EEPROM_XMEGA :
    case JTAG2::MTYPE_USERSIG : {
      return NVM::write_data_memory(start_addr, &JTAG2::packet.body[10], byte_count, true);
    }
  }
  return false;
}

void NVM::nvm_wait (void) {
  while (TPI::SIN(TPI::NVMCSR_REG) & NVM::NVM_BUSY);
}

void NVM::nvm_ctrl (const uint8_t nvmcmd) {
  TPI::SOUT(TPI::NVMCMD_REG, nvmcmd);
}

bool NVM::chip_erase (void) {
  #ifdef DEBUG_USE_USART
  DBG::print("(ERA)", false);
  #endif
  if (!TPI::is_control(TPI::ENABLE_NVMPG)) {
    if (!TPI::enter_tpi(true)) return false;
  }
  TPI::SSTPR(NVM::FLASH_TOP);
  NVM::nvm_wait();
  NVM::nvm_ctrl(NVM::CHIP_ERASE);
  TPI::SST(0xFF);
  TPI::SST(0xFF);
  NVM::nvm_wait();
  #ifdef DEBUG_USE_USART
  DBG::print("[ERASED]", false);
  #endif
  return true;
}

bool NVM::read_data_memory (uint32_t start_addr, uint8_t *data, size_t byte_count, bool type) {
  uint8_t *q = data;
  uint16_t addr = (uint16_t)start_addr;
  size_t cnt = 0;
  if (byte_count == 0 || byte_count > 256) return false;
  TPI::SSTPR(addr);
  while (cnt < byte_count) {
    *q++ = TPI::SLD();
    cnt++;
  }
  if (type) {
    #ifdef DEBUG_USE_USART
    if (byte_count <= 8) {
      DBG::write(',');
      DBG::hexlist(data, cnt);
    }
    else {
      DBG::print("[RD]", false);
      DBG::dump(data, cnt);
    }
    #endif
  }
  else {
    #ifdef DEBUG_DUMP_MEMORY
    DBG::print("[RD]", false);
    DBG::dump(data, cnt);
    #endif
  }
  return cnt == byte_count;
}

bool NVM::write_data_memory (uint32_t start_addr, uint8_t *data, size_t byte_count, bool type) {
  uint8_t *q = data;
  uint16_t addr = (uint16_t)start_addr;
  size_t word_count = (byte_count + 1) >> 1;
  size_t cnt = 0;
  if (byte_count == 0 || byte_count > 256) return false;
  if (type) {
    /* erase section */
    TPI::SSTPR(addr);
    NVM::nvm_wait();
    NVM::nvm_ctrl(NVM::SECTION_ERASE);
    TPI::SST(0xFF);
    TPI::SST(0xFF);
    #ifdef DEBUG_USE_USART
    if (byte_count <= 8) {
      DBG::write(',');
      DBG::hexlist(data, byte_count);
    }
    else {
      DBG::print("[WR]", false);
      DBG::dump(data, byte_count);
    }
    #endif
  }
  else {
    #ifdef DEBUG_DUMP_MEMORY
    DBG::print("[WR]", false);
    DBG::dump(data, byte_count);
    #endif
  }
  TPI::SSTPR(addr);
  NVM::nvm_wait();
  while (cnt < word_count) {
    NVM::nvm_ctrl(NVM::WORD_WRITE);
    TPI::SST(*q++);
    TPI::SST(*q++);
    NVM::nvm_wait();
    cnt++;
  }
  return cnt == word_count;
}

// end of code
