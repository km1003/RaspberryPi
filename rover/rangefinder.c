/*
 * rangefinder.c for the URM37 ultrasonic rangefinder used with
 * the Raspberry Pi
 * TODO: use a wiringPi interrupt thread to read the range
 */
#include <stdio.h>
#include <wiringPi.h>
#include "serial.h"
#include "time.h"

#define PWMIN		(22)
#define COMPTRIG	(23)

int uart;
char autoModeCmd[] = {0x44, 0x02, 0xaa, 0xf0};

// set the rangefinder mode and gpio pins to read
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

  return 0;
}

// get the range from the pwm out
int getRange()
{
  int range;
  uint64_t start=0, stop=0, pulseWidth;	
  do
  {
    while(digitalRead(PWMIN) == LOW);	// wait for high signal
    while(digitalRead(PWMIN) == HIGH);	// wait for falling edge
    start = getuSecs();			// start of the low pulse
    while(digitalRead(PWMIN) == LOW);	// wait for the end of the pulse
    stop = getuSecs();			// end of the low pulse
    pulseWidth = stop - start;		// length of pulse in uSecs
    range=(int)((pulseWidth/50.0)+0.5);	// every 50us of pulse = 1cm
  } while(range < 3 || range > 600);	// valid range is 1cm - 500cm
  return range;
}
