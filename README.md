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

6. Timer(파일명 : **main_timer_led.c**)

    현재 작성 중입니다. 

  
   
