#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "serial.h"

void writeSerial(unsigned char* buf, int len, int uart)
{
  int i;
  for(i = 0; i < len; i++)
    serialPutchar(uart, buf[i]);
}

int readSerial(unsigned char* buf, int len, int uart)
{
  int i;
  for(i = 0; i < len; i++)
  {
    if(serialDataAvail(uart) <= 0)
      return i;
    buf[i] = (serialGetchar(uart) & 0xff);
  }
  return i;
}
