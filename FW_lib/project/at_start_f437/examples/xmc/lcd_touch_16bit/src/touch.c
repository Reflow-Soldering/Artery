/**
  **************************************************************************
  * @file     touch.c
  * @brief    this file contains all the functions prototypes for the
  *           touch firmware driver.
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
#include "touch.h"
#include "xmc_lcd.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_XMC_lcd_touch_16bit
  * @{
  */

touch_dev_type touch_dev_struct =
{
  touch_pin_init,  /*!< function for init touch pin */
  touch_read_xy,   /*!< function for read data from touch */
  touch_scan,      /*!< function for scanning the screen */
  touch_adjust,    /*!< function for adjust the screen */
  0,
  0,
};

uint8_t cmd_rdx = 0xd0;
uint8_t cmd_rdy = 0x90;

touch_dev_type *touch_struct;

/**
  * @brief  this function is init touch pin.
  * @param  none
  * @retval none
  */
void touch_pin_init(void)
{
  gpio_init_type  gpio_init_struct = {0};
  spi_init_type   spi_init_struct;

  /* enable the gpio clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_SPI4_PERIPH_CLOCK, TRUE);

  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE13, GPIO_MUX_6);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_6);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE1, GPIO_MUX_5);

  gpio_init_struct.gpio_pins = GPIO_PINS_13 |  GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_1;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_12;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_11;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_bits_set(GPIOB, GPIO_PINS_11);

  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_256;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI4, &spi_init_struct);
  spi_enable(SPI4, TRUE);
}

/**
  * @brief  this function is read data from touch.
  * @param  x/y : coordinate vaule.
  * @retval none
  */
void touch_read_xy(uint16_t *x, uint16_t *y)
{
  uint16_t xtemp, ytemp;
  xtemp = touch_read_x_or_y(cmd_rdx);
  ytemp = touch_read_x_or_y(cmd_rdy);

  *x = xtemp;
  *y = ytemp;
}

/**
  * @brief  this function is processing data that read from touch.
  * @param  xy: data.
  * @retval processed data
  */
/* read five times, remove max and min, and then take the average */
#define READ_TIMES                       5
#define LOST_VAL                         1
uint16_t touch_read_x_or_y(uint8_t xy)
{
  uint16_t i, j;
  uint16_t buf[READ_TIMES];
  uint16_t sum = 0;
  uint16_t temp;

  for(i = 0; i < READ_TIMES; i++)
  {
    buf[i] = touch_read_ad(xy);
  }

  for(i = 0; i < READ_TIMES - 1; i++)
  {
    for(j = i + 1; j < READ_TIMES; j++)
    {
      /* increase the sorting */
      if(buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  sum = 0;

  for(i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
  {
    sum += buf[i];
  }

  temp = sum/(READ_TIMES - 2 * LOST_VAL);

  return temp;
}

/**
  * @brief  this function is read data from touch.
  * @param  xy : command.
  * @retval data
  */
uint16_t touch_read_ad(uint8_t xy)
{
  uint16_t num = 0;

  /* enable touch ic */
  TCS_LOW;

  /* send command */
  touch_write_1byte(xy);

  /* touch ic conversion timer */
  delay_us(6);
  num = touch_read_1byte();
  num = num << 8;
  num |= touch_read_1byte();
  num >>= 3;

  /* disable touch ic */
  TCS_HIGH;
  return(num);
}

/**
  * @brief  this function is write command to touch.
  * @param  cmd : command.
  * @retval none
  */
void touch_write_1byte(uint8_t cmd)
{
  while(spi_i2s_flag_get(SPI4, SPI_I2S_TDBE_FLAG) == RESET);
  spi_i2s_data_transmit(SPI4, cmd);

  while(spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET);
  SPI4->dt;
}

/**
  * @brief  this function is read data from touch.
  * @param  none
  * @retval data
  */
uint8_t touch_read_1byte(void)
{
  uint8_t touch;

  while(spi_i2s_flag_get(SPI4, SPI_I2S_TDBE_FLAG) == RESET);
  spi_i2s_data_transmit(SPI4, 0XFF);

  while(spi_i2s_flag_get(SPI4, SPI_I2S_RDBF_FLAG) == RESET);
  touch = spi_i2s_data_receive(SPI4);

  return touch;
}

/**
  * @brief  this function is scanning the screen.
  * @param  none
  * @retval none
  */
void touch_scan(void)
{
  uint16_t x, y;

  if(gpio_input_data_bit_read(GPIOB, GPIO_PINS_11) == 0)
  {
    touch_dev_struct.touch_read_xy(&touch_dev_struct.x_p[1], &touch_dev_struct.y_p[1]);
    x = (240 * touch_dev_struct.x_p[1]) / (0xed0);
    y = (320 * touch_dev_struct.y_p[1]) / (0xe60);

    lcd_drawpoint(x, y, RED);
    lcd_drawpoint(x - 1, y, RED);
    lcd_drawpoint(x + 1, y, RED);
    lcd_drawpoint(x, y + 1, RED);
    lcd_drawpoint(x, y - 1, RED);
  }
}

/**
  * @brief  this function is adjust the screen.
  * @param  none
  * @retval state
  */
error_status touch_adjust(void)
{
  /* touch adjust code */
  return SUCCESS;
}

/**
  * @}
  */

/**
  * @}
  */
