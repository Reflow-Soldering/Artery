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

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_I2S_fullduplex_dma I2S_fullduplex_dma
  * @{
  */

#define TXBUF_SIZE                       32
#define RXBUF_SIZE                       TXBUF_SIZE

uint16_t i2s2_buffer_tx[TXBUF_SIZE];
uint16_t i2s2_buffer_rx[RXBUF_SIZE];
uint16_t i2s3_buffer_tx[TXBUF_SIZE];
uint16_t i2s3_buffer_rx[RXBUF_SIZE];
volatile error_status transfer_status1 = ERROR, transfer_status2 = ERROR;

/**
  * @brief  buffer compare function.
  * @param  none
  * @retval the result of compare
  */
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length)
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
  * @brief  transfer data fill.
  * @param  none
  * @retval none
  */
void tx_data_fill(void)
{
  uint32_t data_index = 0;
  for(data_index = 0; data_index < TXBUF_SIZE; data_index++)
  {
    i2s2_buffer_tx[data_index] = data_index;
    i2s3_buffer_tx[data_index] = TXBUF_SIZE - data_index;
  }
}

/**
  * @brief  dma configuration.
  * @param  none
  * @retval none
  */
static void dma_config(void)
{
  dma_init_type dma_init_struct;

  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  dmamux_enable(DMA1, TRUE);
  
  /* use dma1_channel1 as spi2 transmit channel */
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)i2s2_buffer_tx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI2->dt);
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL1, DMAMUX_DMAREQ_ID_SPI2_TX);
  
  /* use dma1_channel2 as spi2 receive channel */
  dma_reset(DMA1_CHANNEL2);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)i2s2_buffer_rx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(I2S2EXT->dt);
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init(DMA1_CHANNEL2, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL2, DMAMUX_DMAREQ_ID_I2S2_EXT_RX);
  
  /* use dma1_channel3 as spi3 receive channel */
  dma_reset(DMA1_CHANNEL3);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)i2s3_buffer_rx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(SPI3->dt);
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init(DMA1_CHANNEL3, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL3, DMAMUX_DMAREQ_ID_SPI3_RX);
  
  /* use dma1_channel4 as spi3 transmit channel */
  dma_reset(DMA1_CHANNEL4);
  dma_init_struct.buffer_size = 32;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = FALSE;
  dma_init_struct.memory_base_addr = (uint32_t)i2s3_buffer_tx;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(I2S3EXT->dt);
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init(DMA1_CHANNEL4, &dma_init_struct);
  dmamux_init(DMA1MUX_CHANNEL4, DMAMUX_DMAREQ_ID_I2S3_EXT_TX);
}

/**
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(void)
{
  i2s_init_type i2s_init_struct;
  
  /* master i2s initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  i2s_default_para_init(&i2s_init_struct);
  
  /* i2s2 as master transmission */
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(SPI2, &i2s_init_struct);
  
  /* i2s2ext as slave reception */
  i2s_init_struct.operation_mode =I2S_MODE_SLAVE_RX;
  i2s_init(I2S2EXT, &i2s_init_struct);
  
  /* use dma transmit and receive */
  spi_i2s_dma_transmitter_enable(SPI2, TRUE);
  spi_i2s_dma_receiver_enable(I2S2EXT, TRUE);
  
  i2s_enable(SPI2, TRUE);
  i2s_enable(I2S2EXT, TRUE);
  
  /* slave i2s initialization */
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  
  /* i2s3 as slave reception */
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_RX;
  i2s_init(SPI3, &i2s_init_struct);
  
  /* i2s3ext as slave transmission */
  i2s_init_struct.operation_mode =I2S_MODE_SLAVE_TX;
  i2s_init(I2S3EXT, &i2s_init_struct);
  
  /* use dma transmit and receive */
  spi_i2s_dma_receiver_enable(SPI3, TRUE);
  spi_i2s_dma_transmitter_enable(I2S3EXT, TRUE);
  
  i2s_enable(SPI3, TRUE);
  i2s_enable(I2S3EXT, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);

  /* master i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE0, GPIO_MUX_7);

  /* master i2s ck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins = GPIO_PINS_1;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE1, GPIO_MUX_6);

  /* master i2s ext_sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_2;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE2, GPIO_MUX_6);

  /* master i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE4, GPIO_MUX_6);

  /* slave i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOA, &gpio_initstructure);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_6);

  /* slave i2s ck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE10, GPIO_MUX_6);

  /* slave i2s ext_sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_11;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE11, GPIO_MUX_5);

  /* slave i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE12, GPIO_MUX_6);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
  at32_board_init();
  at32_led_on(LED4);
  tx_data_fill();
  gpio_config();
  dma_config();
  i2s_config();
  
  /* enable i2s slave dma to get and fill data */
  dma_channel_enable(DMA1_CHANNEL3, TRUE);
  dma_channel_enable(DMA1_CHANNEL4, TRUE);
  
  /* enable i2s master dma to get and fill data */
  dma_channel_enable(DMA1_CHANNEL2, TRUE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
  
  /* wait master and slave spi data receive end */
  while(dma_flag_get(DMA1_FDT2_FLAG) == RESET)
  {
  }
  while(dma_flag_get(DMA1_FDT3_FLAG) == RESET)
  {
  }
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) != RESET);

  /* test result:the data check */
  transfer_status1 = buffer_compare(i2s2_buffer_rx, i2s3_buffer_tx, TXBUF_SIZE);
  transfer_status2 = buffer_compare(i2s3_buffer_rx, i2s2_buffer_tx, TXBUF_SIZE);

  /* test result indicate:if success ,led2 lights */
  if((transfer_status1 == SUCCESS) &&(transfer_status2 == SUCCESS))
  {
    at32_led_on(LED2);
  }
  else
  {
    at32_led_on(LED3);
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
