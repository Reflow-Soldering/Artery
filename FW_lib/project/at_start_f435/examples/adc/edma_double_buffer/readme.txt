/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  the edma_double_buffer mode transfer adc conversion data.
  the trigger source is software
  the convert data as follow:
  - adc1_ordinary_valuetab[0] ---> adc1_channel_4
  - adc1_ordinary_valuetab[1] ---> adc1_channel_5
  - adc1_ordinary_valuetab[2] ---> adc1_channel_6
  
  - double_adc1_ordinary_valuetab[0] ---> adc1_channel_4
  - double_adc1_ordinary_valuetab[1] ---> adc1_channel_5
  - double_adc1_ordinary_valuetab[2] ---> adc1_channel_6

  for more detailed information. please refer to the application note document AN0093.
