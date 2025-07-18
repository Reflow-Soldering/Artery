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

/** @addtogroup 437_CAN_filter CAN_filter
  * @{
  */

/* extended identifier */
#define FILTER_EXT_ID1                   ((uint32_t)0x18F5F100)
#define FILTER_EXT_ID2                   ((uint32_t)0x18F5F200)
#define FILTER_EXT_ID3                   ((uint32_t)0x18F5F300)

/* standard identifier */
#define FILTER_STD_ID1                   ((uint16_t)0x04F6)
#define FILTER_STD_ID2                   ((uint16_t)0x04F7)
#define FILTER_STD_ID3                   ((uint16_t)0x04F8)

uint8_t test_result = 0;
/**
  *  @brief  can gpio config
  *  @param  none
  *  @retval none
  */
static void can_gpio_config(void)
{
  gpio_init_type gpio_init_struct;
  /* enable the gpio clock */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the can tx, rx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9 | GPIO_PINS_8;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE9, GPIO_MUX_9);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE8, GPIO_MUX_9);
}

/**
  *  @brief  can configiguration.
  *  @param  none
  *  @retval the result of can_configuration
  *          this parameter can be one of the following values:
  *          SUCCESS or ERROR
  */
error_status can_configuration(void)
{
  can_base_type can_base_struct;
  can_baudrate_type can_baudrate_struct;
  can_filter_init_type can_filter_init_struct;
  
  /* as specified in CAN protocol, the maximum allowable oscillator tolerance is 1.58%. 
     The HICK accuracy does not meet the clock requirements in CAN protocol. to guarantee normal 
     communication, it is recommended to use HEXT as the system clock source. */
  if(crm_flag_get(CRM_HEXT_STABLE_FLAG) != SET)
  {
    return ERROR;
  }
  
  /* enable the can clock */
  crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);

  /* can base init */
  can_default_para_init(&can_base_struct);
  can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;
  can_base_struct.ttc_enable = FALSE;
  can_base_struct.aebo_enable = TRUE;
  can_base_struct.aed_enable = TRUE;
  can_base_struct.prsf_enable = FALSE;
  can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED;
  can_base_struct.mmssr_selection = CAN_SENDING_BY_ID;
  can_base_init(CAN1, &can_base_struct);

  /* can baudrate, set boudrate = pclk/(baudrate_div *(1 + bts1_size + bts2_size)) */
  can_baudrate_struct.baudrate_div = 12;
  can_baudrate_struct.rsaw_size = CAN_RSAW_3TQ;
  can_baudrate_struct.bts1_size = CAN_BTS1_8TQ;
  can_baudrate_struct.bts2_size = CAN_BTS2_3TQ;
  if(can_baudrate_set(CAN1, &can_baudrate_struct) != SUCCESS)
  {
    return ERROR;
  }

  /* can filter 0 config */
  can_filter_init_struct.filter_activate_enable = TRUE;
  can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_LIST;
  can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;
  can_filter_init_struct.filter_number = 0;
  can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;
  can_filter_init_struct.filter_id_high = (((FILTER_EXT_ID1 << 3) >> 16) & 0xFFFF);   /* extended identifier is 29 bit */
  can_filter_init_struct.filter_id_low = ((FILTER_EXT_ID1 << 3) & 0xFFFF) | 0x04;
  can_filter_init_struct.filter_mask_high = ((FILTER_EXT_ID2 << 3) >> 16) & 0xFFFF; /* extended identifier is 29 bit */
  can_filter_init_struct.filter_mask_low = ((FILTER_EXT_ID2 << 3) & 0xFFFF) | 0x04;
  can_filter_init(CAN1, &can_filter_init_struct);

  /* can filter 1 config */
  can_filter_init_struct.filter_activate_enable = TRUE;
  can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_LIST;
  can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;
  can_filter_init_struct.filter_number = 1;
  can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;
  can_filter_init_struct.filter_id_high = FILTER_STD_ID1 << 5;  /* standard identifier is 11 bit */
  can_filter_init_struct.filter_id_low = 0;
  can_filter_init_struct.filter_mask_high = FILTER_STD_ID2 << 5;/* standard identifier is 11 bit */
  can_filter_init_struct.filter_mask_low = 0;
  can_filter_init(CAN1, &can_filter_init_struct);

  /* can interrupt config */
  nvic_irq_enable(CAN1_SE_IRQn, 0x00, 0x00);
  nvic_irq_enable(CAN1_RX0_IRQn, 0x00, 0x00);
  can_interrupt_enable(CAN1, CAN_RF0MIEN_INT, TRUE);

  /* error interrupt enable */
  can_interrupt_enable(CAN1, CAN_ETRIEN_INT, TRUE);
  can_interrupt_enable(CAN1, CAN_EOIEN_INT, TRUE);
  
  return SUCCESS;
}

/**
  *  @brief  can transmit data
  *  @param  none
  *  @retval none
  */
static void can_transmit_data(void)
{
  uint8_t transmit_mailbox;
  can_tx_message_type tx_message_struct;

  /* transmit FILTER_STD_ID1 */
  tx_message_struct.standard_id = FILTER_STD_ID1;
  tx_message_struct.extended_id = 0;
  tx_message_struct.id_type = CAN_ID_STANDARD;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);

  /* transmit FILTER_STD_ID2 */
  tx_message_struct.standard_id = FILTER_STD_ID2;
  tx_message_struct.extended_id = 0;
  tx_message_struct.id_type = CAN_ID_STANDARD;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);

  /* transmit FILTER_STD_ID3 */
  tx_message_struct.standard_id = FILTER_STD_ID3;
  tx_message_struct.extended_id = 0;
  tx_message_struct.id_type = CAN_ID_STANDARD;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);

  /* transmit FILTER_EXT_ID1 */
  tx_message_struct.standard_id = 0;
  tx_message_struct.extended_id = FILTER_EXT_ID1;
  tx_message_struct.id_type = CAN_ID_EXTENDED;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);

  /* transmit FILTER_EXT_ID2 */
  tx_message_struct.standard_id = 0;
  tx_message_struct.extended_id = FILTER_EXT_ID2;
  tx_message_struct.id_type = CAN_ID_EXTENDED;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);

  /* transmit FILTER_EXT_ID3 */
  tx_message_struct.standard_id = 0;
  tx_message_struct.extended_id = FILTER_EXT_ID3;
  tx_message_struct.id_type = CAN_ID_EXTENDED;
  tx_message_struct.frame_type = CAN_TFT_DATA;
  tx_message_struct.dlc = 8;
  tx_message_struct.data[0] = 0x11;
  tx_message_struct.data[1] = 0x22;
  tx_message_struct.data[2] = 0x33;
  tx_message_struct.data[3] = 0x44;
  tx_message_struct.data[4] = 0x55;
  tx_message_struct.data[5] = 0x66;
  tx_message_struct.data[6] = 0x77;
  tx_message_struct.data[7] = 0x88;
  transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct);
  while(can_transmit_status_get(CAN1, (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL);
}

/**
  *  @brief  can1 interrupt function rx0
  *  @param  none
  *  @retval none
  */
void CAN1_RX0_IRQHandler(void)
{
  can_rx_message_type rx_message_struct;
  if(can_interrupt_flag_get(CAN1,CAN_RF0MN_FLAG) != RESET)
  {
    if(test_result == 4)
    {
      test_result = 0;
    }

    can_message_receive(CAN1, CAN_RX_FIFO0, &rx_message_struct);

    if((rx_message_struct.id_type == CAN_ID_STANDARD) && (rx_message_struct.standard_id == FILTER_STD_ID1))
      test_result++;
    else if((rx_message_struct.id_type == CAN_ID_STANDARD) && (rx_message_struct.standard_id == FILTER_STD_ID2))
      test_result++;
    else if((rx_message_struct.id_type == CAN_ID_EXTENDED) && (rx_message_struct.extended_id == FILTER_EXT_ID1))
      test_result++;
    else if((rx_message_struct.id_type == CAN_ID_EXTENDED) && (rx_message_struct.extended_id == FILTER_EXT_ID2))
      test_result++;
  }
}

/**
  *  @brief  can1 interrupt function se
  *  @param  none
  *  @retval none
  */
void CAN1_SE_IRQHandler(void)
{
  __IO uint32_t err_index = 0;
  if(can_interrupt_flag_get(CAN1,CAN_ETR_FLAG) != RESET)
  {
    err_index = CAN1->ests & 0x70;
    can_flag_clear(CAN1, CAN_ETR_FLAG);
    /* error type is stuff error */
    if(err_index == 0x00000010)
    {
      /* when stuff error occur: in order to ensure communication normally,
      user must restart can or send a frame of highest priority message here */
    }
  }
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
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  can_gpio_config();
  if(can_configuration() == ERROR)
  {
    /* CAN clock initialization error */
    while(1)
    {
    }
  }
  can_transmit_data();
  while(1)
  {
    if(test_result == 4)
    {
      at32_led_toggle(LED2);
      at32_led_toggle(LED3);
      at32_led_toggle(LED4);
      delay_sec(1);
    }
  }
}

/**
  * @}
  */

/**
  * @}
  */
