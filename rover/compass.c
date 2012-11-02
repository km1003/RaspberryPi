#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

float getHeading()
{
  int address = 0x1E;
  int fd;
  char buf[10];
  char* fileName = "/dev/i2c-1";
  const float PI = 3.1415927;
  if((fd = open(fileName, O_RDWR)) < 0){
    printf("error opening %s\n", fileName);
    return -1;
  }
  if(ioctl(fd, I2C_SLAVE, address) < 0){
    printf("error getting bus access to talk to slave\n");
    return -1;
  }
  buf[0] = 0x3C; // document suggested init code
  buf[1] = 0x02; //
  buf[2] = 0x00; //
  buf[3] = 0x03; // start of read registers
  int numWrote;
  if((numWrote = write(fd, &buf[1], 2)) != 2){
    printf("error writing init to compass: %d\n", numWrote);
    return -1;
  }
  int numRead;
  if(write(fd, &buf[3], 1) != 1){
    printf("error writing register select to compass\n");
    return -1;
  }
  numRead = read(fd, &buf[4], 6);
  //printf("read %d bytes from compass\n", numRead);
  //printf("%d.%d.%d.%d.%d.%d\n",
  //  buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);
  int x = (buf[4] ) | (buf[5] << 8);
  int z = (buf[6] ) | (buf[7] << 8);
  int y = (buf[8] ) | (buf[9] << 8);
  printf("X: %f, Y: %f, Z: %f\n",
    (float)x/0xffff, (float)y/0xffff, (float)z/0xffff);
  float X = (float)x/0xffff;
  float Y = (float)y/0xffff;
  float h = atan2(Y, X);

  // TODO: compensate for the Z axis and normalize
  if(h < 0)
    h += 2*PI;
  float heading = h * 180/M_PI;
  return heading;
}
