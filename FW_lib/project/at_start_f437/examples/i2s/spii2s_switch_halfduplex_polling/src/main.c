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

/** @addtogroup 437_I2S_spii2s_switch_halfduplex_polling I2S_spii2s_switch_halfduplex_polling
  * @{
  */

#define SPI_MASTER_CS_HIGH   gpio_bits_set(GPIOA, GPIO_PINS_4)
#define SPI_MASTER_CS_LOW    gpio_bits_reset(GPIOA, GPIO_PINS_4)

uint16_t i2s3_buffer_tx[32] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                               0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                               0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                               0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                               0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                               0x3D3E, 0x3F40
                              };
uint16_t spi3_buffer_tx[32] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
                               0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
                               0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
                               0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                               0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
                               0x8D8E, 0x8F90
                              };
uint16_t i2s2_buffer_rx[32];
uint16_t spi2_buffer_rx[32];
__IO uint32_t tx_index = 0, rx_index = 0;
volatile error_status transfer_status1 = ERROR, transfer_status2 = ERROR, transfer_status3 = ERROR;

/**
  * @brief  buffer_compare function.
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
  * @brief  i2s configuration.
  * @param  none
  * @retval none
  */
static void i2s_config(i2s_operation_mode_type i2s3_mode, i2s_operation_mode_type i2s2_mode)
{
  i2s_init_type i2s_init_struct;
  
  /* i2s3 initialization */
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  spi_i2s_reset(SPI3);
  i2s_default_para_init(&i2s_init_struct);
  
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = FALSE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = i2s3_mode;
  i2s_init(SPI3, &i2s_init_struct);
  
  /* i2s2 initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  spi_i2s_reset(SPI2);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_32BIT;
  i2s_init_struct.mclk_output_enable = FALSE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_48K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode =i2s2_mode;
  i2s_init(SPI2, &i2s_init_struct);
}

/**
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spi_config(void)
{
  spi_init_type spi_init_struct;
  
  /* master spi initialization */
  crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  spi_i2s_reset(SPI3);
  spi_default_para_init(&spi_init_struct);
  
  /* single line bidirectional half duplex mode-transmitting */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI3, &spi_init_struct);
  
  spi_enable(SPI3, TRUE);
  
  /* slave spi initialization */
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  spi_i2s_reset(SPI2);
  
  /* dual line unidirectional simplex receive-only mode */
  spi_init_struct.transmission_mode = SPI_TRANSMIT_SIMPLEX_RX;
  spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_8;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_HARDWARE_MODE;
  spi_init(SPI2, &spi_init_struct);

  spi_enable(SPI2, TRUE);
}

/**
  * @brief  gpio configuration.
  * @param  spi_i2s_mode:select spi or i2s to be use
  *         - 0 (spi mode)
  *         - 1 (i2s mode)
  * @retval none
  */
static void gpio_config(uint16_t spi_i2s_mode)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);

  /* master i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  if(spi_i2s_mode == 0)
  {
    gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  }
  else
  {
    gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  }
  gpio_init(GPIOA, &gpio_initstructure);
  if(spi_i2s_mode == 0)
  {
    /* non communication time: master pull up CS pin release slave */
    SPI_MASTER_CS_HIGH;
  }
  else
  {
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_6);
  }

  /* master i2s ck pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_10;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE10, GPIO_MUX_6);

  /* master i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_12;
  gpio_init(GPIOC, &gpio_initstructure);
  gpio_pin_mux_config(GPIOC, GPIO_PINS_SOURCE12, GPIO_MUX_6);

  /* slave i2s ws pin */
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins = GPIO_PINS_0;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE0, GPIO_MUX_7);

  /* slave i2s ck pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_pins = GPIO_PINS_1;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE1, GPIO_MUX_6);

  /* slave i2s sd pin */
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_pins = GPIO_PINS_4;
  gpio_init(GPIOD, &gpio_initstructure);
  gpio_pin_mux_config(GPIOD, GPIO_PINS_SOURCE4, GPIO_MUX_6);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t index = 0;
  system_clock_config();
  at32_board_init();
  at32_led_on(LED4);
  
  /* first: i2s communication */
  gpio_config(1);
  i2s_config(I2S_MODE_MASTER_TX, I2S_MODE_SLAVE_RX);
  i2s_enable(SPI2, TRUE);
  i2s_enable(SPI3, TRUE);
  while(rx_index < 32)
  {
    /* i2s transmit data fill */
    while(spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI3, i2s3_buffer_tx[tx_index++]);
    
    /* i2s receive data get */
    while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
    i2s2_buffer_rx[rx_index++] = spi_i2s_data_receive(SPI2);
  }
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);

  /* test result:the data check */
  transfer_status1 = buffer_compare(i2s2_buffer_rx, i2s3_buffer_tx, 32);

  /* receive buffer clear */
  tx_index = 0;
  rx_index = 0;
  for(index = 0; index < 32; index++)
  {
    i2s2_buffer_rx[index] = 0;
  }
  
  /* second: spi communication */
  gpio_config(0);
  spi_config();
  
  /* start communication: master pull down CS pin select slave */
  SPI_MASTER_CS_LOW;
  
  while(rx_index < 32)
  {
    /* spi transmit data fill */
    while(spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI3, spi3_buffer_tx[tx_index++]);
    
    /* spi receive data get */
    while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
    spi2_buffer_rx[rx_index++] = spi_i2s_data_receive(SPI2);
  }
  
  /* wait master and slave idle when communication end */
  while(spi_i2s_flag_get(SPI3, SPI_I2S_BF_FLAG) != RESET);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
  
  /* end communication: master pull up CS pin release slave */
  SPI_MASTER_CS_HIGH;

  /* test result:the data check */
  transfer_status2 = buffer_compare(spi2_buffer_rx, spi3_buffer_tx, 32);

  /* receive buffer clear */
  tx_index = 0;
  rx_index = 0;
  for(index = 0; index < 32; index++)
  {
    i2s2_buffer_rx[index] = 0;
  }
  
  /* third: i2s communication */
  gpio_config(1);
  i2s_config(I2S_MODE_SLAVE_TX, I2S_MODE_MASTER_RX);
  
  /* when slave transmission mode,the slave trans data need fill in advance */
  while(spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET);
  spi_i2s_data_transmit(SPI3, i2s3_buffer_tx[tx_index++]);
  
  i2s_enable(SPI3, TRUE);
  i2s_enable(SPI2, TRUE);
  while(rx_index < 32)
  {
    /* i2s receive data get */
    while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
    i2s2_buffer_rx[rx_index++] = spi_i2s_data_receive(SPI2);
    
    /* i2s transmit data fill */
    while(spi_i2s_flag_get(SPI3, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI3, i2s3_buffer_tx[tx_index++]);
  }
  
  /* master half duplex receiving mode,the busy flag isnot usefull */

  /* test result:the data check */
  transfer_status3 = buffer_compare(i2s2_buffer_rx, i2s3_buffer_tx, 32);

  /* test result indicate:if passed ,led2 lights */
  if((transfer_status1 == SUCCESS) && (transfer_status2 == SUCCESS) && (transfer_status3 == SUCCESS))
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
