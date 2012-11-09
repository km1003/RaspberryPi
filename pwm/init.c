#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "wiringPi.h"

int main(int argc, char** argv)
{
  //int i;

  // Setup wiringPi to use GPIO mode
  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode(18, PWM_OUTPUT); // enable pwm out
  pinMode(4, OUTPUT); // enable level shifter control
  pinMode(17, OUTPUT); // enable M1 polarity control
  pinMode(27, OUTPUT); // enable M2 polarity control
  digitalWrite(4, 0); // level shifter enable bit
  digitalWrite(17, 0); // M1 polarity bit
  digitalWrite(27, 1); // M2 polarity bit
  pwmWrite(18, 0);
  return 0;
}
