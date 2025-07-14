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
#include "at_surf_f437_board_delay.h"
#include <stdio.h>


/*
*USART���� UART��� ���� USART1�����
*baud rate 115200bps
*
*/


/*
*	1. UART init
*	2. printf GNU ���� ���� �ʼ�!!
*	3. Printf �غ���
*
*/


/* support printf function, usemicrolib is unnecessary */
#if (__ARMCC_VERSION > 6000000)
  __asm (".global __use_no_semihosting\n\t");
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
  FILE __stdout;
#else
 #ifdef __CC_ARM
  #pragma import(__use_no_semihosting)
  struct __FILE
  {
    int handle;
  };
  FILE __stdout;
  void _sys_exit(int x)
  {
    x = x;
  }
  /* __use_no_semihosting was requested, but _ttywrch was */
  void _ttywrch(int ch)
  {
    ch = ch;
  }
 #endif
#endif

#if defined (__GNUC__) && !defined (__clang__)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  retargets the c library printf function to the usart.
  * @param  none
  * @retval none
  */
PUTCHAR_PROTOTYPE
{
	/*
  1. USART1���� ���۹��۰� ����ִ��� Ȯ�� -> �ȵǾ������� ���ѷ���
  2. ���۹��۰� ä������ ������
  3. ���۹��ۿ��� �۽��� �Ϸ� �Ǿ����� Ȯ�� -> �Ϸ�Ǹ� ch����
  */
	
  while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
  usart_data_transmit(USART1, (uint16_t)ch);
  while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET);
  return ch;
}

#if (defined (__GNUC__) && !defined (__clang__)) || (defined (__ICCARM__))
#if defined (__GNUC__) && !defined (__clang__)
int _write(int fd, char *pbuffer, int size)
#elif defined ( __ICCARM__ )
#pragma module_name = "?__write"
int __write(int fd, char *pbuffer, int size)
#endif
{
  for(int i = 0; i < size; i ++)
  {
    while(usart_flag_get(PRINT_UART, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(PRINT_UART, (uint16_t)(*pbuffer++));
    while(usart_flag_get(PRINT_UART, USART_TDC_FLAG) == RESET);
  }

  return size;
}
#endif


__IO uint32_t time_cnt = 0;


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
	// Uart Init
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	
	delay_init();
	#if defined (__GNUC__) && !defined (__clang__)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif

	//TX init 
  //����ü �ʱ�ȭ�� �� ���� �� �� �����Ƿ� �Ѳ����� ���� ���� ������ �� ���ϰ���
  
	gpio_init_type io_usart1 = 
	{
		.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE,
		.gpio_mode = GPIO_MODE_MUX,
		.gpio_out_type = GPIO_OUTPUT_PUSH_PULL,
		.gpio_pins = GPIO_PINS_9,
		.gpio_pull = GPIO_PULL_NONE,
	};
	gpio_init(GPIOA, &io_usart1);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_7);


	gpio_init(GPIOA, &io_usart1);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_7);
	
	
	usart_init(USART1,115200, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(USART1, TRUE);
	usart_receiver_enable(USART1, TRUE); 
	usart_enable(USART1, TRUE);

  /* output a message on hyperterminal using printf function */
  printf("usart printf example: retarget the c library printf function to the usart\r\n");

  /*
  scanf()�Լ��� ���� �ʴ� ������ �����ϴ�.
  �Ϲ����� C ���α׷����̶�� ���� ���α׷����� �����ϰ�����
  �߿����� ��� �ٸ� ������ �����ؾ��ϴµ�, �Է��� �� ������ ��ٷ����ϹǷ�,
  �ٸ� ������ ������ ������ �� ����.
  ���������δ� printf()�� �ᵵ ���� �����Ƿ� �����Ѵ�.
  �׷� RX�� ��� �׽�Ʈ �ϴ���?
  MCU ���� USART2������ �����Ͽ� USART1�� ������ �Ǿ����� Ȯ���ϸ� �ȴ�.
  */
  while(1)
  {
    printf("usart printf counter: %u\r\n",time_cnt++);
    delay_sec(1);
  }
}

/**
  * @}
  */

/**
  * @}
  */
