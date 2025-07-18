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
#include "julia_fpu.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_CORTEX_m4_fpu CORTEX_m4_fpu
  * @{
  */

#define  SCREEN_X_SIZE                   ((uint16_t)320)
#define  SCREEN_Y_SIZE                   ((uint16_t)240)
#define  ZOOM                            ((uint16_t)100)

uint8_t buffer[SCREEN_X_SIZE * SCREEN_Y_SIZE];

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  while(1)
  {
    at32_led_toggle(LED4);

    generate_julia_fpu(SCREEN_X_SIZE, SCREEN_Y_SIZE, SCREEN_X_SIZE / 2, SCREEN_Y_SIZE / 2, ZOOM, buffer);
  }
}

/**
  * @}
  */

/**
  * @}
  */
