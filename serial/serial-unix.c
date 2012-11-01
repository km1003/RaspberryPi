#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

//#include <wiringSerial.h>
#include <wiringPi.h>

int main(int argc, char** argv)
{
  int fd, i, j, num;
  char* filename = "/dev/ttyAMA0";
  unsigned char cmd[4], readbuf[32];
  
  if(wiringPiSetupGpio() == -1)
  {
    printf("failed to init wiringPi\n");
    exit(1);
  }

  // Init the level shifter
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);
  delay(100);

  if((fd = open(filename, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
  {
    printf("failed to open %s\n", filename);
    exit(1);
  }

  cmd[0] = 0x11;
  cmd[1] = 0x00;
  cmd[2] = 0x00;
  cmd[3] = (cmd[0] + cmd[1] + cmd[2]) & 0xff;

  for(i = 0; i < 10; i++)
  {
    for(j = 0; j < 32; j++)
      readbuf[j] = 0;
    delay(500);
    if((num = write(fd, cmd, 4)) < 0)
    {
      printf("write error num=%d\n", num);
      continue;
    }
    printf("wrote %d bytes to %s\n", num, filename);
    for(j = 0; j < num; j++)
      printf("[%x]", cmd[j]);
    printf("\n");
    delay(500);
    num = 0;
    while((num = read(fd, readbuf, 4)) > 0)
    {
      fflush(stdout);
      printf("read %d bytes from %s\n", num, filename);
      for(j = 0; j < num; j++)
        printf("[%x]", readbuf[j]);
      printf("\n");
    }
    printf("num=%d\n", num);
  }

  if(close(fd) < 0)
    printf("failed to close %s\n", filename);

  return 0;
}
