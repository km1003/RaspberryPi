#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//#include <wiringPi.h>

int main(int argc, char **argv)
{
  int address = 0x1E;
  int fd;
  char buf[10];
  char* fileName = "/dev/i2c-1";

  if((fd = open(fileName, O_RDWR)) < 0){
    printf("error opening %s\n", fileName);
    exit(1);
  }

  if(ioctl(fd, I2C_SLAVE, address) < 0){
    printf("error getting bus access to talk to slave\n");
    exit(1);
  }

  printf("opened %s successfully, fd=%d\n", fileName, fd);

  buf[0] = 0x3C;
  buf[1] = 0x02;
  buf[2] = 0x00;
  buf[3] = 0x03;


  int numWrote;
  if((numWrote = write(fd, &buf[1], 2)) != 2){
    printf("error writing init to compass: %d\n", numWrote);
    exit(1);
  }

  int numRead;
  for(;;){
    if(write(fd, &buf[3], 1) != 1){
      printf("error writing register select to compass\n");
      exit(1);
    }
    
    numRead = read(fd, &buf[4], 6);
    printf("read %d bytes from compass\n", numRead);
    printf("%d.%d.%d.%d.%d.%d\n",
      buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);

  }

  

  return 0;
}
