/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  halfduplex mode transfer data by interrupt mode.
  the pins connection as follow:
  - spi2 slaver              spi3 master
    pd0(cs)        <--->     pa4 (cs)
    pd1(sck)       <--->     pc10(sck)
    pc2(miso)      <--->     pc12(mosi)

  for more detailed information. please refer to the application note document AN0102.

