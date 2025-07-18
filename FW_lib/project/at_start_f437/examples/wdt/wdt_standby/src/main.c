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

/** @addtogroup 437_WDT_WDT_standby WDT_WDT_standby
  * @{
  */

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  /* enable the pwc clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  if(crm_flag_get(CRM_WDT_RESET_FLAG) != RESET)
  {
    /* reset from wdt */
    crm_flag_clear(CRM_WDT_RESET_FLAG);

    at32_led_on(LED4);
  }
  else
  {
    /* reset from other mode */
    at32_led_off(LED4);
  }

  delay_ms(100);

  /* disable register write protection */
  wdt_register_write_enable(TRUE);

  /* set the wdt divider value */
  wdt_divider_set(WDT_CLK_DIV_4);

  /* set reload value

   timeout = reload_value * (divider / lick_freq )    (s)

   lick_freq     = 40000 Hz
   divider      = 4
   reload_value = 3000

   timeout = 3000 * (4 / 40000 ) = 0.3s = 300ms
  */
  wdt_reload_value_set(3000 - 1);

  /* reload wdt counter */
  wdt_counter_reload();
  
  /* enable wdt */
  wdt_enable();

  /* enter standby mode */
  pwc_standby_mode_enter();

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
