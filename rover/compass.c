#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PI 		3.1415926535897932384626433832795
#define TRUE		(1)
#define FALSE		(0)
typedef char		bool;

bool initialized 	= FALSE;
char buf[] = {0x03C, 0x02, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char *fileName 		= "/dev/i2c-1"; // I2C device
int address 		= 0x1E; // HMC5883L compass I2C address
int i2cfd 		= -1; // I2C linux file descriptor;

// Returns the heading from the compass
float getHeading()
{
  // the i2c file descriptor hasn't been initialized
  if(i2cfd < 0)
  {
    if((i2cfd = open(fileName, O_RDWR)) < 0){
      printf("error opening %s\n", fileName);
      return -1;
    }
  }
  if(ioctl(i2cfd, I2C_SLAVE, address) < 0){
    printf("error getting bus access to talk to slave\n");
    return -1;
  }
  // first time initialization of the compass mode is done automaticallly
  if(!initialized)
  {
    if(write(i2cfd, &buf[1], 2) != 2){
      printf("error writing init to compass\n");
      return -1;
    }
    initialized = TRUE;
  }
  // select register 3 to read from
  if(write(i2cfd, &buf[3], 1) != 1){
    printf("error writing register select to compass\n");
    return -1;
  }  
  if(read(i2cfd, &buf[4], 6) != 6)
    printf("\nFailed to read 6 bytes from compass\n");
  short x = (buf[4] << 8) | buf[5];
  short z = (buf[6] << 8) | buf[7];
  short y = (buf[8] << 8) | buf[9];
  float h = atan2((float)y, (float)x); // heading in radians
  if(h < 0)
    h += 2*PI;
  float heading = h * 180/PI;
  return heading;
}
