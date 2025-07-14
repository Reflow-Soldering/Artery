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
    Timer3 CH2를 이용하여 ADC 조절값으로 PWM을 조절해보자 그리고 Printf도 해보자
    1. Timer3 CH2 초기화
    2. printf 문구 초기화(USART)
    3. ADC 초기화
    4. LCD 데모버전이라 못씀
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
  1. USART1에서 전송버퍼가 비어있는지 확인 -> 안되어있으면 무한루프
  2. 전송버퍼가 채워지면 전송함
  3. 전송버퍼에서 송신이 완료 되었는지 확인 -> 완료되면 ch리턴
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
		//변수는 그대로 쓸 것
	
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

  //버스 연결
  crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	delay_init();
	
	#if defined (__GNUC__) && !defined (__clang__)
  setvbuf(stdout, NULL, _IONBF, 0);
	#endif
	
	//주파수 -> 10Hz (100ms)
	//288MHz/28800/1000
	tmr_base_init(TMR3, TMR_PERIOD, TMR_DIVIDE);
	
	//클럭 내부 Divide 여부 - 이미 위에서 나눴으므로 안씀
	//tmr_base_init은 내부 카운트로 divide
	//tmr_clock_source_div_set은 물리적으로 나눔
	tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);
	
	//PWM output channel init
	//구조체 변수 초기화
	tmr_output_default_para_init(&tmr_oc_init_struct);
	//A채널 출력 - B출력은 A의 반대임
	tmr_oc_init_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
	//준비상태 안씀
	tmr_oc_init_struct.oc_idle_state = FALSE;
	//출력 Enable
	tmr_oc_init_struct.oc_output_state = TRUE;
	//초기 극성 설정(high, low)
	tmr_oc_init_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	//타이머 채널 최종 설정 
	tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_2, &tmr_oc_init_struct);
	
	
	//채널값 설정 1000의 중간값인 500 설정
	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 500);
	
	//최종 출력채널 enable -> 얘는 내부에서 준비가 완료 된 상태
	tmr_channel_enable(TMR3, TMR_SELECT_CHANNEL_3, TRUE);
	
	//출력 채널 핀 연결
	tmr_output_enable(TMR3, TRUE);
	
	
	//여기까지 TMR3 출력 설정//
	
	
	/*
	ADC 설정
	*/
	adc_common_config_type adc_common_init_struct;					//mode 및 조건 설정용
	adc_base_config_type adc_base_init_struct;											//데이터 방식 및 규정 설정용
	gpio_init_type gpio_init_struct;
	
	//ADC BUS 연결
	
	//GPIO BUS 연결
	
	//GPIO 변수 초기화 
	gpio_default_para_init(&gpio_init_struct);
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER; //MODERATE와 STRONGER의 차이점은 출력이 많이 나가냐 아니냐 차이 GPIO Drive capability register 참조
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;			//출력설정이지만 우리는 analog 쓰므로 그대로 둔다
	gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
	gpio_init_struct.gpio_pins = GPIO_PINS_5;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOA, &gpio_init_struct);
	
	////GPIO 초기화 완료
	
	
	////USART 초기화 진행
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
	
	
	
	////ADC 공통 변수 초기화
	
	adc_common_default_para_init(&adc_common_init_struct);
	//독립모드로 사용
	adc_common_init_struct.combine_mode = ADC_INDEPENDENT_MODE; //하나만 쓰겠다는 뜻인가!
	
	//DMA모드는 안씀 - 여러 ADC를 사용하지 않고 독립적으로 사용하므로
	adc_common_init_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;
	//DMA 응답 반복 여부도 무시
	adc_common_init_struct.common_dma_request_repeat_state = FALSE;
	
	//샘플링의 간격은 얼마나 할 것인가???
	adc_common_init_struct.sampling_interval = ADC_SAMPLING_INTERVAL_5CYCLES;
	
	//이건 모르겠다.. 내부 온도센서의 레퍼런스 전압 구동 여부로 추정
	adc_common_init_struct.tempervintrv_state = FALSE;
	
	//ADC Vbat 설정 회로에서 기본으로 Coincell이 아니라 3.3V로 점퍼 연결 확인- 안씀!
	adc_common_init_struct.vbat_state = FALSE;
	//최종 설정 진행
	adc_common_config(&adc_common_init_struct);
	
	//ADC base 기능 설정 진행
	
	adc_base_default_para_init(&adc_base_init_struct);
	
	//시퀀스 모드 설정
	adc_base_init_struct.sequence_mode = TRUE;
	//반복모드 설정
	adc_base_init_struct.repeat_mode = FALSE;
	//데이터 정렬 방식
	adc_base_init_struct.data_align = ADC_RIGHT_ALIGNMENT;
	//ADC 수??? 모르겠음
	adc_base_init_struct.ordinary_channel_length = 1;
	//base adc 규정
	adc_base_config(ADC1, &adc_base_init_struct);
	//분해능 결정(resolution)
	adc_resolution_set(ADC1, ADC_RESOLUTION_12B);
	
	//adc 채널 및 샘플링 시간 규정
	//(어떤 ADC임?, 채널은 몇번임?, 우선순위는 몇번임?, 몇사이클 측정할거임?)
	adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 1, ADC_SAMPLETIME_47_5);

	//오버샘플링 비율과 트리거모드 사용 여부
	adc_ordinary_oversample_trig_enable(ADC1, FALSE);
	
	//오버샘플링을 반복할것인가?
	adc_ordinary_oversample_restart_set(ADC1, ADC_OVERSAMPLE_CONTINUE);
	
	//오버샘플링 인에이블
	adc_ordinary_oversample_enable(ADC1, TRUE);
	
	//ADC 인에이블
	adc_enable(ADC1,TRUE);
	//ADC1이 준비가 되었는지 확인
	while(adc_flag_get(ADC1, ADC_RDY_FLAG) == RESET);
	
	//ADC 캘리브레이션
	adc_calibration_init(ADC1);
	//ADC 캘리브레이션 완료될때까지 대기
	//RESET 혹은 0으로 받아야 완료가 된 것
	while(adc_calibration_init_status_get(ADC1));
	
	//adc 캘리브레이션 시작
	adc_calibration_start(ADC1);
	//RESET 혹은 0으로 받아야 완료가 된 것
	while(adc_calibration_status_get(ADC1))
	{
	};
	
	///여기까지가 ADC 끝
	


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

