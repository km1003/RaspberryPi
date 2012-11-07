/*
 * rangefinder.c for the URM37 ultrasonic rangefinder used with
 * the Raspberry Pi
 * TODO: use a wiringPi interrupt thread to read the range
 */
#include <stdio.h>
#include <wiringPi.h>
#include "serial.h"
#include "time.h"

#define PWMIN           (22)
#define COMPTRIG        (23)

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
  pullUpDnControl(PWMIN, PUD_UP);
  pullUpDnControl(PWMIN, PUD_UP);

  // start the rangefinder in autonomous mode
  writeSerial(autoModeCmd, 4, uart);

  return 0;
}

// get the range from the pwm out - NOTE: Prefered mode is to read the
// comptrig pin as this function uses a hard loop and is not accurate.
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

// set the range (cm) threshold for comptrig pin in autonomous mode
// WARNING: Use this sparingly as it writes to the EEPROM
void setRange(int range)
{
  char rangeLo[] = {0x44, 0x00, 0x00, 0x00};
  char rangeHi[] = {0x44, 0x01, 0x00, 0x00};
  // ignore if not a valid range threshold
  if(range > 400 || range < 4)
    return;
  rangeLo[2] = range&0xff;
  rangeHi[2] = (range>>8)&0xff;
  rangeLo[3] = (rangeLo[0]+rangeLo[1]+rangeLo[2])&0xff;
  rangeHi[3] = (rangeHi[0]+rangeHi[1]+rangeHi[2])&0xff;
  writeSerial(rangeHi, 4, uart);
  writeSerial(rangeLo, 4, uart);
}
