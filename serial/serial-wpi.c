#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <wiringSerial.h>
#include <wiringPi.h>

void writeSerial(unsigned char* buf, int len, int uart)
{
  int i;
  printf("writing %d bytes to serial: ", len);
  for(i = 0; i < len; i++)
  {
    serialPutchar(uart, buf[i]);
    printf("[%x]", buf[i]);
  }
  printf("\n");
}

int main(int argc, char** argv)
{
  unsigned char sendbuf[4];
  unsigned char buf[32];
  int i, j, num, sum, uart;

  if(wiringPiSetupGpio() == -1)
  {
    printf("failed to init wiringPi\n");
    exit(1);
  }

  // Init the level shifter
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);

  // Init the uart at 9600 baud
  if((uart = serialOpen("/dev/ttyAMA0", 9600)) <= 0)
  {
    printf("failed to init uart\n");
    exit(1);
  }

  printf("uart fd: %d\n", uart);

  // Set command to read the temp from the URM37 rangefinder
  sendbuf[0] = 0x11;
  sendbuf[1] = 0x00;
  sendbuf[2] = 0x00;
  sum = sendbuf[0] + sendbuf[1] + sendbuf[2]; // calculate checksum
  sendbuf[3] = sum & 0xff;

  for(i = 0; i < 10; i++)
  {
    writeSerial(sendbuf, 4, uart);
    delay(1000);
    printf("recieved:");
    while(serialDataAvail(uart) > 0)
      printf("[%x]", serialGetchar(uart));
    printf("\n");
    serialFlush(uart);
  }

  serialFlush(uart);
  serialClose(uart);

  return 0;
}
