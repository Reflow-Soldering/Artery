/**
  **************************************************************************
  * @file     flash.h
  * @brief    flash header file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "at32f435_437_board.h"

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup EMAC_iap_app_led3_toggle
  * @{
  */

/** @defgroup app_led3_definition
  * @{
  */

#define FLASH_SIZE                       (*((uint32_t*)0x1FFFF7E0))  /* read from at32 flash capacity register(unit:kbyte) */
#define SRAM_SIZE                        224                         /* sram size, unit:kbyte */
#define USER_FLASH_END_ADDRESS           0x080FFFFF

/**
  * @}
  */

/** @defgroup app_led3_exported_functions
  * @{
  */

void flash_2kb_write(uint32_t write_addr, uint8_t *pbuffer);
flag_status flash_upgrade_flag_read(void);
uint32_t flash_if_write(__IO uint32_t* flash_address, uint32_t* data ,uint16_t data_length);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
