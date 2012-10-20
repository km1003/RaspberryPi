#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int address = 0x63;
  int fd;
  char* fileName = "/dev/i2c-0";

  if((fd = open(fileName, O_RDWR)) < 0){
    printf("error opening %s\n", fileName);
    exit(1);
  }

  if(ioctl(fd, I2C_SLAVE, address) < 0){
    printf("error getting bus access to talk to slave\n");
    exit(1);
  }

  printf("opened %s successfully\n", fileName);

  return 0;
}
