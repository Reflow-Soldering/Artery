/**
 **************************************************************************
 * @file    main_gpio_int.c
 * @brief   main program
 **************************************************************************
 * This is button interrupt operation program
 *
 * This is modified from Artery BSP example the key
 * When press key1 or 2, RGB LED is on
 * I am a beginner! I made it, so you can made it too.
 * Reference code is buzz_main.c in AT_SURF_MAIN
 **************************************************************************
 */


#include "at32f435_437.h"
#include "at32f435_437_clock.h"
#include "at_surf_f437_board_delay.h"

/*
* GPIOB0�� TMR3 CH3���
* ������ tmr3�� �־� ��������� ������
*
*/

/*
* 1. PB0 -> IO Mux Timer 3 Channel 3
* 2. Timter �ʱ�ȭ Delay�Լ��� 10ms�� �����Ͽ� �׽�Ʈ�غ���
* 3. �������� ��ī��Ʈ�� ������ �̹����� �ٿ� ī��Ʈ�� �غ���
*/


//make value for tmr pwm
tmr_output_config_type tmr_output_clock_init_structure;

uint16_t ScaleValue[] = {1910,1702,1516,1431,1276,1135,1012,955};
uint8_t Scale_Rising = 0;

/**
 * @brief  main program
 * @param  none
 * @retval none
 */

int main(void)
{

		/* initial system clock */
		system_clock_config();    //288MHz


		/* initialize delay */
		delay_init();

		/* initialize key */
    //1. �ʱ�ȭ
    //LED Blue �� TMR3����

    //����ü ����
    gpio_init_type gpio_init_struct;


    //�������� - TMR3, PORTB -> GPIOB 0�� Timer3���
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);    //GPIOB
    crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK,TRUE);    //Timer 3 Bus

    //GPIO�ʱ�ȭ
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pins = GPIO_PINS_0;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

    gpio_init(GPIOB, &gpio_init_struct);


    //GPIO MUX ����   -> TMR3 Ch2��� MUX 3��
    //GPIOB ��Ʈ�� 0����, MUX 2��
    gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE0, GPIO_MUX_2);

		

    //TMR3 CH3(PB0) configuration
    //timer3 �ʱ�ȭ
    // tmr_base_init///
    tmr_base_init(TMR3,ScaleValue[Scale_Rising], 287 ); //(Ÿ�̸�, �Ǹ����, ���������Ϸ� ������)


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
    tmr_output_clock_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
    //Output(TMR3 CH2) Enable
    tmr_output_clock_init_structure.oc_output_state = TRUE;

    //configuration function TMR3, CH3
    tmr_output_channel_config(TMR3,TMR_SELECT_CHANNEL_3,&tmr_output_clock_init_structure);
		

    //count value set
    tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, ScaleValue[Scale_Rising]/2);
	//	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_2, 0x4000);

    //output auto reload
    tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_3, TRUE);
		//tmr_output_channel_buffer_enable(TMR3, TMR_SELECT_CHANNEL_2, TRUE);

    //tmr3 buffer enable
    tmr_period_buffer_enable(TMR3,TRUE);

		//Use Counter enable
    tmr_counter_enable(TMR3,TRUE);

   


    while(1)
  {
		tmr_counter_enable(TMR3, TRUE);
		delay_ms(1000);
		tmr_counter_enable(TMR3, FALSE);
		Scale_Rising++;
		if(Scale_Rising == 8)
		{
			while(1)
			{
			}
		}
		tmr_period_value_set(TMR3,ScaleValue[Scale_Rising]);
		tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_3, ScaleValue[Scale_Rising]/2);
		delay_ms(500);

  }
}