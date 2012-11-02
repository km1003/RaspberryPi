/*
 * rangefinder.c for the URM37 ultrasonic rangefinder used with
 * the Raspberry Pi
 */
#include <stdio.h>
#include <wiringPi.h>
#include "serial.h"
#include "time.h"

#define PWMIN		(22)
#define COMPTRIG	(23)

int uart;
char autoModeCmd[] = {0x44, 0x02, 0xaa, 0xf0};

int initRangefinder()
{
  if(wiringPiSetupGpio() == -1)
  {
    printf("rangefinder: failed to init wiringPi\n");
    return -1;
  }

  // Init the uart at 9600 baud
  if((uart = serialOpen("/dev/ttyAMA0", 9600)) <= 0)
  {
    printf("rangefinder: failed to init uart\n");
    return -1;
  }

  // init the rangefinder gpios
  pinMode(PWMIN, INPUT);
  pinMode(COMPTRIG, INPUT);

  // start the rangefinder in autonomous mode
  writeSerial(autoModeCmd, 4, uart);

  // allow time for the rangefinder to start up
  delay(300);

  return 0;
}

// get the range from the pwm out
int getRange()
{
  uint64_t start=0, stop=0, pulseWidth;
  while(digitalRead(PWMIN) != 1);
  while(digitalRead(PWMIN) == 1);
  start = getuSecs();
  while(digitalRead(PWMIN) == 0);
  stop = getuSecs();
  pulseWidth = stop - start;
  return (int)pulseWidth/50;
}
