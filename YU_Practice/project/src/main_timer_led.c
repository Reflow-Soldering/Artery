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
*  1. TMR1 �ʱ�ȭ - 1��, ��ī��Ʈ, Overflow Flag Ȯ��(polling ���)
*  2. LED RED �ʱ�ȭ - 1�ʸ��� Up, Down
*  3. ���� �ݺ�
*/

/* ���ͷ�Ʈ ���� �����÷ο� üũ�Ͽ� 1�ʸ��� ���� ����
*   ���� LED�� Advanced Timer�� TMR1���
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
	//1. �ʱ�ȭ
	//LED Red �� TMR1����
	
	//����ü ����
	gpio_init_type gpio_init_struct;
	
	
	//�������� - TMR1, PORTB
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);	//LED RED Bus
	crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK,TRUE);	//Timer 1 Bus
	
	//LED �ʱ�ȭ Red
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_10);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOB, &gpio_init_struct);
	
	
	//LED RED OFF
	gpio_bits_set(GPIOB,GPIO_PINS_10);
	
	//timier1 �ʱ�ȭ
	//1�� �����(1Hz = 1��)
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
			//���⼭ Ŭ���� �ϰų�
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
	//�̰����� Ŭ���� �ؾ� �մϴ�.
	//tmr_flag_clear(TMR1, TMR_OVF_FLAG);
  }
	//�̰����� Ŭ���� �ϸ� 1���� �˴ϴ�
	//tmr_flag_clear(TMR1, TMR_OVF_FLAG);
}