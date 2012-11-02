#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#include "compass.h"
#include "rangefinder.h"
#include "serial.h"

// Defines
#define FALSE   (0)
#define TRUE    (1)

// Broadcom GPIO map
#define SHIFTEN (4)
#define TXD     (14)
#define RXD     (15)
#define M1POL   (17)
#define PWM     (18)
#define M2POL   (27)

// Type definitions
typedef char    bool;

// Function prototypes
void init(void);
void turnLeft(int speed);
void turnRight(int speed);
void driveForward(int speed);
void driveReverse(int speed);
void stop(void);

int main(int argc, char** argv)
{
  int i;
  init();

  while(1)
  {
    printf("range: %dcm\n", getRange());
    delay(100);
  }

/* TODO: work on sensor code
  // drive in a ccw square
  for(i = 0; i < 10; i++)
  {
    driveForward(512);
    delay(1000);
    turnLeft(512);
    delay(300);
  }
  
  stop();
*/



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
  pwmWrite(PWM, 0);		// zero pwm output
  if(initRangefinder() < 0)
    printf("failed to init rangefinder\n");
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
