/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows in halfduplex
  mode how to use polling transfer data. use the mode switch to realize spi and
  i2s communication.
  the pins connection as follow:
  - spi2/i2s2                spi3/i2s3 
    pd0(ws)        <--->     pa4 (ws)
    pd1(sck)       <--->     pc10(sck)
    pd4(sd)        <--->     pc12(sd)

  for more detailed information. please refer to the application note document AN0102.
