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

#include "xmc_nand.h"
#include "at32f435_437_board.h"
#include "at32f435_437_clock.h"

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_XMC_nand XMC_nand
  * @{
  */

crm_clocks_freq_type crm_clocks_freq_struct = {0};
nand_id_type nand_id_struct = {0};
nand_address_type write_read_addr_struct;
uint8_t txbuffer[BUFFER_SIZE], rxbuffer[BUFFER_SIZE + NAND_SPARE_AREA_SIZE];
uint32_t pagenumber = BUFFER_SIZE / NAND_PAGE_SIZE, writereadstatus = 0, status= 0;
uint32_t j = 0;

void fill_buffer(uint8_t *pbuffer, uint16_t bufferlenght, uint32_t offset);

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* configure the system clock */
  system_clock_config();

  /* led initalization */
  at32_board_init();

  /* usart1 initalization */
  uart_print_init(115200);

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* xmc initialization */
  nand_init();


  /* nand reset command */
  nand_reset();
  delay_us(10);

  /* nand read id command */
  nand_read_id(&nand_id_struct);

  /* verify the nand id */
  /* nand support:samsung:k9gag08u0e        hynix:h27u1g8f2ctr
                  id     :0xecd58472        id   :0xadf1801d
  */
  if((nand_id_struct.maker_id == NAND_AT_MakerID) && (nand_id_struct.device_id == NAND_AT_DeviceID))
  {
    /* nand memory address to write to */
    write_read_addr_struct.zone  = 0x00;
    write_read_addr_struct.block = 0x00;
    write_read_addr_struct.page  = 0x00;
    write_read_addr_struct.byte  = 0x00;

    /* erase the nand first block */
    status = nand_erase_block(write_read_addr_struct);

    /* fill the buffer to send */
    fill_buffer(txbuffer, BUFFER_SIZE , 0x66);

    /* write data to xmc nand memory */
    status = nand_write_small_page(txbuffer, write_read_addr_struct, pagenumber);

    /* read back the written data */
    status = nand_read_small_page (rxbuffer, write_read_addr_struct, pagenumber);

    /* verify the written data */
    for(j = 0; j < BUFFER_SIZE; j++)
    {
      if(txbuffer[j] != rxbuffer[j])
      {
        writereadstatus++;
      }
    }

    if(writereadstatus == 0)
    {
      /* pass : printf "data is right" */
      printf("data is right\r\n");
    }
    else
    {
      /* fail: printf "data is error" */
      printf("data is error\r\n");
    }
  }

  /* printf to indicate that whether there is an device error (id not correct) */
  else
  {
    printf("the id is error\r\n");
  }

  while(1)
  {
  }
}

/**
  * @brief  fill the global buffer
  * @param  pbuffer: pointer on the buffer to fill
  * @param  buffersize: size of the buffer to fill
  * @param  offset: first value to fill on the buffer
  */
void fill_buffer(uint8_t *pbuffer, uint16_t bufferlenght, uint32_t offset)
{
  uint16_t indextmp = 0;

  /* put in global buffer same values */
  for(indextmp = 0; indextmp < bufferlenght; indextmp++ )
  {
    pbuffer[indextmp] = indextmp + offset;
  }
}


/**
  * @}
  */

/**
  * @}
  */


