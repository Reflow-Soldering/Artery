/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32f435_437.h"
#include "at32f435_437_clock.h"
#include "at_surf_f437_board_delay.h"
#include <stdio.h>
#include <rt_sys.h> // _sys_* 함수들을 위한 Keil MDK 헤더
#include <errno.h>  // errno 정의를 위해 필요

/*
*USART에서 UART통신 진행 USART1사용함
*baud rate 115200bps
*
*/


/*
*	1. UART init
*	2. printf GNU 문구 복붙
*	3. Printf 해보기
*
*/

// __use_no_semihosting을 사용하면, 세미호스팅 호출이 비활성화됩니다.
// 이 지시어는 그대로 유지합니다.
#if defined (__ARMCC_VERSION) // Keil MDK 컴파일러 (ARM Compiler 5 또는 6)
  #if (__ARMCC_VERSION >= 6000000) // ARM Compiler 6 (Arm Clang)
    __asm (".global __use_no_semihosting\n\t");
  #else // ARM Compiler 5 (armcc)
    #pragma import(__use_no_semihosting)
  #endif

  // _sys_exit 함수는 프로그램이 종료될 때 호출됩니다. 무한 루프를 돌게 합니다.
  void _sys_exit(int x)
  {
    (void)x; // unused parameter 경고 억제
    for(;;); // 무한 루프
  }

  // _ttywrch 함수는 printf가 내부적으로 호출할 수 있습니다.
  // fputc에서 직접 UART로 출력하므로 여기서는 단순화합니다.
  void _ttywrch(int ch)
  {
    (void)ch; // unused parameter 경고 억제
  }

  // printf를 위한 fputc 구현
  int fputc(int ch, FILE *f)
  {
    (void)f; // unused parameter 경고 억제

    while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET); // 전송 버퍼가 비어있을 때까지 대기
    usart_data_transmit(USART1, (uint16_t)ch); // 데이터 전송
    while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET); // 전송 완료 대기
    return ch;
  }

  // scanf를 위한 fgetc 구현
  int fgetc(FILE *f)
  {
    (void)f; // unused parameter 경고 억제
    uint16_t data;

    while(usart_flag_get(USART1, USART_RDBF_FLAG) == RESET); // 수신 버퍼에 데이터가 있을 때까지 대기
    data = usart_data_receive(USART1); // 데이터 수신

    // 선택 사항: 에코백 (입력받은 문자를 다시 터미널에 출력하여 사용자가 입력하는 것을 볼 수 있게 함)
    // fputc((int)data, f); // 입력받은 문자를 다시 UART로 출력
    // if (data == '\r') fputc('\n', f); // Enter 키 입력 시 새 줄로 이동 (\r\n)

    return (int)data;
  }

  // --- 세미호스팅 관련 시스템 호출 함수 오버라이드 ---
  // MicroLIB을 사용하지 않을 때, C 라이브러리가 참조하는 모든 _sys_ 함수들을 구현해야 합니다.
  // L6915E 오류를 해결하는 핵심 부분입니다.

  // _sys_open: 파일을 여는 함수 (더미 구현)
  FILEHANDLE _sys_open(const char *name, int openmode)
  {
      (void)name; (void)openmode; // unused parameter 경고 억제
      return -1; // 파일을 열 수 없음을 반환 (일반적으로 임베디드에서 파일 시스템 없음)
  }

  // _sys_close: 파일을 닫는 함수 (더미 구현)
  int _sys_close(FILEHANDLE fh)
  {
      (void)fh; // unused parameter 경고 억제
      return 0; // 성공 반환
  }

  // _sys_write: 파일에 쓰는 함수. STDOUT/STDERR은 fputc로 리다이렉트합니다.
  int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
  {
      (void)mode; // unused parameter 경고 억제
      if (fh == _LLIO_STDOUT || fh == _LLIO_STDERR) // 표준 출력 또는 표준 에러
      {
          for (unsigned i = 0; i < len; i++)
          {
              fputc(buf[i], (FILE *)fh); // fputc를 통해 UART로 출력
          }
          return len; // 성공적으로 처리된 바이트 수 반환
      }
      errno = EBADF; // 잘못된 파일 디스크립터
      return -1; // 오류
  }

  // _sys_read: 파일에서 읽는 함수. STDIN은 fgetc로 리다이렉트합니다.
  int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
  {
      (void)mode; // unused parameter 경고 억제
      if (fh == _LLIO_STDIN) // 표준 입력
      {
          for (unsigned i = 0; i < len; i++)
          {
              buf[i] = (unsigned char)fgetc((FILE *)fh); // fgetc를 통해 UART에서 입력
              // Enter 키 입력 처리 (scanf는 보통 줄 단위 입력을 기대)
              if (buf[i] == '\r' || buf[i] == '\n') {
                  return i + 1; // 입력된 문자 수 반환 (CR 또는 LF 포함)
              }
          }
          return len; // 요청된 길이만큼 읽음
      }
      errno = EBADF; // 잘못된 파일 디스크립터
      return -1; // 오류
  }

  // _sys_seek: 파일 포인터를 이동시키는 함수 (더미 구현)
  int _sys_seek(FILEHANDLE fh, long offset)
  {
      (void)fh; (void)offset; // unused parameter 경고 억제
      return -1; // 지원하지 않음
  }

  // _sys_istty: 파일 핸들이 터미널인지 확인하는 함수
  int _sys_istty(FILEHANDLE fh)
  {
      // 표준 입출력 (stdin, stdout, stderr)은 터미널로 간주합니다.
      if (fh == _LLIO_STDIN || fh == _LLIO_STDOUT || fh == _LLIO_STDERR)
      {
          return 1; // 터미널임
      }
      return 0; // 터미널 아님
  }

  // _sys_flen: 파일 길이 확인 함수 (더미 구현)
  long _sys_flen(FILEHANDLE fh)
  {
      (void)fh; // unused parameter 경고 억제
      return 0; // 길이 0
  }

  // _sys_tmpnam: 임시 파일 이름 생성 함수 (더미 구현)
  char *_sys_tmpnam(char *name, int sig, unsigned maxlen)
  {
      (void)name; (void)sig; (void)maxlen; // unused parameter 경고 억제
      return NULL;
  }

  // _sys_command_string: 명령 문자열 처리 함수 (더미 구현)
  char *_sys_command_string(char *cmd, int len)
  {
      (void)cmd; (void)len; // unused parameter 경고 억제
      return NULL;
  }

  // _sys_exit_extended: 확장된 종료 함수 (더미 구현)
  void _sys_exit_extended(int return_code, int sub_code)
  {
      (void)return_code; (void)sub_code; // unused parameter 경고 억제
      for(;;); // 무한 루프
  }

  // _sys_getenv: 환경 변수 가져오기 함수 (더미 구현)
  char *_sys_getenv(const char *name)
  {
      (void)name; // unused parameter 경고 억제
      return NULL;
  }

  // _sys_ensure: 출력 버퍼를 플러시하는 함수 (더미 구현)
  int _sys_ensure(FILEHANDLE fh)
  {
      (void)fh; // unused parameter 경고 억제
      return 0;
  }

#endif // __ARMCC_VERSION
///* suport printf function, usemicrolib is unnecessary */
//#if (__ARMCC_VERSION > 6000000)
//  __asm (".global __use_no_semihosting\n\t");
//  void _sys_exit(int x)
//  {
//    x = x;
//  }
//  /* __use_no_semihosting was requested, but _ttywrch was */
//  void _ttywrch(int ch)
//  {
//    ch = ch;
//  }
//  FILE __stdout;
//#else
// #ifdef __CC_ARM
//  #pragma import(__use_no_semihosting)
//  struct __FILE
//  {
//    int handle;
//  };
//  FILE __stdout;
//  void _sys_exit(int x)
//  {
//    x = x;
//  }
//  /* __use_no_semihosting was requested, but _ttywrch was */
//  void _ttywrch(int ch)
//  {
//    ch = ch;
//  }
// #endif
//#endif

//#if defined ( __GNUC__ ) && !defined (__clang__)
//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif
//	
//	
///**
//  * @brief  retargets the c library printf function to the usart.
//  * @param  none
//  * @retval none
//  */
//PUTCHAR_PROTOTYPE
//{
//  while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
//  usart_data_transmit(USART1, (uint16_t)ch);
//  while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET);
//  return ch;
//}

//#if (defined (__GNUC__) && !defined (__clang__)) || (defined (__ICCARM__))
//#if defined (__GNUC__) && !defined (__clang__)
//int _write(int fd, char *pbuffer, int size)
//#elif defined ( __ICCARM__ )
//#pragma module_name = "?__write"
//int __write(int fd, char *pbuffer, int size)
//#endif
//{
//  for(int i = 0; i < size; i ++)
//  {
//    while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
//    usart_data_transmit(USART1, (uint16_t)(*pbuffer++));
//    while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET);
//  }

//  return size;
//}
//#endif


__IO uint32_t time_cnt = 0;


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();
	// Uart Init
	crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK,TRUE);
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
	
	delay_init();
	#if defined (__GNUC__) && !defined (__clang__)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif

	//TX init
	gpio_init_type io_usart1 = 
	{
		.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE,
		.gpio_mode = GPIO_MODE_MUX,
		.gpio_out_type = GPIO_OUTPUT_PUSH_PULL,
		.gpio_pins = GPIO_PINS_9|GPIO_PINS_10,
		.gpio_pull = GPIO_PULL_NONE,
	};
	gpio_init(GPIOA, &io_usart1);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE9, GPIO_MUX_7);
	gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE10, GPIO_MUX_7);
	
	
	usart_init(USART1,115200, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(USART1, TRUE);
	usart_receiver_enable(USART1, TRUE); 
	usart_enable(USART1, TRUE);

  /* output a message on hyperterminal using printf function */
  printf("usart printf example: retarget the c library printf function to the usart\r\n");

  while(1)
  {
		printf("Enter Number : ");
    printf("usart printf counter: %u\r\n",time_cnt++);
    delay_sec(1);
  }
}

/**
  * @}
  */

/**
  * @}
  */
