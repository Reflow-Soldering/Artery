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

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_DMA_flash_to_sram DMA_flash_to_sram
  * @{
  */

#define BUFFER_SIZE                      32

const uint32_t src_const_buffer[BUFFER_SIZE]= {0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
                                               0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
                                               0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
                                               0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
                                               0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
                                               0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
                                               0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
                                               0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80};
uint32_t dst_buffer[BUFFER_SIZE];
gpio_init_type  gpio_init_struct = {0};
dma_init_type dma_init_struct;
__IO uint32_t data_counter_end = 0x01; /* this variable should not be initialized to 0 */
error_status transfer_status = ERROR;

/**
  * @brief  compares two buffers.
  * @param  pbuffer, pbuffer1: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval SUCCESS: pbuffer identical to pbuffer1
  *         ERROR: pbuffer differs from pbuffer1
  */
error_status buffer_compare(const uint32_t* pbuffer, uint32_t* pbuffer1, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer != *pbuffer1)
    {
      return ERROR;
    }

    pbuffer++;
    pbuffer1++;
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
  /* initial system clock */
  system_clock_config();

  /* at board initial */
  at32_board_init();

  /* enable dma1 clock */
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

  /* dma1 channel1 configuration */
  dma_reset(DMA1_CHANNEL1);
  dma_init_struct.buffer_size = BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)dst_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_WORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)src_const_buffer;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_WORD;
  dma_init_struct.peripheral_inc_enable = TRUE;
  dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  /* enable transfer full data interrupt */
  dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);

  /* dma1 channel1 interrupt nvic init */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  nvic_irq_enable(DMA1_Channel1_IRQn, 1, 0);

  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* wait the end of transmission */
  while(data_counter_end != 0)
  {
  }

  /* check if the transmitted and received data are equal */
  transfer_status = buffer_compare(src_const_buffer, dst_buffer, BUFFER_SIZE);

  /* transfer_status = success, if the transmitted and received data are the same
     transfer_status = error, if the transmitted and received data are different */
  if(transfer_status == SUCCESS)
  {
    /* turn led2/led3/led4 on */
    at32_led_on(LED2);
    at32_led_on(LED3);
    at32_led_on(LED4);
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
