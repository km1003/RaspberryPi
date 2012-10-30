#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include <wiringSerial.h>
#include <wiringPi.h>

// t1 = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;
// gettimeofday (&tv, NULL) ;
// epoch = (tv.tv_sec * 1000000 + tv.tv_usec) / 1000 ;
#define FALSE (0)
#define TRUE (1)


uint64_t getuSecs()
{
  struct timeval tv;
  uint64_t t1;

  gettimeofday(&tv, NULL);
  t1 = (tv.tv_sec * 1000000 + tv.tv_usec);
  return t1;
}

void writeSerial(unsigned char* buf, int len, int uart)
{
  int i;
  printf("writing %d bytes to serial: ", len);
  for(i = 0; i < len; i++)
  {
    serialPutchar(uart, (buf[i])&0xff);
    printf("[%x]", (buf[i])&0xff);
  }
  printf("\n");
}

int main(int argc, char** argv)
{
  char rxChecked = FALSE;
  char c = 0;
  unsigned char sendbuf[4];
  int count, i, sum, uart;
  uint64_t pstart=0, pstop=0, sstart=0, sstop=0;

  if(wiringPiSetupGpio() == -1)
  {
    printf("failed to init wiringPi\n");
    exit(1);
  }

  // Init the level shifter
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);

  //pinMode(15, INPUT);
  //pullUpDnControl (15, PUD_UP);

  // Init the uart at 9600 baud
  if((uart = serialOpen("/dev/ttyAMA0", 9600)) <= 0)
  {
    printf("failed to init uart\n");
    exit(1);
  }
  
  //pinMode(15, INPUT);
  //pullUpDnControl (15, PUD_UP);
  //pullUpDnControl (14, PUD_UP);

  printf("uart fd: %d\n", uart);

  // Set command to read the temp from the URM37 rangefinder
  sendbuf[0] = 0x22;
  sendbuf[1] = 0x00;
  sendbuf[2] = 0x00;
  sum = sendbuf[0] + sendbuf[1] + sendbuf[2]; // calculate checksum
  sendbuf[3] = sum & 0xff;
  delay(1000);

  while(1)
  {
    // send serial every second
    if(sstop == 0 || ((getuSecs() - sstart) > 1000000))
    {
      sstart = getuSecs();
      sstop = getuSecs();
      writeSerial(sendbuf, 4, uart);
      rxChecked = FALSE;
    }

    // check for serial rx 100 msecs later
    if((getuSecs() - sstart) > 100000 && !rxChecked)
    {
      rxChecked = TRUE;
      sstop = getuSecs();
      sum = 0;
      count = 0;
      printf("[%llu] recieved:", getuSecs());
      while(serialDataAvail(uart) > 0)
      {
        c = (serialGetchar(uart))&0xff;
        count++;
        if(count < 4)
          sum += c;
        printf("[%x]", c);
      }
      printf(" sum=%x, count=%d", sum&0xff, count);
      if(sum == c && count == 4) printf(" MATCH!!");
      printf("\n");
      //serialFlush(uart);
    }
  }

  serialFlush(uart);
  serialClose(uart);

  return 0;
}
