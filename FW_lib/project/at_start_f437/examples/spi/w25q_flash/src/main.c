/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
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
#include "at32f435_437_clock.h"
#include "spi_flash.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_SPI_w25q_flash SPI_w25q_flash
  * @{
  */

#define FLASH_TEST_ADDR                  0x1000
#define BUF_SIZE                         0x256

uint8_t buffer_tx[BUF_SIZE];
uint8_t buffer_rx[BUF_SIZE];
volatile error_status transfer_status = ERROR;

void tx_data_fill(void);
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);

/**
  * @brief  transfer data fill.
  * @param  none
  * @retval none
  */
void tx_data_fill(void)
{
  uint32_t data_index = 0;
  for(data_index = 0; data_index < BUF_SIZE; data_index++)
  {
    buffer_tx[data_index] = data_index;
  }
}

/**
  * @brief  buffer compare function.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval the result of compare
  */
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return ERROR;
    }

    pbuffer1++;
    pbuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t index = 0;
  __IO uint32_t flash_id_index = 0;
  system_clock_config();
  at32_board_init();
  tx_data_fill();
  uart_print_init(115200);
  spiflash_init();
  flash_id_index = spiflash_read_id();
  if((flash_id_index != W25Q128)&&(flash_id_index != W25Q80)&&(flash_id_index != W25Q16)&&(flash_id_index != W25Q32)&&(flash_id_index != W25Q64))
  {
    printf("flash id check error!\r\n");
    for(index = 0; index < 50; index++)
    {
      at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      delay_ms(200);
    }
    return 1;
  }
  else
  {
    printf("flash id check success! id: %x\r\n", flash_id_index);
  }

  /* erase sector */
  spiflash_sector_erase(FLASH_TEST_ADDR / SPIF_SECTOR_SIZE);

  /* write data */
  spiflash_write(buffer_tx, FLASH_TEST_ADDR, BUF_SIZE);

  /* read data */
  spiflash_read(buffer_rx, FLASH_TEST_ADDR, BUF_SIZE);

  /* printf read data */
  printf("Read Data: ");
  for(index = 0; index < BUF_SIZE; index++)
  {
    printf("%x ", buffer_rx[index]);
  }

  /* test result:the data check */
  transfer_status = buffer_compare(buffer_rx, buffer_tx, BUF_SIZE);

  /* test result indicate:if success ,led2 lights */
  if(transfer_status == SUCCESS)
  {
    printf("\r\nflash data read write success!\r\n");
    at32_led_on(LED2);
  }
  else
  {
    printf("\r\nflash data read write ERROR!\r\n");
    at32_led_off(LED2);
  }

  while(1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */
