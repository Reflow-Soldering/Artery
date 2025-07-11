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

/** @addtogroup 435_TMR_parallel_synchro TMR_parallel_synchro
  * @{
  */

tmr_output_config_type tmr_oc_init_structure;
crm_clocks_freq_type crm_clocks_freq_struct = {0};
uint16_t timerperiod = 0;

void crm_configuration(void);
void gpio_configuration(void);

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  /* system peripheral configuration */
  crm_configuration();

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* gpio configuration */
  gpio_configuration();

  /* tmr base configuration */
  timerperiod = ((crm_clocks_freq_struct.apb1_freq * 2) / 750000 ) - 1;
  tmr_base_init(TMR2, timerperiod, 0);
  tmr_cnt_dir_set(TMR2, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR2, TMR_CLOCK_DIV1);
  tmr_base_init(TMR3, 9, 0);
  tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
  tmr_base_init(TMR4, 4, 0);
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR4, TMR_CLOCK_DIV1);

  /* output compare toggle mode configuration: channel1 */
  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_output_channel_config(TMR2, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_1, timerperiod/5);

  tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_1, 3);

  tmr_output_channel_config(TMR4, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_1, 3);

  /* primary mode selection: TMR2 */
  tmr_sub_sync_mode_set(TMR2, TRUE);
  tmr_primary_mode_select(TMR2, TMR_PRIMARY_SEL_OVERFLOW);

  /* subordinate mode selection: TMR3 */
  tmr_sub_mode_select(TMR3, TMR_SUB_HANG_MODE);
  tmr_trigger_input_select(TMR3, TMR_SUB_INPUT_SEL_IS1);
  /* subordinate mode selection: TMR4 */
  tmr_sub_mode_select(TMR4, TMR_SUB_HANG_MODE);
  tmr_trigger_input_select(TMR4, TMR_SUB_INPUT_SEL_IS1);

  /* tmr enable counter */
  tmr_counter_enable(TMR2, TRUE);
  tmr_counter_enable(TMR3, TRUE);
  tmr_counter_enable(TMR4, TRUE);

  while(1)
  {}
}

/**
  * @brief  configure the tmr3 pins.
  * @param  none
  * @retval none
  */
void gpio_configuration(void)
{
  gpio_init_type gpio_init_struct;

  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_0;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE0, GPIO_MUX_1);
  gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE6, GPIO_MUX_2);
  gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_2);
}

/**
  * @brief  configures the different peripheral clocks.
  * @param  none
  * @retval none
  */
void crm_configuration(void)
{
  /* tmr3 clock enable */
  crm_periph_clock_enable(CRM_TMR2_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);

  /* gpios clock enable */
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
}

/**
  * @}
  */

/**
  * @}
  */
