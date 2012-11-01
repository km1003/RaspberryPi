#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "serial.h"

void writeSerial(unsigned char* buf, int len, int uart)
{
  int i;
#ifdef DEBUG
  printf("writing %d bytes to serial: ", len);
#endif
  for(i = 0; i < len; i++)
  {
    serialPutchar(uart, (buf[i])&0xff);
#ifdef DEBUG
    printf("[%x]", (buf[i])&0xff);
#endif
  }
#ifdef DEBUG
  printf("\n");
#endif
}

/*
 * Reads up to len from serial into buf.
 * Returns: the number of bytes read.
 */
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
