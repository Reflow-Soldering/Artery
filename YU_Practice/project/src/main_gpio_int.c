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
#include "at32f435_437_conf.h"
//#include "at_surf_f437_board_key.h"
//#include "at_surf_f437_board_lcd.h"

/*
* 1. GPIO ��ư �ʱ�ȭ �Ѵ�. 
*     GPIO ��ư�� GPIOA-0�� GPIOC-13�̴�. Input�� interrupt falling edge�� �����Ѵ�.
* 2. GPIO RGB LED �ʱ�ȭ �Ѵ�.
*     GPIO RGB�� ���� �Ʒ��� ����.
*     RED -> GPIOB-10
*     GREEN -> GPIOD-13
*     BLUE -> GPIOB-5
* 3. 1�� Ű�� 1ȸ ������ ���� LED ������ 2ȸ°���� ������
* 4. 2�� Ű�� 1ȸ ������ �ʷ� LED ������, 2ȸ°���� ������.
* 5. �� ��ư�� ��� 1ȸ ������ �Ķ� LED�� ������ 2ȸ°���� ������.
* 
*/

uint8_t key1, key2;
void EXINT0_IRQHandler(void)
{
		if(gpio_input_data_bit_read(GPIOA, GPIO_PINS_0) == RESET)
		{
			key1++;
			key1 = key1%2;
			exint_flag_clear(EXINT_LINE_0);
		}
}


void EXINT15_10_IRQHandler(void)
{
	if(gpio_input_data_bit_read(GPIOC, GPIO_PINS_13) == RESET)
	{

		key2++;
		key2 = key2%2;
		exint_flag_clear(EXINT_LINE_13);
	}
}
		
	
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
	exint_init_type gpio_exint_struct;
	
	
	
	//��Ʈ Ŭ�� ���� ����
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
	
	//GPIO input ����(����ġ)
	//GPIO A ����ġ �� 0�� input ó��
	/*
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = GPIO_PINS_0;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOA, &gpio_init_struct);
	
	//GPIO C ����ġ �� 13�� input ó��
	gpio_init_struct.gpio_pins = GPIO_PINS_13;
	
	gpio_init(GPIOD, &gpio_init_struct);
	*/
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
	
	
	//���ͷ�Ʈ ����
	scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);
	scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOC,SCFG_PINS_SOURCE13);
	
	//����ü �ʱ�ȭ
	exint_default_para_init(&gpio_exint_struct);
	
	//�ܺ� ���ͷ�Ʈ �ʱ�ȭ
	//exint 0�� ���� �ʱ�ȭ
	
	gpio_exint_struct.line_enable = TRUE;
	gpio_exint_struct.line_mode = EXINT_LINE_INTERRUPT;
	gpio_exint_struct.line_select = EXINT_LINE_0;
	gpio_exint_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
	exint_init(&gpio_exint_struct);
	
	nvic_irq_enable(EXINT0_IRQn, 0, 0);
	
	//exint 13�� �ʱ�ȭ
	gpio_exint_struct.line_select = EXINT_LINE_13;
	exint_init(&gpio_exint_struct);
	
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
	nvic_irq_enable(EXINT15_10_IRQn,0,1);
	
	//LED�� sink�̹Ƿ� set�� reset�� �ݴ���!
	
 // uint8_t key1, key2;
 
	gpio_bits_set(GPIOB,GPIO_PINS_10);
	gpio_bits_set(GPIOB,GPIO_PINS_5);
	gpio_bits_set(GPIOD,GPIO_PINS_13);
  
	while(1)
  {
		
		if((key1 == 0)&&(key2 == 0))
		{
			//��� ����
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
			
		}
		else if((key1 == 1)&&(key2 == 0))
		{
			//���� ����
			gpio_bits_reset(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		else if((key1 == 0) && (key2 == 1))
		{
			//�ʷ� ����
			gpio_bits_reset(GPIOD,GPIO_PINS_13);
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
		}
		else
		{
			//�Ķ� ����
			gpio_bits_reset(GPIOB, GPIO_PINS_5);
			gpio_bits_set(GPIOB,GPIO_PINS_10);
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