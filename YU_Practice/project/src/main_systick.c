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
System Core Clock은 초기에 8Mhz이다 at32f435_437_conf.h에 있음
at32f435_437_clock.c에 system_core_clock_upodate 함수가 완료되는 순간
288Mhz로 바뀌게 되고, 결국 MS_TICK은 288000000나누기 1000이 되어
288000이 되는것이다.
************************************************************/

/*****************************************
내용
1. MS_TICK은 1ms를 대체하는 것이다. 나누기 1천을 하였으므로
2. DELAY는 임의로 설정 가능하다. 예시는 200ms이다.
3. 반드시 at32f435_437_int.c에 systick_handler를 extern하여 
    구동하여야 한다.
4. export된 함수는 SysTick_Handler이며, 이 안에 systick_handler함수를 넣어야 한다.
5. 이 SysTick_Handler 에 3번 처럼 해야 하고, SysTick_Handler 는 startup_at32f435_437.s에 있다.


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
	//Reload 마스크는 Systick이 24비트이므로 FFFFFF이다
	//즉 내가 원하는 값이 빼기 1을 해서 FFFFFF보다 크다면 에러인것
	//오버플로우 발생으로 진행할수 없게 만듬
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);
  }

	
	//리로드 레지스터. 다운카운트하여 0이 될 때 복귀 될 값을 저장
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
	
	//인터럽트 우선순위 설정 우선순위는 7
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
	
	//현재 systick 값 설정 - 0으로 초기화
  SysTick->VAL   = 0UL;
	
	//SysTick_CTRL_TICKINT_Msk -> Systick 타이머를 Down Count함
	//SysTick_CTRL_ENABLE_Msk -> Systick 타이머를 시작한다!
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
	

	//총 288Mhz를 가지고 움직임!!!
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);

  /* config systick reload value and enable interrupt */
  systick_interrupt_config(MS_TICK);
	
	/*GPIO LED Red Init*/
	//GPIO Poll에서 가져옴
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
