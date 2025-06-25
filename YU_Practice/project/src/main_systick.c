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
#include "at32f435_437.h"
#include "at32f435_437_clock.h"

/** @addtogroup AT32F437_periph_examples
  * @{
  */

/** @addtogroup 437_CORTEX_m4_systick_interrupt CORTEX_m4_systick_interrupt
  * @{
  */
	
/************************************************************
System Core Clock�� �ʱ⿡ 8Mhz�̴� at32f435_437_conf.h�� ����
at32f435_437_clock.c�� system_core_clock_upodate �Լ��� �Ϸ�Ǵ� ����
288Mhz�� �ٲ�� �ǰ�, �ᱹ MS_TICK�� 288000000������ 1000�� �Ǿ�
288000�� �Ǵ°��̴�.
************************************************************/

/*****************************************
����
1. MS_TICK�� 1ms�� ��ü�ϴ� ���̴�. ������ 1õ�� �Ͽ����Ƿ�
2. DELAY�� ���Ƿ� ���� �����ϴ�. ���ô� 200ms�̴�.
3. �ݵ�� at32f435_437_int.c�� systick_handler�� extern�Ͽ� 
    �����Ͽ��� �Ѵ�.
4. export�� �Լ��� SysTick_Handler�̸�, �� �ȿ� systick_handler�Լ��� �־�� �Ѵ�.
5. �� SysTick_Handler �� 3�� ó�� �ؾ� �ϰ�, SysTick_Handler �� startup_at32f435_437.s�� �ִ�.


*****************************************/

#define MS_TICK                          (system_core_clock / 1000U)
#define DELAY                            200/* 200 ms ticks */

/**
  * @brief  systick interrupt handler.
  * @param  none
  * @retval none
  */
void systick_handler(void)
{
  static uint32_t ticks = 0;
	
  ticks ++;

  /* toggle led */
  if(ticks > DELAY)
  {
		if(gpio_output_data_bit_read(GPIOB,GPIO_PINS_10) == RESET)
		{
			gpio_bits_set(GPIOB,GPIO_PINS_10);
		}
		else
		{
			gpio_bits_reset(GPIOB,GPIO_PINS_10);
		}
    
    ticks = 0;
  }
}

/**
  * @brief  config systick and enable interrupt.
  * @param  none
  * @retval none
  */
static uint32_t systick_interrupt_config(uint32_t ticks)
{
	//Reload ����ũ�� Systick�� 24��Ʈ�̹Ƿ� FFFFFF�̴�
	//�� ���� ���ϴ� ���� ���� 1�� �ؼ� FFFFFF���� ũ�ٸ� �����ΰ�
	//�����÷ο� �߻����� �����Ҽ� ���� ����
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);
  }

	
	//���ε� ��������. �ٿ�ī��Ʈ�Ͽ� 0�� �� �� ���� �� ���� ����
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
	
	//���ͷ�Ʈ �켱���� ���� �켱������ 7
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	
	//���� systick �� ���� - 0���� �ʱ�ȭ
  SysTick->VAL   = 0UL;
	
	//SysTick_CTRL_TICKINT_Msk -> Systick Ÿ�̸Ӹ� Down Count��
	//SysTick_CTRL_ENABLE_Msk -> Systick Ÿ�̸Ӹ� �����Ѵ�!
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk |
                   SysTick_CTRL_ENABLE_Msk;
  return (0UL);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

	  /* config systick clock source */
	

	//�� 288Mhz�� ������ ������!!!
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);

  /* config systick reload value and enable interrupt */
  systick_interrupt_config(MS_TICK);
	
	/*GPIO LED Red Init*/
	//GPIO Poll���� ������
	gpio_init_type gpio_init_struct;
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_10);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOB, &gpio_init_struct);
	

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
