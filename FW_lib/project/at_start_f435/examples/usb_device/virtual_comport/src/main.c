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
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"
#include "string.h"

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_USB_device_virtaul_comport USB_device_virtaul_comport
  * @{
  */

/* usb global struct define */
otg_core_type otg_core_struct;
uint32_t usb_buffer[256];
uint8_t usart2_tx_counter = 0;
uint16_t data_len;
void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);

/* usart global struct define */
extern linecoding_type linecoding;
void usb_usart_config(linecoding_type linecoding);
void usart_gpio_config(void);
#define  usart_buffer_size  2048
uint8_t usart_rx_buffer[usart_buffer_size];
uint32_t usb_tx_buffer[512];
uint16_t hw_usart_rx_index = 0;
uint16_t hw_usart_read_index = 0;
uint16_t usart_rx_data_len = 0;
uint16_t ov_cnt = 0;
void usart_send_data(uint8_t *send_data, uint16_t len);
uint16_t usart_receive_data(void);
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  uint32_t timeout;

  uint8_t send_zero_packet = 0;

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  system_clock_config();

  at32_board_init();

  /* usart gpio config */
  usart_gpio_config();

  /* hardware usart config: usart2 */
  usb_usart_config(linecoding);

  /* usb gpio config */
  usb_gpio_config();

#ifdef USB_LOW_POWER_WAKUP
  usb_low_power_wakeup_config();
#endif

  /* enable otgfs clock */
  crm_periph_clock_enable(OTG_CLOCK, TRUE);

  /* select usb 48m clcok source */
  usb_clock48m_select(USB_CLK_HEXT);

  /* enable otgfs irq */
  nvic_irq_enable(OTG_IRQ, 0, 0);

  /* init usb */
  usbd_init(&otg_core_struct,
            USB_FULL_SPEED_CORE_ID,
            USB_ID,
            &cdc_class_handler,
            &cdc_desc_handler);
  while(1)
  {
    /* get usb vcp receive data */
    data_len = usb_vcp_get_rxdata(&otg_core_struct.dev, (uint8_t *)usb_buffer);

    /* send data to hardware usart */
    if(data_len > 0)
    {
      usart_send_data((uint8_t *)usb_buffer, data_len);
    }

    /* if hardware usart received data,usb send data to host */
    usart_rx_data_len = usart_receive_data();
    if(usart_rx_data_len || send_zero_packet == 1)
    {
      if(usart_rx_data_len > 0)
        send_zero_packet = 1;

      if(usart_rx_data_len == 0)
        send_zero_packet = 0;

      timeout = 50000;

      if((hw_usart_read_index + usart_rx_data_len) < usart_buffer_size)
      {
        do
        {
          /* send data to host */
          memcpy((uint8_t *)usb_tx_buffer, &usart_rx_buffer[hw_usart_read_index], usart_rx_data_len);
          if(usb_vcp_send_data(&otg_core_struct.dev, (uint8_t *)usb_tx_buffer, usart_rx_data_len) == SUCCESS)
          {
            hw_usart_read_index = hw_usart_read_index + usart_rx_data_len;

            break;
          }
        }while(timeout --);

      }
      /* process the fifo overflow */
      else
      {
        do
        {
          /* send data to host */
          memcpy((uint8_t *)usb_tx_buffer, &usart_rx_buffer[hw_usart_read_index], usart_buffer_size - hw_usart_read_index);
          if(usb_vcp_send_data(&otg_core_struct.dev, (uint8_t *)usb_tx_buffer, usart_buffer_size - hw_usart_read_index) == SUCCESS)
          {
            /* get fifo overflow data count */
            ov_cnt = usart_rx_data_len - (usart_buffer_size - hw_usart_read_index);
            hw_usart_read_index = 0;
            break;
          }
        }while(timeout --);
        timeout = 50000;
        do
        {
          /* send data to host */
          memcpy((uint8_t *)usb_tx_buffer, &usart_rx_buffer[hw_usart_read_index], ov_cnt);
          if(usb_vcp_send_data(&otg_core_struct.dev, (uint8_t *)usb_tx_buffer, ov_cnt) == SUCCESS)
          {
            hw_usart_read_index = ov_cnt;
            break;
          }
        }while(timeout --);
      }
    }
  }
}

/**
  * @brief  this function handles usart2 send data.
  * @param  send_data: pointer to data
  * @param  len: data len
  * @retval none
  */
void usart_send_data(uint8_t *send_data, uint16_t len)
{
  uint16_t index = 0;
  for(index = 0; index < len; index++ )
  {
    do
    {
      ;
    }while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);

    usart_data_transmit(USART2, send_data[index]);
  }
}

/**
  * @brief  this function handles usart2 receive data.
  * @param  none
  * @retval the data len
  */
uint16_t usart_receive_data(void)
{
  uint16_t usart_data_len;
  if(hw_usart_read_index == hw_usart_rx_index)
  {
    usart_data_len = 0;
  }
  else
  {
    /* whether to process the fifo overflow or not */
    if(hw_usart_rx_index > hw_usart_read_index)
      usart_data_len = hw_usart_rx_index - hw_usart_read_index;
    else if(hw_usart_rx_index == 0 && hw_usart_rx_index != hw_usart_read_index)
      usart_data_len = usart_buffer_size - hw_usart_read_index;
    else
      usart_data_len = (usart_buffer_size-1) + hw_usart_rx_index - hw_usart_read_index;
  }

  return usart_data_len;
}

/**
  * @brief  this function handles usart2 handler.
  * @param  none
  * @retval none
  */
void USART2_IRQHandler(void)
{
  if(usart_interrupt_flag_get(USART2, USART_RDBF_FLAG) != RESET)
  {
    /* read one byte from the receive data register */
    usart_rx_buffer[hw_usart_rx_index] = usart_data_receive(USART2);
    hw_usart_rx_index = ( (hw_usart_rx_index+1) & (usart_buffer_size-1) );
  }
}

/**
  * @brief  this function handles usart2  and linecoding config.
  * @param  linecoding: linecoding value
  * @retval none
  */
void usb_usart_config( linecoding_type linecoding)
{
  usart_stop_bit_num_type usart_stop_bit;
  usart_data_bit_num_type usart_data_bit;
  usart_parity_selection_type usart_parity_select;

  /* enable the usart2 and gpio clock */
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, FALSE);
  crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);

  /* stop bit */
  switch(linecoding.format)
  {
    case 0x0:
      usart_stop_bit = USART_STOP_1_BIT;
      break;
  /* to be used when transmitting and receiving data in smartcard mode */
    case 0x1:
      usart_stop_bit = USART_STOP_1_5_BIT;
      break;
    case 0x2:
      usart_stop_bit = USART_STOP_2_BIT;
      break;
    default :
      break;
  }

    /* parity */
  switch(linecoding.parity)
  {
    case 0x0:
      usart_parity_select = USART_PARITY_NONE;
      break;
    case 0x1:
      usart_parity_select = USART_PARITY_ODD;
      break;
    case 0x2:
      usart_parity_select = USART_PARITY_EVEN;
      break;
    /* hardware usart not support parity for mark and space  */
    case 0x3:
    case 0x4:
      break;
    default :
      break;
  }

  if(USART_PARITY_NONE == usart_parity_select)
  {
    /* data bits */
    switch(linecoding.data)
    {
      /* hardware usart not support data bits for 5/6 */
      case 0x5:
      case 0x6:
        break;
      case 0x7:
        usart_data_bit = USART_DATA_7BITS;
        break;
      case 0x8:
        usart_data_bit = USART_DATA_8BITS;
        break;
      /* hardware usart not support data bits for 16 */
      case 0x10:
        break;
      default :
        break;
    }
  }
  else
  {
    /* data bits */
    switch(linecoding.data)
    {
      /* hardware usart not support data bits for 5/6 */
      case 0x5:
      case 0x6:
        break;
      case 0x7:
        usart_data_bit = USART_DATA_8BITS;
        break;
      case 0x8:
        usart_data_bit = USART_DATA_9BITS;
        break;
      /* hardware usart not support data bits for 16 */
      case 0x10:
        break;
      default :
        break;
    }
  }

  nvic_irq_enable(USART2_IRQn, 0, 0);

  /* configure usart2 param */
  usart_init(USART2, linecoding.bitrate, usart_data_bit, usart_stop_bit);
  usart_parity_selection_config(USART2, usart_parity_select);
  usart_transmitter_enable(USART2, TRUE);
  usart_receiver_enable(USART2, TRUE);

  /* enable usart2 interrupt */
  usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE);
  usart_enable(USART2, TRUE);
}

/**
  * @brief  this function handles usart2 gpio config.
  * @param  none
  * @retval none
  */
void usart_gpio_config(void)
{
  gpio_init_type gpio_init_struct;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  /* configure the usart2 tx, rx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_2 | GPIO_PINS_3;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOA, &gpio_init_struct);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_7);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_7);
}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);
#if (USB_ID == 0)
    acc_sof_select(ACC_SOF_OTG1);
#else
    acc_sof_select(ACC_SOF_OTG2);
#endif
    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      /* 216MHz */
      case 216000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4_5);
        break;

      /* 240MHz */
      case 240000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5);
        break;

      /* 264MHz */
      case 264000000:
        crm_usb_clock_div_set(CRM_USB_DIV_5_5);
        break;

      /* 288MHz */
      case 288000000:
        crm_usb_clock_div_set(CRM_USB_DIV_6);
        break;

      default:
        break;

    }
  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  /* dp and dm */
  gpio_init_struct.gpio_pins = OTG_PIN_DP | OTG_PIN_DM;
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);

  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DP_SOURCE, OTG_PIN_MUX);
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_DM_SOURCE, OTG_PIN_MUX);

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_SOF_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif


}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

#endif

/**
  * @brief  this function handles otgfs interrupt.
  * @param  none
  * @retval none
  */
void OTG_IRQ_HANDLER(void)
{
  usbd_irq_handler(&otg_core_struct);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  delay_us(us);
}

/**
  * @}
  */

/**
  * @}
  */
