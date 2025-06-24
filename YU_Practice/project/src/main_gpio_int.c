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
* 1. GPIO 버튼 초기화 한다. 
*     GPIO 버튼은 GPIOA-0과 GPIOC-13이다. Input과 interrupt falling edge로 설정한다.
* 2. GPIO RGB LED 초기화 한다.
*     GPIO RGB는 각각 아래와 같다.
*     RED -> GPIOB-10
*     GREEN -> GPIOD-13
*     BLUE -> GPIOB-5
* 3. 1번 키를 1회 누르면 빨강 LED 켜지고 2회째에는 꺼진다
* 4. 2번 키를 1회 누르면 초록 LED 켜지고, 2회째에는 꺼진다.
* 5. 두 버튼이 모두 1회 눌리면 파랑 LED가 켜지고 2회째에는 꺼진다.
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
	//1. 초기화
	//LED 및 버튼 포트 리셋
	
	//구조체 생성
	gpio_init_type gpio_init_struct;
	exint_init_type gpio_exint_struct;
	
	
	
	//포트 클럭 버스 연결
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_GPIOD_PERIPH_CLOCK, TRUE);
	crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
	
	//GPIO input 설정(스위치)
	//GPIO A 스위치 핀 0번 input 처리
	/*
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = GPIO_PINS_0;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOA, &gpio_init_struct);
	
	//GPIO C 스위치 핀 13번 input 처리
	gpio_init_struct.gpio_pins = GPIO_PINS_13;
	
	gpio_init(GPIOD, &gpio_init_struct);
	*/
	//포트 초기화
	//LED 부터
	//빨강, 파랑 초기화
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pins = (GPIO_PINS_10|GPIO_PINS_5);
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	
	gpio_init(GPIOB, &gpio_init_struct);
	
	//초록 초기화
	gpio_init_struct.gpio_pins = GPIO_PINS_13;
	gpio_init(GPIOD, &gpio_init_struct);
	
	
	//인터럽트 설정
	scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOA, SCFG_PINS_SOURCE0);
	scfg_exint_line_config(SCFG_PORT_SOURCE_GPIOC,SCFG_PINS_SOURCE13);
	
	//구조체 초기화
	exint_default_para_init(&gpio_exint_struct);
	
	//외부 인터럽트 초기화
	//exint 0번 부터 초기화
	
	gpio_exint_struct.line_enable = TRUE;
	gpio_exint_struct.line_mode = EXINT_LINE_INTERRUPT;
	gpio_exint_struct.line_select = EXINT_LINE_0;
	gpio_exint_struct.line_polarity = EXINT_TRIGGER_FALLING_EDGE;
	exint_init(&gpio_exint_struct);
	
	nvic_irq_enable(EXINT0_IRQn, 0, 0);
	
	//exint 13번 초기화
	gpio_exint_struct.line_select = EXINT_LINE_13;
	exint_init(&gpio_exint_struct);
	
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
	nvic_irq_enable(EXINT15_10_IRQn,0,1);
	
	//LED는 sink이므로 set과 reset이 반대임!
	
 // uint8_t key1, key2;
 
	gpio_bits_set(GPIOB,GPIO_PINS_10);
	gpio_bits_set(GPIOB,GPIO_PINS_5);
	gpio_bits_set(GPIOD,GPIO_PINS_13);
  
	while(1)
  {
		
		if((key1 == 0)&&(key2 == 0))
		{
			//모두 꺼짐
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
			
		}
		else if((key1 == 1)&&(key2 == 0))
		{
			//빨강 켜짐
			gpio_bits_reset(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		else if((key1 == 0) && (key2 == 1))
		{
			//초록 켜짐
			gpio_bits_reset(GPIOD,GPIO_PINS_13);
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOB,GPIO_PINS_5);
		}
		else
		{
			//파랑 켜짐
			gpio_bits_reset(GPIOB, GPIO_PINS_5);
			gpio_bits_set(GPIOB,GPIO_PINS_10);
			gpio_bits_set(GPIOD,GPIO_PINS_13);
		}
		
		
		//LED 초기화 잘 됐는지 테스트 용
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