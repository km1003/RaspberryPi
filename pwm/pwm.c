#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "wiringPi.h"

int main(int argc, char** argv)
{
  int i;

  // Setup wiringPi to use GPIO mode
  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode(18, PWM_OUTPUT);

  //TODO: Enable level shifter
  //      Set motor polarity pins

  for(;;)
  {
    // Speed up
    for(i = 0; i < 1024; i++)
    {
      pwmWrite(18, i);
      delay(10);
    }

    // Slow down
    for(i = 1023; i >= 0; i--)
    {
      pwmWrite(18, i);
      delay(10);
    }

  }

  return 0;
}
