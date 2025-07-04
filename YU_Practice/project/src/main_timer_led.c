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
*  1. TMR1 초기화 - 1초, 업카운트, Overflow Flag 확인(polling 방식)
*  2. LED RED 초기화 - 1초마다 Up, Down
*  3. 연속 반복
*/

/* 인터럽트 없이 오버플로우 체크하여 1초마다 반전 진행
*   빨강 LED와 Advanced Timer인 TMR1사용
*/

		
	
/**
  * @brief  main program
  * @param  none
  * @retval none
  */
int main(void)
{
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
	//1초 만들기(1Hz = 1초)
	tmr_base_init(TMR1, 0x2712, 0x7077);		//288MHz Period, No Division
	
	//Upcount Mode, TMR1
	tmr_cnt_dir_set(TMR1,TMR_COUNT_UP);
	
	//No interrupt and Just Enable TMR1
	tmr_counter_enable(TMR1,TRUE);

  
	while(1)
  {
		
		if(tmr_flag_get(TMR1, TMR_OVF_FLAG) == SET)
		{
			//Flag Clear position change
			//여기서 클리어 하거나
			tmr_flag_clear(TMR1, TMR_OVF_FLAG);
			if(gpio_output_data_bit_read(GPIOB,GPIO_PINS_10) == RESET)
			{
				gpio_bits_write(GPIOB, GPIO_PINS_10, TRUE);
			}
			else
			{
				gpio_bits_write(GPIOB, GPIO_PINS_10, FALSE);
			}
		}
	//이곳에서 클리어 해야 합니다.
	//tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
	//이곳에서 클리어 하면 1번만 됩니다
	//tmr_flag_clear(TMR1, TMR_OVF_FLAG);
}