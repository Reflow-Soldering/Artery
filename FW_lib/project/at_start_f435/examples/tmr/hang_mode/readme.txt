/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to test
  hang mode with an external trigger.

  hang mode test -----
  step1: tmr1 is configured as master timer:
  - toggle mode is used

  step2: tmr1 is configured as slave timer for an external trigger connected
         to tmr1 ti2 pin (tmr1 ch2 configured as input pin):
  - the tmr1 ti2fp2 is used as trigger input
  - rising edge is used to start and stop the tmr1: hang mode.

  the timxclk is fixed to apb2_freq*2, the three timers are running at: 
  tmrx frequency = tmrx clock counter/ (timx_period + 1) = 500 khz.

  set-up
  connect the following pins to an oscilloscope to monitor the different waveforms:
  - tim1 ch1 ---> pa8

  for more detailed information. please refer to the application note document AN0085.
