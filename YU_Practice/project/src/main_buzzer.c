/**
  **************************************************************************
  * @file     Buzzer.c
  * @brief    Buzzer main program
  **************************************************************************
  *                       This is Buzzer beep operation program
  *
  * This is modified from Artery BSP example the buzz
	* Artery BSP example used I2C DMA.
	* But I used without DMA and Interrupt.
	* For not use --board.h, I wrote again GPIO, I2C, Buzz code.
	* The referencese are AT-SURF-437 Circuit and PCA9555 Datasheet of NXT
	* I am a beginner! I made it, so you can made it too.
  * Reference code is buzz_main.c in AT_SURF_MAIN
  **************************************************************************
  */

#include "at32f435_437.h"
#include "at32f435_437_clock.h"
#include "at_surf_f437_board_delay.h"

//Use I2c_Applicaition.h
#include "i2c_application.h"


/*Not use Surf Board BSP*/
//#include "at_surf_f437_board_buzz.h"
//#include "at_surf_f437_board_lcd.h"


#define BEEP		0x0010
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

	/*Yu Buzz Init for buzz i2c*/
	
	//i2c handle 
	i2c_handle_type hi2c;
	
	//gpio handle
	gpio_init_type gpio_init_struct;
	
	hi2c.i2cx = I2C2;
	i2c_reset(hi2c.i2cx);
	
	/*I2C Low Level INIT From YU*/
	//I2C Bus Enable
	crm_periph_clock_enable(CRM_I2C2_PERIPH_CLOCK, TRUE);
	//GPIOH Bus Enable
	crm_periph_clock_enable(CRM_GPIOH_PERIPH_CLOCK, TRUE);

	
	/*gpio pin mux(altenate) config*/
	/***************************************************/
	gpio_pin_mux_config(GPIOH,GPIO_PINS_SOURCE2,GPIO_MUX_4);
	gpio_pin_mux_config(GPIOH,GPIO_PINS_SOURCE3,GPIO_MUX_4);
	
	/* i2c scl pin config */
	/*I2C GPIO 설정
	*실제로는 SCL만 썼으나, PIN 조합으로 SDA를 동시에 config함*/
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	
	gpio_init_struct.gpio_pins = GPIO_PINS_2|GPIO_PINS_3;
	
	gpio_init(GPIOH, &gpio_init_struct);
	
	/* i2c sda pin config */
	/*
	gpio_init_struct.gpio_pins = GPIO_PINS_3;
  gpio_init(GPIOH, &gpio_init_struct);
	*/
	
	/* i2c initialize */
	/* I2C 초기화 진행*/
	i2c_init(hi2c.i2cx, 0x0F, 0x10F03D6B);		//i2c2, filter digit 15, 400k speed
	
	/*****************************************************/
	/*Low Level Init complete*/
	i2c_enable(hi2c.i2cx,TRUE);
	
	


	/*****************************************************/
	/* PAC9555 Output config*/
	/* 
	* PCA9555.h 사용없이 진행, 초기화 용도
	* io_config를 FFFF로 두는 이유는 예시에서 PCA9555를 읽어오는 것부터 시작하는데
	* 이는 초기화 값이 모두 HIgh로 되어있어서이다.
	* 해당 상태에서 값을 읽어온 후, 포트0를 아웃풋으로 설정하고, 값을 넣는 용도로 쓴 것이다.
	* 나는 read를 쓰지 않으므로, io포트를 초기화 하기 위해 해당 변수를 생성, FFFF로 한 것이다.
	*/
	
	i2c_status_type i2c2_status;
	uint16_t io_config = 0xFFFF;
	io_config &= ~BEEP;
	
	i2c2_status = i2c_memory_write(&hi2c, 0x01, 0x40, 0x06, (uint8_t *)&io_config, 2, 0xffffff);
	if(i2c2_status == I2C_OK)
	{
		
	}
	
	/*******************************************************/
	

  /* display information */
  //lcd_string_show(10, 20, 200, 24, 24, (uint8_t *)"Buzz Test");

  while(1)
  {
		io_config = ~(io_config);
		i2c2_status = i2c_memory_write(&hi2c, 0x01, 0x40, 0x02, (uint8_t *)&io_config, 2, 0xffffff);
		
		if(i2c2_status == I2C_OK)
		{
			
		}
			
		delay_ms(100);
		
		io_config = ~(io_config);
		
		if((i2c_memory_write(&hi2c, 0x01, 0x40, 0x02, (uint8_t *)&io_config, 2, 0xffffff)) != I2C_OK)

		{
			return 0;
		}
		
		delay_ms(1000);

  }
}

