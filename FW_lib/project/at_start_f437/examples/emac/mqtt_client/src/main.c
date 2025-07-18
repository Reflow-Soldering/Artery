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
#include "at32_emac.h"
#include "netconf.h"
#include "mqtt_client.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_EMAC_mqtt_client EMAC_mqtt_client
  * @{
  */

#define DELAY                            100
#define FAST                             1
#define SLOW                             4
uint8_t g_speed = FAST;
volatile uint32_t local_time = 0;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;

  system_clock_config();
  
  uart_print_init(115200);

  at32_board_init();

  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();

  status = emac_system_init();

  while(status == ERROR);

  tcpip_stack_init();
  
  mqtt_client_init();

  while(1)
  {
    /* lwip receive handle */
    lwip_rx_loop_handler();
    
    /*timeout handle*/
    lwip_periodic_handle(local_time);
  }
}

/**
  * @}
  */

/**
  * @}
  */
