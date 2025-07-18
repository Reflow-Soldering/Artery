/**
  **************************************************************************
  * @file     at32f435_437_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f435_437_int.h"
#include "smartcard_config.h"

/** @addtogroup AT32F435_periph_examples
  * @{
  */

/** @addtogroup 435_USART_smartcard
  * @{
  */

extern uint32_t card_inserted;

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* go to infinite loop when hard fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* go to infinite loop when memory manage exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* go to infinite loop when bus fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* go to infinite loop when usage fault exception occurs */
  while(1)
  {
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
}

/**
  * @brief  this function handles external lines 9 to 5 interrupt request.
  * @param  none
  * @retval none
  */
void EXINT9_5_IRQHandler(void)
{
  /* reset smart card cmdvcc */
  gpio_bits_reset(SC_CMDVCC_PORT, SC_CMDVCC_PIN);

  /* rst low */
  gpio_bits_reset(SC_RESET_PORT, SC_RESET_PIN);

  /* rst high */
  gpio_bits_set(SC_RESET_PORT, SC_RESET_PIN);

  /* clear sc exint line pending bit */
  exint_flag_clear(SC_OFF_EXINT);

  /* smart card detected */
  card_inserted = 1;
}

/**
  * @brief  this function handles sc_usart global interrupt request.
  * @param  none
  * @retval none
  */
void SC_USART_IRQHandler(void)
{
  /* if the sc_usart detects a parity error */
  if(usart_interrupt_flag_get(SC_USART, USART_PERR_FLAG) != RESET)
  {
    /* enable sc_usart rdbf interrupt (until receiving the corrupted byte) */
    usart_interrupt_enable(SC_USART, USART_RDBF_INT, TRUE);
    /* flush the sc_usart dr register */
    usart_data_receive(SC_USART);
  }

  if(usart_interrupt_flag_get(SC_USART, USART_RDBF_FLAG) != RESET)
  {
    /* disable sc_usart rdbf interrupt */
    usart_interrupt_enable(SC_USART, USART_RDBF_INT, FALSE);
    usart_data_receive(SC_USART);
  }
}

/**
  * @}
  */

/**
  * @}
  */
