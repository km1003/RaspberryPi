#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "rover.h"
#include "wiringPi.h"

int main(int argc, char** argv)
{
  int i, speed = 512;
  init();

  // drive in a ccw square
  for(i = 0; i < 5; i++)
  {
    driveForward(speed);
    delay(1000);
    turnLeft(speed);
    delay(250);
  }

  return 0;
}

void init()
{
  // Setup wiringPi to use GPIO mode
  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode(PWM, PWM_OUTPUT);	// enable pwm output
  pinMode(SHIFTEN, OUTPUT);	// enable level shifter control
  pinMode(M1POL, OUTPUT);	// enable M1 polarity control
  pinMode(M2POL, OUTPUT);	// enable M2 polarity control
  digitalWrite(SHIFTEN, 1);	// enable level shifter
}

void turnLeft(int speed)
{
  digitalWrite(M1POL, 1);
  digitalWrite(M2POL, 1);
  pwmWrite(PWM, speed);
}

void turnRight(int speed)
{
  digitalWrite(M1POL, 0);
  digitalWrite(M2POL, 0);
  pwmWrite(PWM, speed);
}

void driveForward(int speed)
{
  digitalWrite(M1POL, 0);
  digitalWrite(M2POL, 1);
  pwmWrite(PWM, speed);
}

void driveReverse(int speed)
{
  digitalWrite(M1POL, 1);
  digitalWrite(M2POL, 0);
  pwmWrite(PWM, speed);
}

void stop()
{
  pwmWrite(PWM, 0);
}
