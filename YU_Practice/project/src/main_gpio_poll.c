/**
  **************************************************************************
  * @file     main_gpio_poll.c
  * @brief    main program
  **************************************************************************
  *                       This is Buzzer beep operation program
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
//#include "at_surf_f437_board_key.h"
//#include "at_surf_f437_board_lcd.h"

/*
* 1. GPIO ��ư �ʱ�ȭ �Ѵ�. 
*     GPIO ��ư�� GPIOA-0�� GPIOC-13�̴�. Input���� �����Ѵ�.
* 2. GPIO RGB LED �ʱ�ȭ �Ѵ�.
*     GPIO RGB�� ���� �Ʒ��� ����.
*     RED -> GPIOB-10
*     GREEN -> GPIOD-13
*     YELLOW -> GPIOB-5
* 3. 1�� Ű�� ������ ���� LED�� 2�� Ű�� ������ �ʷ� LED�� �Ѵ�
* 4. �� ��ư�� ���ÿ� ������ �Ķ� LED�� ������ �Ѵ�.
* 5. �� ��ư�� ������ ������ �������� �����Ѵ�.
*/

/**
  * @brief  main program
  * @param  none
  * @retval none
  */
int main(void)
{
  /* initial system clock */
  system_clock_config();

  /* initialize delay */
  delay_init();

  /* initialize key */
	//1. �ʱ�ȭ
	//LED �� ��ư ��Ʈ ����
	
	//����ü ����
	gpio_init_type gpio_init_struct;
	
	
	//��Ʈ Ŭ�� ���� ����
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
	
	//��Ʈ �ʱ�ȭ
	//LED ����
	//����, �Ķ� �ʱ�ȭ
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_10|GPIO_PINS_5);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOB, &gpio_init_struct);
	
	//�ʷ� �ʱ�ȭ
	gpio_init_struct.gpio_pins = GPIO_PINS_13;
	gpio_init(GPIOD, &gpio_init_struct);
	
	
	//Key1 �ʱ�ȭ
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_0);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOA,&gpio_init_struct);
	
	
	//key 2 �ʱ�ȭ
	gpio_init_struct.gpio_pins = GPIO_PINS_13;
	gpio_init(GPIOC,&gpio_init_struct);
	
	
	
	//LED�� sink�̹Ƿ� set�� reset�� �ݴ���!
	
  uint8_t key1, key2;
  while(1)
  {
		key1 = gpio_input_data_bit_read(GPIOA,GPIO_PINS_0);
		key2 = gpio_input_data_bit_read(GPIOC,GPIO_PINS_13);
		
		if((key1 == 0) &&(key2 == 0))
		{
			//��� ����
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		else if((key1 == 1) &&(key2 == 0))
		{
			//���� ����
			gpio_bits_reset(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		else if((key1 == 0) &&(key2 == 1))
		{
			//�ʷ� ����
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_reset(GPIOD,GPIO_PINS_13);
		}
		else
		{
			//�Ķ� ����
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_reset(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		
		//LED �ʱ�ȭ �� �ƴ��� �׽�Ʈ ��
		/*
		gpio_bits_set(GPIOB,GPIO_PINS_10);
		delay_ms(200);
		gpio_bits_set(GPIOD,GPIO_PINS_13);
		delay_ms(200);
		gpio_bits_set(GPIOB,GPIO_PINS_5);
		delay_ms(200);
		gpio_bits_reset(GPIOB,GPIO_PINS_10);
		delay_ms(200);
		gpio_bits_reset(GPIOD,GPIO_PINS_13);
		delay_ms(200);
		gpio_bits_reset(GPIOB,GPIO_PINS_5);
		delay_ms(200);
    */
		
  }
}