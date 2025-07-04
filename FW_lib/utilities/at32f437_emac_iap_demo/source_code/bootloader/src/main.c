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
#include "stdlib.h"
#include "netconf.h"
#include "flash.h"
#include "iap.h"
#include "tmr.h"
#include "httpserver.h"
#include "tftpserver.h"

/** @addtogroup UTILITIES_examples
  * @{
  */

/** @addtogroup EMAC_iap_bootloader
  * @{
  */

volatile uint32_t local_time = 0;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  error_status status;
  uint32_t useraddr, sector_size;
  system_clock_config();
  at32_board_init();
  
  uart_print_init(115200);

  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  delay_init();

  /* check iap upgrade */
  if(at32_button_press()==USER_BUTTON)
  {
    iap_command_handle();
  }

  /* check iap_upgrade_flag flag */
  if(flash_upgrade_flag_read() == RESET)
  {
    /* check app starting address whether 0x08xxxxxx */
    if(((*(uint32_t*)(APP_START_SECTOR_ADDR + 4)) & 0xFF000000) == 0x08000000)
      app_load(APP_START_SECTOR_ADDR);
  }

  status = emac_system_init();

  while(status == ERROR);

  tcpip_stack_init();

  /* init flash */
  flash_unlock();

  if(SECTOR_SIZE_INDEX == 0x04) /* 4kb/sector size */
  {
    sector_size = 4096;
  }
  else /* 2kb/sector size */
  {
    sector_size = 2048;
  }

  for(useraddr = APP_START_SECTOR_ADDR; useraddr <= APP_LAST_SECTOR_ADDR; useraddr += sector_size)
  {
    flash_sector_erase(useraddr);
  }
#ifdef USE_IAP_HTTP
  /* initialize the http server */
  iap_httpd_init();
#endif

#ifdef USE_IAP_TFTP
  /* initialize the tftp server */
  iap_tftpd_init();
#endif

  tmr_init();

  while(1)
  {
    lwip_rx_loop_handler();
    
    lwip_periodic_handle(local_time);
  }
}

/**
  * @}
  */

/**
  * @}
  */
