/**
  **************************************************************************
  * @file     msc_diskio.c
  * @brief    usb mass storage disk function
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
#include "msc_diskio.h"
#include "msc_bot_scsi.h"

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_USB_device_msc
  * @{
  */
uint32_t sector_size = 2048;
uint32_t msc_flash_size;
uint8_t scsi_inquiry[MSC_SUPPORT_MAX_LUN][SCSI_INQUIRY_DATA_LENGTH] =
{
  /* lun = 0 */
  {
    0x00,         /* peripheral device type (direct-access device) */
    0x80,         /* removable media bit */
    0x00,         /* ansi version, ecma version, iso version */
    0x01,         /* respond data format */
    SCSI_INQUIRY_DATA_LENGTH - 5, /* additional length */
    0x00, 0x00, 0x00, /* reserved */
    'A', 'T', '3', '2', ' ', ' ', ' ', ' ', /* vendor information "AT32" */
    'D', 'i', 's', 'k', '0', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', /* Product identification "Disk" */
    '2', '.', '0', '0'  /* product revision level */
  }
};

/**
  * @brief  get disk inquiry
  * @param  lun: logical units number
  * @retval inquiry string
  */
uint8_t *get_inquiry(uint8_t lun)
{
  if(lun < MSC_SUPPORT_MAX_LUN)
    return (uint8_t *)scsi_inquiry[lun];
  else
    return NULL;
}

/**
  * @brief  disk read
  * @param  lun: logical units number
  * @param  addr: logical address
  * @param  read_buf: pointer to read buffer
  * @param  len: read length
  * @retval status of usb_sts_type
  */
usb_sts_type msc_disk_read(uint8_t lun, uint64_t addr, uint8_t *read_buf, uint32_t len)
{
  uint32_t i = 0;
  uint32_t flash_addr = (uint32_t)addr + USB_FLASH_ADDR_OFFSET;
  switch(lun)
  {
    case INTERNAL_FLASH_LUN:
      for(i = 0; i < len; i ++)
      {
        read_buf[i] = *((uint8_t *)flash_addr);
        flash_addr += 1;
      }
      break;
    case SPI_FLASH_LUN:
      break;
    case SD_LUN:
      break;
    default:
      break;
  }
  return USB_OK;
}

/**
  * @brief  disk write
  * @param  lun: logical units number
  * @param  addr: logical address
  * @param  buf: pointer to write buffer
  * @param  len: write length
  * @retval status of usb_sts_type
  */
usb_sts_type msc_disk_write(uint8_t lun, uint64_t addr, uint8_t *buf, uint32_t len)
{
  uint32_t flash_addr = (uint32_t)addr + USB_FLASH_ADDR_OFFSET;
  uint32_t i = 0, tolen = len;
  uint32_t erase_addr = flash_addr;
  switch(lun)
  {
    case INTERNAL_FLASH_LUN:
      flash_unlock();
      while(tolen >= sector_size)
      {
        flash_sector_erase(erase_addr);
        tolen -= sector_size;
        erase_addr += sector_size;
      }
      for(i = 0; i < len; i ++)
      {
        flash_byte_program(flash_addr+i, buf[i]);
      }
      flash_lock();
      break;
    case SPI_FLASH_LUN:
      break;
    case SD_LUN:
      break;
    default:
      break;;
  }
  return USB_OK;
}

/**
  * @brief  disk capacity
  * @param  lun: logical units number
  * @param  blk_nbr: pointer to number of block
  * @param  blk_size: pointer to block size
  * @retval status of usb_sts_type
  */
usb_sts_type msc_disk_capacity(uint8_t lun, uint32_t *blk_nbr, uint32_t *blk_size)
{
  uint32_t devid = (*((uint32_t *)DEBUG_BASE) & 0x00007000) >> 12;
  msc_flash_size = (*((uint32_t *)0x1FFFF7E0) << 10) - (USB_FLASH_ADDR_OFFSET - FLASH_BASE);
  switch(devid)
  {
    case 2:
      sector_size = SECTOR_SIZE_1K;
      break;
    case 3:
      sector_size = SECTOR_SIZE_2K;
      break;
    case 4:
      sector_size = SECTOR_SIZE_4K;
      break;
    default:
      sector_size = SECTOR_SIZE_2K;
      break;
  }
  switch(lun)
  {
    case INTERNAL_FLASH_LUN:
      *blk_nbr = msc_flash_size / sector_size;
      *blk_size = sector_size;
      break;
    case SPI_FLASH_LUN:
      break;
    case SD_LUN:
      break;
    default:
      break;
  }
  return USB_OK;
}

/**
  * @}
  */

/**
  * @}
  */
