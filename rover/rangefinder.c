/*
 * rangefinder.c for the URM37 ultrasonic rangefinder used with
 * the Raspberry Pi
 * TODO: use a wiringPi interrupt thread to read the range
 */
#include <stdio.h>
#include <wiringPi.h>
#include "serial.h"
#include "time.h"

#define COMPTRIG	(4)

int uart;
//char autoModeCmd[] = {0x44, 0x02, 0xaa, 0xf0};

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
//  // start the rangefinder in autonomous mode
//  writeSerial(autoModeCmd, 4, uart);
//  // init the rangefinder gpios
//  pinMode(COMPTRIG, INPUT);
//  pullUpDnControl(COMPTRIG, PUD_UP);
  return 0;
}

// Use uart commands to get range
int getRange()
{
  char buf[] = {0,0,0,0,0,0,0,0,0,0,0,0};
  // Note: servo degree goes here at a later time
  char readRangeCmd[] = {0x22, 0x00, 0x00, 0x22};
  unsigned char sum;
  int count, i, range=-1;
  writeSerial(readRangeCmd, 4, uart);
  // allow some time for rangefinder to respond
  for(i=0;i<5;i++)
  {
    if(serialDataAvail(uart) > 3) break;
    delay(5);
  }
  if((serialDataAvail(uart)%4)>0) return range; // incomplete packet on uart
  // handle getting up to three responses back
  count = readSerial(buf, 12, uart);
  for(i=0;i<count;i+=4)
  {
    sum = (buf[i]+buf[i+1]+buf[i+2]);
    // ignore bad checksum or invalid returned by rangefinder
    if(sum != buf[i+3] || (buf[i+1] == 0xff) || ((buf[i+1]<<8) | buf[i+2]) < 3)
      continue;
    range = ((buf[i+1]<<8) | buf[i+2]);
  }
  return range;
}

/* Note: No longer using the pwmin pin
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
*/
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
