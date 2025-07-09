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
*  Blue LED��� PB5 -> IO Mux Timer 3 Channel 2
*  Red LED �� �� ������ ������ ä�� �����Ͽ� ����ϹǷ� ������
*  ����, ������ tmr3�� �־� ��������� ������
*  
*/

/*
*  1. Blue LED��� PB5 -> IO Mux Timer 3 Channel 2
*  2. Timter �ʱ�ȭ Delay�Լ��� 10ms�� �����Ͽ� �׽�Ʈ�غ���
*  3. �������� ��ī��Ʈ�� ������ �̹����� �ٿ� ī��Ʈ�� �غ���
*/



//make value for tmr pwm
tmr_output_config_type tmr_output_clock_init_structure;
int32_t Current_PWM_Value = 0;
uint32_t PWM_Period = 64;
/**
  * @brief  main program
  * @param  none
  * @retval none
  */

int main(void)
{

  /* initial system clock */
  system_clock_config(); //288MHz

  /* initialize delay */
  delay_init();

  /* initialize key */
//1. �ʱ�ȭ
//LED Red �� TMR3����
//����ü ����
gpio_init_type gpio_init_struct;

//�������� - TMR3, PORTB -> LED Blue�� TMR3 Ch2���
crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE); //LED RED Bus
crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK,TRUE); //Timer 1 Bus

//LED �ʱ�ȭ Blue
gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
gpio_init_struct.gpio_pins = (GPIO_PINS_5);
gpio_init_struct.gpio_pull = GPIO_PULL_NONE;


gpio_init(GPIOB, &gpio_init_struct);



//GPIO MUX ����  -> LED Blue�� ��� MUX2��
//GPIOB ��Ʈ�� 5����, MUX 2��
gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE5, GPIO_MUX_2);


//TMR3 CH2 configuration
//timer3 �ʱ�ȭ
//5Hz
tmr_base_init(TMR3, PWM_Period, 0x200); //288MHz


//Downcount Mode, TMR3
tmr_cnt_dir_set(TMR3,TMR_COUNT_DOWN);

//TMR3 no division
tmr_clock_source_div_set(TMR3, TMR_CLOCK_DIV1);


//TMR3 init
//reset to default
tmr_output_default_para_init(&tmr_output_clock_init_structure);

//TMR3 init start
//Mode A Selected
tmr_output_clock_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;

//No Idle status
tmr_output_clock_init_structure.oc_idle_state = FALSE;
//Start Polarity HIgh
tmr_output_clock_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
//Output(TMR3 CH2) Enable
tmr_output_clock_init_structure.oc_output_state = TRUE;


//configuration function TMR3, CH2
tmr_output_channel_config(TMR3,TMR_SELECT_CHANNEL_2,&tmr_output_clock_init_structure);

//count value set
tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, Current_PWM_Value);

//output auto reload
tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_2, TRUE);

//tmr3 buffer enable
tmr_period_buffer_enable(TMR3,TRUE);


//Use Counter enable
tmr_counter_enable(TMR3,TRUE);

int direction = 1;

while(1)
{
	Current_PWM_Value += direction * 2; // Adjust '5' to change fading speed/smoothness



	// Check boundaries and reverse fading direction

	if (Current_PWM_Value >= PWM_Period) { // **LED is dimmest (or off) when CCR == ARR**
      Current_PWM_Value = PWM_Period; // Cap at max value
      direction = -1;                 // Change to brighten (decrease CCR)
    } else if (Current_PWM_Value <= 0) { // **LED is brightest when CCR == 0**
      Current_PWM_Value = 0;          // Cap at min value
      direction = 1;                  // Change to dim (increase CCR)
    }

    // Update the TMR3 Channel 2 Compare Register with the new duty cycle value
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, Current_PWM_Value);

    // Delay to control the overall fading speed
    delay_ms(50); // Adjust '50' for desired fade speed
  }

}
