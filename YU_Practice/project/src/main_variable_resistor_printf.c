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

/**
  * @brief  main program
  * @param  none
  * @retval none
  */

  /*
    Timer3 CH2�� �̿��Ͽ� ADC ���������� PWM�� �����غ��� �׸��� Printf�� �غ���
    1. Timer3 CH2 �ʱ�ȭ
    2. printf ���� �ʱ�ȭ(USART)
    3. ADC �ʱ�ȭ
    4. LCD ��������̶� ����
  */
/*
-----USART Clang retarget
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


//TMR Frequency Define
#define 		TMR_PERIOD			1000
#define		TMR_DIVIDE  			28800  


int main(void)
{
		//������ �״�� �� ��
	
  float voltage = 0;
  volatile uint16_t adc_convert_value = 0;
	volatile uint32_t time_cnt = 0;

  /* initial system clock */
  system_clock_config();

  /* initial the nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /*
  TMR3 CH2 Init
  */
  tmr_output_config_type  tmr_oc_init_struct;

  //���� ����
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	delay_init();
	
	#if defined (__GNUC__) && !defined (__clang__)
  setvbuf(stdout, NULL, _IONBF, 0);
	#endif
	
	//���ļ� -> 10Hz (100ms)
	//288MHz/28800/1000
	tmr_base_init(TMR3, TMR_PERIOD, TMR_DIVIDE);
	
	//Ŭ�� ���� Divide ���� - �̹� ������ �������Ƿ� �Ⱦ�
	//tmr_base_init�� ���� ī��Ʈ�� divide
	//tmr_clock_source_div_set�� ���������� ����
	tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
	
	//PWM output channel init
	//����ü ���� �ʱ�ȭ
	tmr_output_default_para_init(&tmr_oc_init_struct);
	//Aä�� ��� - B����� A�� �ݴ���
	tmr_oc_init_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
	//�غ���� �Ⱦ�
	tmr_oc_init_struct.oc_idle_state = FALSE;
	//��� Enable
	tmr_oc_init_struct.oc_output_state = TRUE;
	//�ʱ� �ؼ� ����(high, low)
	tmr_oc_init_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	//Ÿ�̸� ä�� ���� ���� 
	tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_oc_init_struct);
	
	
	//ä�ΰ� ���� 1000�� �߰����� 500 ����
	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 500);
	
	//���� ���ä�� enable -> ��� ���ο��� �غ� �Ϸ� �� ����
	tmr_channel_enable(TMR3, TMR_SELECT_CHANNEL_3, TRUE);
	
	//��� ä�� �� ����
	tmr_output_enable(TMR3, TRUE);
	
	
	//������� TMR3 ��� ����//
	
	
	/*
	ADC ����
	*/
	adc_common_config_type adc_common_init_struct;					//mode �� ���� ������
	adc_base_config_type adc_base_init_struct;											//������ ��� �� ���� ������
	gpio_init_type gpio_init_struct;
	
	//ADC BUS ����
	
	//GPIO BUS ����
	
	//GPIO ���� �ʱ�ȭ 
	gpio_default_para_init(&gpio_init_struct);
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; //MODERATE�� STRONGER�� �������� ����� ���� ������ �ƴϳ� ���� GPIO Drive capability register ����
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;			//��¼��������� �츮�� analog ���Ƿ� �״�� �д�
	gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
	gpio_init_struct.gpio_pins = GPIO_PINS_5;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOA, &gpio_init_struct);
	
	////GPIO �ʱ�ȭ �Ϸ�
	
	
	////USART �ʱ�ȭ ����
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
	
	
	
	////ADC ���� ���� �ʱ�ȭ
	
	adc_common_default_para_init(&adc_common_init_struct);
	//�������� ���
	adc_common_init_struct.combine_mode = ADC_INDEPENDENT_MODE; //�ϳ��� ���ڴٴ� ���ΰ�!
	
	//DMA���� �Ⱦ� - ���� ADC�� ������� �ʰ� ���������� ����ϹǷ�
	adc_common_init_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;
	//DMA ���� �ݺ� ���ε� ����
	adc_common_init_struct.common_dma_request_repeat_state = FALSE;
	
	//���ø��� ������ �󸶳� �� ���ΰ�???
	adc_common_init_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;
	
	//�̰� �𸣰ڴ�.. ���� �µ������� ���۷��� ���� ���� ���η� ����
	adc_common_init_struct.tempervintrv_state = FALSE;
	
	//ADC Vbat ���� ȸ�ο��� �⺻���� Coincell�� �ƴ϶� 3.3V�� ���� ���� Ȯ��- �Ⱦ�!
	adc_common_init_struct.vbat_state = FALSE;
	//���� ���� ����
	adc_common_config(&adc_common_init_struct);
	
	//ADC base ��� ���� ����
	
	adc_base_default_para_init(&adc_base_init_struct);
	
	//������ ��� ����
	adc_base_init_struct.sequence_mode = TRUE;
	//�ݺ���� ����
	adc_base_init_struct.repeat_mode = FALSE;
	//������ ���� ���
	adc_base_init_struct.data_align = ADC_RIGHT_ALIGNMENT;
	//ADC ��??? �𸣰���
	adc_base_init_struct.ordinary_channel_length = 1;
	//base adc ����
	adc_base_config(ADC1, &adc_base_init_struct);
	//���ش� ����(resolution)
	adc_resolution_set(ADC1, ADC_RESOLUTION_12B);
	
	//adc ä�� �� ���ø� �ð� ����
	//(� ADC��?, ä���� �����?, �켱������ �����?, �����Ŭ �����Ұ���?)
	adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 1, ADC_SAMPLETIME_47_5);

	//�������ø� ������ Ʈ���Ÿ�� ��� ����
	adc_ordinary_oversample_trig_enable(ADC1, FALSE);
	
	//�������ø��� �ݺ��Ұ��ΰ�?
	adc_ordinary_oversample_restart_set(ADC1, ADC_OVERSAMPLE_CONTINUE);
	
	//�������ø� �ο��̺�
	adc_ordinary_oversample_enable(ADC1, TRUE);
	
	//ADC �ο��̺�
	adc_enable(ADC1,TRUE);
	//ADC1�� �غ� �Ǿ����� Ȯ��
	while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);
	
	//ADC Ķ���극�̼�
	adc_calibration_init(ADC1);
	//ADC Ķ���극�̼� �Ϸ�ɶ����� ���
	//RESET Ȥ�� 0���� �޾ƾ� �Ϸᰡ �� ��
	while(adc_calibration_init_status_get(ADC1));
	
	//adc Ķ���극�̼� ����
	adc_calibration_start(ADC1);
	//RESET Ȥ�� 0���� �޾ƾ� �Ϸᰡ �� ��
	while(adc_calibration_status_get(ADC1))
	{
	};
	
	///��������� ADC ��
	


  /* initialize variable resistor */
  //variable_resistor_init();

  /* display information */
  //lcd_string_show(10, 20, 310, 24, 24, (uint8_t *)"Variable resistor Test");

  while(1)
  {
    if(adc_flag_get(ADC1,ADC_OCCE_FLAG) ==SET )
    {
       adc_convert_value= ADC1->odt;
       /* calculate voltage value */
       voltage = 3.3 * adc_convert_value / 4095;

      
			printf("Voltage : %2.2f\n", voltage);
			printf("count %d\n", time_cnt);
			delay_sec(1);
		
		}
  }
}

