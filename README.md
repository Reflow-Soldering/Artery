<div align = "center">
  
# ARTERY Microcontroller 실습
## Board : AT-SURF-437

</div>

*  해당 자료는 Artery MCU 사의 Cortex-M4를 사용하여 만든 AT-SURF-437이라는 Evaluation Board를 이용하여 만들었습니다.

* 또한, 해당 자료는 **AT-START-437**의 예제와 **AT-SURF-437**의 예제를 이용하여 만들었습니다.

* START보드는 아래 그림과 같이 Arduino Mega와 유사한 형태로 이루어져 있습니다. (추후 그림을 올리겠습니다)

* SURF 보드는 아래 그림과 같이 EVB를 띄우고 있으며, 여타 다른 제작사의 Evaluation Board와 유사합니다 (추후 그림을 올리도록 하겠습니다)
  * 현재 **AT-SURF-437**로 코드를 짜고 있습니다.

* 현재 목표는 예시의 동작을 확인 후 BSP로 제공된 xxxx_board.c 파일을 쓰지 않고 Artery에서 제공하는 driver 파일만 이용하여 구동하는 것입니다.

***
현재까지 만들어진 목록 - 함수화는 진행하지 않았음
***
***
***
1. General Purpose Input Output(파일명 : **main_gpio_poll.c**)

    RGB LED와 KEY(Button) polling 방식으로 적용하였습니다.

2. Interuupt(파일명 : **main_gpio_int.c**)

    RGB LED의 설정은 1과 같으며, KEY(Button)만 인터럽트 방식으로 설정했습니다.

    **코딩을 너무 안해서 그런지 단순히 Interrupt 설정만 해주어도 크게 문제 없이 작동합니다.**

3. Buzzer(파일명 : **main_buzzer.c**)

   Buzzer가 1초마다 울리고 끊기는 루틴으로 설정되었$습니다.

    **주의 사항**
    * 현재 해당 항목은 AT-SURF-437의 I/O 부족으로 인한 것인지 I2C가 선행되어야 합니다.
    * I2C는 기존 예제에서 DMA로 되어있었으나, polling으로 변경 후 write로 치환 되었습니다.

4. Systick(파일명 : **main_systick.c**)

   가장 먼저는 아니지만 최대한 빨리 했어야 할 systick입니다.

   **AT-START-437**에서 예제를 검토 후 **AT-SURF-437**에 적용 하였습니다.

   빨강 LED를 200ms마다 깜빡일 수 있도록 하였습니다.

   레지스터 관련 내용은 주석으로 달았습니다. main문에서 확인하시길 바랍니다.

5. Timer Polling(파일명 : **main_timer_led.c**)
6. Timer Interrupt(파일명 : **main_timer_int_led.c**)
   
   1초마다 빨강 LED가 On/Off되는 파일입니다. 완전히 1초가 되지 않습니다.

   **288MHz로 계산기를 돌려서 진행했습니다. 엑셀파일 참조 바랍니다**

   **6번**은 Overflow Interrupt Flag를 tmr_flag_get(TMR1, TMR_OVF_FLAG) 함수를 이용하여 확인합니다.

   Set이 되었으면 GPIO 출력값을 읽어 On/Off 합니다.

   **7번**은 Overflow Interrupt Flag가 직접 인터럽트로 동작하여 TMR1 Interrupt 함수로 넘어갑니다.

   Set이 되었으면 내부적으로 TMR1_OVF_TMR10_IRQHandler로 넘어가서 Timer관련 Flag 변수와 Overflow Flag를 초기화합니다.

   main에서는 Flag 변수를 확인하여 초기화 하고, GPIO상태를 확인하여 On/Off합니다.

   솔직히 TMR1하고 TMR10이라고 왜 써져있는지 모르겠습니다. 같이 쓰는거려나...

7. Timer PWM LED(파일명 : **main_tmr_pwm_led.c**)

   PWM을 이용한 밝기 제어입니다.
   
   RGB중 Blue만 사용했습니다. Timer3의 2번 채널로 출력을 사용하여 제어했습니다.

   자세한 사항은 Reference Manual의 Timer항목 참고 바랍니다.

9. Timer PWM Scale(파일명 : **main_tmr_pwm_scale_sound.c**)

   PWM을 이용한 음계 설정입니다. GPIOB의 0을 Timer3의 3번 채널로 사용하여 출력했습니다 -> 2번 채널은 LED이기 때문에!

   주파수는 계산기에서 참고하고 배열처리 했습니다.

   도-레-미-파-솔-라-시-도 순으로 1초가 울렸다가 0.5초 멈추고 완전히 끝나면 while문에서 무한으로 루프를 돌렸으니

   테스트용으로만 사용 바랍니다. 저는 인트로 용으로 사용할겁니다.

   **가장 중요한 것은 구동할 부저가 무조건 수동 부저(내부 회로 없는 부저)여야 합니다.**

   **능동부저(내부 회로 내장)의 경우 신호만 연결하면 지정된 소리가 나기 때문입니다.**

11. USART Printf(파일명 : **main_usart_printf.c**)
    정확하게는 USART기능에서 UART(비동기 통신)한 내용입니다.

    printf만 있는 이유는 scanf함수를 구동할 경우 입력될 때까지 무한정 대기이기 때문에,

    다른 함수를 실행할 수 없습니다.

    또한 반드시 printf를 쓰기 위해서 Artery MCU의 HAL 함수를 retartget하여야 합니다.
