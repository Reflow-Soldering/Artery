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

#include "at32f435_437_board.h"
#include "flash.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_FLASH_write_read
  * @{
  */

#if defined (AT32F437xM) || defined (AT32F435xM) || defined (AT32F437xD) || defined (AT32F435xD) 
#define SECTOR_SIZE                      4096   /* this parameter depends on the specific model of the chip */
#else
#define SECTOR_SIZE                      2048   /* this parameter depends on the specific model of the chip */
#endif

uint16_t flash_buf[SECTOR_SIZE / 2];

/**
  * @brief  read data using halfword mode
  * @param  read_addr: the address of reading
  * @param  p_buffer: the buffer of reading data
  * @param  num_read: the number of reading data
  * @retval none
  */
void flash_read(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read)
{
  uint16_t i;
  for(i = 0; i < num_read; i++)
  {
    p_buffer[i] = *(uint16_t*)(read_addr);
    read_addr += 2;
  }
}

/**
  * @brief  write data using halfword mode without checking
  * @param  write_addr: the address of writing
  * @param  p_buffer: the buffer of writing data
  * @param  num_write: the number of writing data
  * @retval result
  */
error_status flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write)
{
  uint16_t i;
  flash_status_type status = FLASH_OPERATE_DONE; 
  for(i = 0; i < num_write; i++)
  {
    status = flash_halfword_program(write_addr, p_buffer[i]);
    if(status != FLASH_OPERATE_DONE)
      return ERROR;
    write_addr += 2;
  }
  return SUCCESS;
}

/**
  * @brief  write data using halfword mode with checking
  * @param  write_addr: the address of writing
  * @param  p_buffer: the buffer of writing data
  * @param  num_write: the number of writing data
  * @retval result
  */
error_status flash_write(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write)
{
  uint32_t offset_addr;
  uint32_t sector_position;
  uint16_t sector_offset;
  uint16_t sector_remain;
  uint16_t i;
  flash_status_type status = FLASH_OPERATE_DONE;
 
  flash_unlock();
  offset_addr = write_addr - FLASH_BASE;
  sector_position = offset_addr / SECTOR_SIZE;
  sector_offset = (offset_addr % SECTOR_SIZE) / 2;
  sector_remain = SECTOR_SIZE / 2 - sector_offset;
  if(num_write <= sector_remain)
    sector_remain = num_write;
  while(1)
  {
    flash_read(sector_position * SECTOR_SIZE + FLASH_BASE, flash_buf, SECTOR_SIZE / 2);
    for(i = 0; i < sector_remain; i++)
    {
      if(flash_buf[sector_offset + i] != 0xFFFF)
        break;
    }
    if(i < sector_remain)
    {
      /* wait for operation to be completed */
      status = flash_operation_wait_for(ERASE_TIMEOUT);
     
      if((status == FLASH_PROGRAM_ERROR) || (status == FLASH_EPP_ERROR))
        flash_flag_clear(FLASH_PRGMERR_FLAG | FLASH_EPPERR_FLAG);
      else if(status == FLASH_OPERATE_TIMEOUT)
        return ERROR;
      status = flash_sector_erase(sector_position * SECTOR_SIZE + FLASH_BASE);
      if(status != FLASH_OPERATE_DONE)
        return ERROR;
      for(i = 0; i < sector_remain; i++)
      {
        flash_buf[i + sector_offset] = p_buffer[i];
      }
      if(flash_write_nocheck(sector_position * SECTOR_SIZE + FLASH_BASE, flash_buf, SECTOR_SIZE / 2) != SUCCESS)
        return ERROR;
    }
    else
    {
      if(flash_write_nocheck(write_addr, p_buffer, sector_remain) != SUCCESS)
        return ERROR;
    }
    if(num_write == sector_remain)
      break;
    else
    {
      sector_position++;
      sector_offset = 0;
      p_buffer += sector_remain;
      write_addr += (sector_remain * 2);
      num_write -= sector_remain;
      if(num_write > (SECTOR_SIZE / 2))
        sector_remain = SECTOR_SIZE / 2;
      else
        sector_remain = num_write;
    }
  }
  flash_lock();
  return SUCCESS;
}



/**
  * @}
  */

/**
  * @}
  */
