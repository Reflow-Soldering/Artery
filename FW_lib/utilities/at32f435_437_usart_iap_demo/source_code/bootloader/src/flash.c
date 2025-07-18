/**
  **************************************************************************
  * @file     flash.c
  * @brief    flash program
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

#include "flash.h"
#include "usart.h"
#include "iap.h"

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup USART_iap_bootloader
  * @{
  */

/**
  * @brief  flash erase/program operation.
  * @note   follow 2kb operation of ont time
  * @param  none
  * @retval none
  */
void flash_2kb_write(uint32_t write_addr, uint8_t *pbuffer)
{
  uint16_t index, write_data;
  static uint32_t erased_addr = 0;
  flash_unlock();
  if(SECTOR_SIZE_INDEX == 0x04) /* 4kb/sector size */
  {
    if(erased_addr != (write_addr & 0xFFFFF000))
    {
      erased_addr = (write_addr & 0xFFFFF000);
      flash_sector_erase(write_addr);
    }
  }
  else /* 2kb/sector size */
  {
    flash_sector_erase(write_addr);
  }
  for(index = 0; index < 2048; index += 2)
  {
    write_data = (pbuffer[index+1] << 8) + pbuffer[index];
    flash_halfword_program(write_addr, write_data);
    write_addr += sizeof(uint16_t);
  }
  flash_lock();
}

/**
  * @brief  check flash upgrade flag.
  * @param  none
  * @retval none
  */
flag_status flash_upgrade_flag_read(void)
{
  if((*(uint32_t*)IAP_UPGRADE_FLAG_ADDR) == IAP_UPGRADE_FLAG)
    return SET;
  else
    return RESET;
}

/**
  * @}
  */

/**
  * @}
  */
