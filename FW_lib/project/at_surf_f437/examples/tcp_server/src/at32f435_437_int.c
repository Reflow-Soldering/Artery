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
#include "netconf.h"
#include "at_surf_f437_board_pca9555.h"

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
  * @brief  this function handles timer6 overflow handler.
  * @param  none
  * @retval none
  */
void TMR6_DAC_GLOBAL_IRQHandler(void)
{
  if(tmr_interrupt_flag_get(TMR6, TMR_OVF_FLAG) != RESET)
  {
    /* Update the local_time by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    time_update();
    tmr_flag_clear(TMR6, TMR_OVF_FLAG);
  }
}

/**
  * @brief  this function handles dma interrupt request.
  * @param  none
  * @retval none
  */
void PCA_I2C_DMA_RX_IRQHandler(void)
{
  i2c_dma_rx_irq_handler(&hi2c_pca);
}

/**
  * @brief  this function handles dma interrupt request.
  * @param  none
  * @retval none
  */
void PCA_I2C_DMA_TX_IRQHandler(void)
{
  i2c_dma_tx_irq_handler(&hi2c_pca);
}

/**
  * @brief  this function handles i2c event interrupt request.
  * @param  none
  * @retval none
  */
void PCA_I2C_EVT_IRQHandler(void)
{
  i2c_evt_irq_handler(&hi2c_pca);
}

/**
  * @brief  this function handles i2c error interrupt request.
  * @param  none
  * @retval none
  */
void PCA_I2C_ERR_IRQHandler(void)
{
  i2c_err_irq_handler(&hi2c_pca);
}

/**
  * @brief  this function handles exint interrupt request.
  * @param  none
  * @retval none
  */
void EXINT3_IRQHandler(void)
{
  pca9555_irq_handler();
}
