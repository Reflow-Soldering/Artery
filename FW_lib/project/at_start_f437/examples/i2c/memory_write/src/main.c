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
#include "i2c_application.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_I2C_memory_write I2C_memory_write
  * @{
  */

#define I2C_TIMEOUT                      0xFFFFFFF

//#define I2Cx_CLKCTRL                   0xB170FFFF   //10K
//#define I2Cx_CLKCTRL                   0xC0E06969   //50K
#define I2Cx_CLKCTRL                     0x80504C4E   //100K
//#define I2Cx_CLKCTRL                   0x30F03C6B   //200K

#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C2
#define I2Cx_CLK                         CRM_I2C2_PERIPH_CLOCK
#define I2Cx_DMA                         DMA1
#define I2Cx_DMA_CLK                     CRM_DMA1_PERIPH_CLOCK

#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PIN                GPIO_PINS_10
#define I2Cx_SCL_GPIO_PinsSource         GPIO_PINS_SOURCE10
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_MUX                GPIO_MUX_4

#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PIN                GPIO_PINS_11
#define I2Cx_SDA_GPIO_PinsSource         GPIO_PINS_SOURCE11
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_MUX                GPIO_MUX_4

#define I2Cx_DMA_TX_Channel              DMA1_CHANNEL1
#define I2Cx_DMA_TX_DMAMUX_Channel       DMA1MUX_CHANNEL1
#define I2Cx_DMA_TX_DMAREQ               DMAMUX_DMAREQ_ID_I2C2_TX
#define I2Cx_DMA_TX_IRQn                 DMA1_Channel1_IRQn

#define I2Cx_DMA_RX_Channel              DMA1_CHANNEL2
#define I2Cx_DMA_RX_DMAMUX_Channel       DMA1MUX_CHANNEL2
#define I2Cx_DMA_RX_DMAREQ               DMAMUX_DMAREQ_ID_I2C2_RX
#define I2Cx_DMA_RX_IRQn                 DMA1_Channel2_IRQn

#define I2Cx_EVT_IRQn                    I2C2_EVT_IRQn
#define I2Cx_ERR_IRQn                    I2C2_ERR_IRQn

#define BUF_SIZE                         8

uint8_t tx_buf1[BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t tx_buf2[BUF_SIZE] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
uint8_t tx_buf3[BUF_SIZE] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
uint8_t rx_buf1[BUF_SIZE] = {0};
uint8_t rx_buf2[BUF_SIZE] = {0};
uint8_t rx_buf3[BUF_SIZE] = {0};

i2c_handle_type hi2cx;

/**
  * @brief  error handler program
  * @param  i2c_status
  * @retval none
  */
void error_handler(uint32_t error_code)
{
  while(1)
  {
    at32_led_toggle(LED2);
    delay_ms(500);
  }
}

/**
  * @brief  compare whether the valus of buffer 1 and buffer 2 are equal.
  * @param  buffer1: buffer 1 address.
            buffer2: buffer 2 address.
  * @retval 0: equal.
  *         1: unequal.
  */
uint32_t buffer_compare(uint8_t* buffer1, uint8_t* buffer2, uint32_t len)
{
  uint32_t i;

  for(i = 0; i < len; i++)
  {
    if(buffer1[i] != buffer2[i])
    {
      return 1;
    }
  }

  return 0;
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  i2c_status_type i2c_status;

  /* initial system clock */
  system_clock_config();

  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* at board initial */
  at32_board_init();

  hi2cx.i2cx = I2Cx_PORT;

  /* i2c config */
  i2c_config(&hi2cx);

  while(1)
  {
    /* wait for key USER_BUTTON press before starting the communication */
    while(at32_button_press() != USER_BUTTON)
    {
    }

    /* write data to memory device */
    if((i2c_status = i2c_memory_write(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, tx_buf1, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    delay_ms(5);

    /* read data from memory device */
    if((i2c_status = i2c_memory_read(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, rx_buf1, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* write data to memory device */
    if((i2c_status = i2c_memory_write_int(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, tx_buf2, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* wait for the communication to end */
    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    delay_ms(5);

    /* read data from memory device */
    if((i2c_status = i2c_memory_read_int(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, rx_buf2, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* wait for the communication to end */
    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* write data to memory device */
    if((i2c_status = i2c_memory_write_dma(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, tx_buf3, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* wait for the communication to end */
    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    delay_ms(5);

    /* read data from memory device */
    if((i2c_status = i2c_memory_read_dma(&hi2cx, I2C_MEM_ADDR_WIDIH_8, I2Cx_ADDRESS, 0x00, rx_buf3, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    /* wait for the communication to end */
    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
    {
      error_handler(i2c_status);
    }

    if((buffer_compare(tx_buf1, rx_buf1, BUF_SIZE) == 0) &&
       (buffer_compare(tx_buf2, rx_buf2, BUF_SIZE) == 0) &&
       (buffer_compare(tx_buf3, rx_buf3, BUF_SIZE) == 0))
    {
      at32_led_on(LED3);
    }
    else
    {
      error_handler(i2c_status);
    }

  }
}

/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_init_structure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

    /* gpio configuration */
    gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);

    gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);

    /* configure i2c pins: scl */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_NONE;

    gpio_init_structure.gpio_pins           = I2Cx_SCL_GPIO_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

    /* configure i2c pins: sda */
    gpio_init_structure.gpio_pins           = I2Cx_SDA_GPIO_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_init_structure);

    /* configure and enable i2c interrupt */
    nvic_irq_enable(I2Cx_EVT_IRQn, 0, 0);
    nvic_irq_enable(I2Cx_ERR_IRQn, 0, 0);

    /* configure and enable i2c dma channel interrupt */
    nvic_irq_enable(I2Cx_DMA_TX_IRQn, 0, 0);
    nvic_irq_enable(I2Cx_DMA_RX_IRQn, 0, 0);

    /* i2c dma tx and rx channels configuration */
    /* enable the dma clock */
    crm_periph_clock_enable(I2Cx_DMA_CLK, TRUE);

    /* i2c dma channel configuration */
    hi2c->dma_tx_channel = I2Cx_DMA_TX_Channel;
    hi2c->dma_rx_channel = I2Cx_DMA_RX_Channel;

    dma_reset(hi2c->dma_tx_channel);
    dma_reset(hi2c->dma_rx_channel);

    hi2c->dma_init_struct.peripheral_base_addr    = (uint32_t)&hi2c->i2cx->txdt;
    hi2c->dma_init_struct.memory_base_addr        = 0;
    hi2c->dma_init_struct.direction               = DMA_DIR_MEMORY_TO_PERIPHERAL;
    hi2c->dma_init_struct.buffer_size             = 0xFFFF;
    hi2c->dma_init_struct.peripheral_inc_enable   = FALSE;
    hi2c->dma_init_struct.memory_inc_enable       = TRUE;
    hi2c->dma_init_struct.peripheral_data_width   = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.memory_data_width       = DMA_MEMORY_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.loop_mode_enable        = FALSE;
    hi2c->dma_init_struct.priority                = DMA_PRIORITY_LOW;

    dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
    dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

    dmamux_init(I2Cx_DMA_TX_DMAMUX_Channel, I2Cx_DMA_TX_DMAREQ);
    dmamux_init(I2Cx_DMA_RX_DMAMUX_Channel, I2Cx_DMA_RX_DMAREQ);

    dmamux_enable(I2Cx_DMA, TRUE);

    /* config i2c */
    i2c_init(hi2c->i2cx, 0x0F, I2Cx_CLKCTRL);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}

/**
  * @}
  */

/**
  * @}
  */
