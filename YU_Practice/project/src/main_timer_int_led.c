/**
  **************************************************************************
  * @file     main_gpio_int.c
  * @brief    main program
  **************************************************************************
  *                       This is button interrupt operation program
  *
  * This is modified from Artery BSP example the key
	*  When press key1 or 2, RGB LED is on
	* I am a beginner! I made it, so you can made it too.
  * Reference code is buzz_main.c in AT_SURF_MAIN
  **************************************************************************
  */
	
	
#include "at32f435_437.h"
#include "at32f435_437_clock.h"
#include "at_surf_f437_board_delay.h"

/*
*  1. TMR1 초기화 - 1초, 업카운트, Overflow Flag 인터럽트 진행
*  2. LED RED 초기화 - 1초마다 Set, Reset
*  3. 연속 반복
*/

/* 인터럽트 사용하여 인터럽트 함수 안에는 변수를 1로 세팅만 할 것임
*   빨강 LED와 Advanced Timer인 TMR1사용
*////

		
	
/**
  * @brief  main program
  * @param  none
  * @retval none
  */
int main(void)
{
	
	extern uint8_t Tim_Int_Flag;
  /* initial system clock */
  system_clock_config();	//288MHz


  /* initialize delay */
  delay_init();

  /* initialize key */
	//1. 초기화
	//LED Red 및 TMR1리셋
	
	//구조체 생성
	gpio_init_type gpio_init_struct;
	
	
	//버스연결 - TMR1, PORTB
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);	//LED RED Bus
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK,TRUE);	//Timer 1 Bus
	
	//LED 초기화 Red
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_10);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOB, &gpio_init_struct);
	
	
	//LED RED OFF
	gpio_bits_set(GPIOB,GPIO_PINS_10);
	
	//timier1 초기화
	//1초 만들기(1Hz = 1초) 완전한 1초 안됨
	tmr_base_init(TMR1, 0x2712, 0x7077);		//288MHz Period, No Division
	
	//Upcount Mode, TMR1
	tmr_cnt_dir_set(TMR1,TMR_COUNT_UP);
	
	//Timer Interrupt Enable
	tmr_interrupt_enable(TMR1, TMR_OVF_INT, TRUE);

	/* Enable NVIC overflow interrupt */
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(TMR1_OVF_TMR10_IRQn, 0, 0);
	
	//Use Timer Interrupt Use
	tmr_counter_enable(TMR1,TRUE);
  
	while(1)
  {
		//클리어를 인터럽트에서 함
		if(Tim_Int_Flag == 1)
		{
			Tim_Int_Flag = 0;
			if(gpio_output_data_bit_read(GPIOB, GPIO_PINS_10) == RESET)
			{
				gpio_bits_write(GPIOB, GPIO_PINS_10, TRUE);
			}
			else
			{
				gpio_bits_write(GPIOB, GPIO_PINS_10, FALSE);
			}
		}
		// 이번에는 여기서 하면 1초 On 2초 Off임
		//Tim_Int_Flag = 0;
  }
}