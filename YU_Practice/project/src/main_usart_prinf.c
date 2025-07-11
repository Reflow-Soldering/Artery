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
#include <rt_sys.h> // _sys_* �Լ����� ���� Keil MDK ���
#include <errno.h>  // errno ���Ǹ� ���� �ʿ�

/*
*USART���� UART��� ���� USART1�����
*baud rate 115200bps
*
*/


/*
*	1. UART init
*	2. printf GNU ���� ����
*	3. Printf �غ���
*
*/

// __use_no_semihosting�� ����ϸ�, ����ȣ���� ȣ���� ��Ȱ��ȭ�˴ϴ�.
// �� ���þ�� �״�� �����մϴ�.
#if defined (__ARMCC_VERSION) // Keil MDK �����Ϸ� (ARM Compiler 5 �Ǵ� 6)
  #if (__ARMCC_VERSION >= 6000000) // ARM Compiler 6 (Arm Clang)
    __asm (".global __use_no_semihosting\n\t");
  #else // ARM Compiler 5 (armcc)
    #pragma import(__use_no_semihosting)
  #endif

  // _sys_exit �Լ��� ���α׷��� ����� �� ȣ��˴ϴ�. ���� ������ ���� �մϴ�.
  void _sys_exit(int x)
  {
    (void)x; // unused parameter ��� ����
    for(;;); // ���� ����
  }

  // _ttywrch �Լ��� printf�� ���������� ȣ���� �� �ֽ��ϴ�.
  // fputc���� ���� UART�� ����ϹǷ� ���⼭�� �ܼ�ȭ�մϴ�.
  void _ttywrch(int ch)
  {
    (void)ch; // unused parameter ��� ����
  }

  // printf�� ���� fputc ����
  int fputc(int ch, FILE *f)
  {
    (void)f; // unused parameter ��� ����

    while(usart_flag_get(USART1, USART_TDBE_FLAG) == RESET); // ���� ���۰� ������� ������ ���
    usart_data_transmit(USART1, (uint16_t)ch); // ������ ����
    while(usart_flag_get(USART1, USART_TDC_FLAG) == RESET); // ���� �Ϸ� ���
    return ch;
  }

  // scanf�� ���� fgetc ����
  int fgetc(FILE *f)
  {
    (void)f; // unused parameter ��� ����
    uint16_t data;

    while(usart_flag_get(USART1, USART_RDBF_FLAG) == RESET); // ���� ���ۿ� �����Ͱ� ���� ������ ���
    data = usart_data_receive(USART1); // ������ ����

    // ���� ����: ���ڹ� (�Է¹��� ���ڸ� �ٽ� �͹̳ο� ����Ͽ� ����ڰ� �Է��ϴ� ���� �� �� �ְ� ��)
    // fputc((int)data, f); // �Է¹��� ���ڸ� �ٽ� UART�� ���
    // if (data == '\r') fputc('\n', f); // Enter Ű �Է� �� �� �ٷ� �̵� (\r\n)

    return (int)data;
  }

  // --- ����ȣ���� ���� �ý��� ȣ�� �Լ� �������̵� ---
  // MicroLIB�� ������� ���� ��, C ���̺귯���� �����ϴ� ��� _sys_ �Լ����� �����ؾ� �մϴ�.
  // L6915E ������ �ذ��ϴ� �ٽ� �κ��Դϴ�.

  // _sys_open: ������ ���� �Լ� (���� ����)
  FILEHANDLE _sys_open(const char *name, int openmode)
  {
      (void)name; (void)openmode; // unused parameter ��� ����
      return -1; // ������ �� �� ������ ��ȯ (�Ϲ������� �Ӻ���忡�� ���� �ý��� ����)
  }

  // _sys_close: ������ �ݴ� �Լ� (���� ����)
  int _sys_close(FILEHANDLE fh)
  {
      (void)fh; // unused parameter ��� ����
      return 0; // ���� ��ȯ
  }

  // _sys_write: ���Ͽ� ���� �Լ�. STDOUT/STDERR�� fputc�� �����̷�Ʈ�մϴ�.
  int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
  {
      (void)mode; // unused parameter ��� ����
      if (fh == _LLIO_STDOUT || fh == _LLIO_STDERR) // ǥ�� ��� �Ǵ� ǥ�� ����
      {
          for (unsigned i = 0; i < len; i++)
          {
              fputc(buf[i], (FILE *)fh); // fputc�� ���� UART�� ���
          }
          return len; // ���������� ó���� ����Ʈ �� ��ȯ
      }
      errno = EBADF; // �߸��� ���� ��ũ����
      return -1; // ����
  }

  // _sys_read: ���Ͽ��� �д� �Լ�. STDIN�� fgetc�� �����̷�Ʈ�մϴ�.
  int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
  {
      (void)mode; // unused parameter ��� ����
      if (fh == _LLIO_STDIN) // ǥ�� �Է�
      {
          for (unsigned i = 0; i < len; i++)
          {
              buf[i] = (unsigned char)fgetc((FILE *)fh); // fgetc�� ���� UART���� �Է�
              // Enter Ű �Է� ó�� (scanf�� ���� �� ���� �Է��� ���)
              if (buf[i] == '\r' || buf[i] == '\n') {
                  return i + 1; // �Էµ� ���� �� ��ȯ (CR �Ǵ� LF ����)
              }
          }
          return len; // ��û�� ���̸�ŭ ����
      }
      errno = EBADF; // �߸��� ���� ��ũ����
      return -1; // ����
  }

  // _sys_seek: ���� �����͸� �̵���Ű�� �Լ� (���� ����)
  int _sys_seek(FILEHANDLE fh, long offset)
  {
      (void)fh; (void)offset; // unused parameter ��� ����
      return -1; // �������� ����
  }

  // _sys_istty: ���� �ڵ��� �͹̳����� Ȯ���ϴ� �Լ�
  int _sys_istty(FILEHANDLE fh)
  {
      // ǥ�� ����� (stdin, stdout, stderr)�� �͹̳η� �����մϴ�.
      if (fh == _LLIO_STDIN || fh == _LLIO_STDOUT || fh == _LLIO_STDERR)
      {
          return 1; // �͹̳���
      }
      return 0; // �͹̳� �ƴ�
  }

  // _sys_flen: ���� ���� Ȯ�� �Լ� (���� ����)
  long _sys_flen(FILEHANDLE fh)
  {
      (void)fh; // unused parameter ��� ����
      return 0; // ���� 0
  }

  // _sys_tmpnam: �ӽ� ���� �̸� ���� �Լ� (���� ����)
  char *_sys_tmpnam(char *name, int sig, unsigned maxlen)
  {
      (void)name; (void)sig; (void)maxlen; // unused parameter ��� ����
      return NULL;
  }

  // _sys_command_string: ��� ���ڿ� ó�� �Լ� (���� ����)
  char *_sys_command_string(char *cmd, int len)
  {
      (void)cmd; (void)len; // unused parameter ��� ����
      return NULL;
  }

  // _sys_exit_extended: Ȯ��� ���� �Լ� (���� ����)
  void _sys_exit_extended(int return_code, int sub_code)
  {
      (void)return_code; (void)sub_code; // unused parameter ��� ����
      for(;;); // ���� ����
  }

  // _sys_getenv: ȯ�� ���� �������� �Լ� (���� ����)
  char *_sys_getenv(const char *name)
  {
      (void)name; // unused parameter ��� ����
      return NULL;
  }

  // _sys_ensure: ��� ���۸� �÷����ϴ� �Լ� (���� ����)
  int _sys_ensure(FILEHANDLE fh)
  {
      (void)fh; // unused parameter ��� ����
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
