#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#include "compass.h"
#include "rangefinder.h"
#include "serial.h"
#include "time.h"

// Defines
#define FALSE   	(0)
#define TRUE    	(1)

// Broadcom GPIO map
#define SHIFTEN 	(4)
#define TXD     	(14)
#define RXD     	(15)
#define M1POL   	(17)
#define PWM     	(18)
#define PWMIN   	(22)
#define COMPTRIG	(23)
#define M2POL   	(27)

// Enumerations
typedef enum
{
  NONE		= 0,
  FORWARD	= 1,
  REVERSE	= 2,
  LEFT		= 3,
  RIGHT		= 4,
} Direction;

// Type definitions
typedef char    bool;
typedef struct
{
  bool		isDriving;
  Direction	direction;
  Direction	lastTurn;
  int		speed;
  bool		proximityAlert;
} Rover;

// Function prototypes
void processMove(void);
void toString(void);
void init(void);
void drive(Direction direction, int speed);
void stop(void);
void signalHandler(int sig);

// Globals
Rover		rover;
	
int main(int argc, char** argv)
{
  init();

  // compass test code
  float min=999.9, max=0;
  int elapsed, mint=0x7fffffff, maxt=0;
  uint64_t start, end;
  while(1)
  {
    start = getuSecs();
    float heading = getHeading();
    end = getuSecs();
    if(heading > max) max = heading;
    if(heading < min) min = heading;
    elapsed = (int)(end-start);
    if(elapsed > maxt) maxt = elapsed;
    if(elapsed < mint) mint = elapsed;
    printf("heading: %3.2f,\tmin:%3.2f,\tmax:%3.2f\n", heading, min, max);
    printf("elapsed: %4dus, \tmin:%dus,\tmax:%dus\n", elapsed, mint, maxt);
    delay(100);
  }
/*
//setRange(25); // WARNING: This writes to the EEPROM
  // Routine to test the rangefinder function
  int minR=0, maxR=0, minT=0, maxT=0, dif, range;
  uint64_t start, end;
  while(1)
  {
    start = getuSecs();
    range = getRange();
    end = getuSecs();
    dif = (int)(end-start);
    if(range < minR || minR==0) minR=range;
    if(range > maxR) maxR=range;
    if(dif < minT || minT==0) minT=dif;
    if(dif > maxT) maxT=dif;
    printf("comptrig pin: %d\n", digitalRead(COMPTRIG));
    printf("range: %dcm min: %dcm max: %dcm\n", range, minR, maxR);
    printf("elapsed: %dus min: %dus max: %dus\n", dif, minT, maxT);
    delay(100);
  }
*/
/* 
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
/*
  // Main Loop - Rover
  while(1)
  {
    // check for a proximity alert from the rangefinder
    if(digitalRead(COMPTRIG) == 1)
      rover.proximityAlert = FALSE;
    else
      rover.proximityAlert = TRUE;

    // process next move
    processMove();

    // print rover state to screen
    toString();

    // sleep thread a while
    delay(100);
  }
  return 0;
*/
}

// Process the next move for the rover to make
void processMove()
{
  // proximity alert from the rangefinder
  if(rover.proximityAlert)
  {
    // the rover is driving, forward or turning?
    if(rover.isDriving)
    {
      if(rover.direction == FORWARD)
      {
        stop();
        printf("\nproximity alert! stopping\n");
      }
    }
    // the rover is stopped, look for a clear heading
    else
    {
      // decide which way to rotate
      if(rover.lastTurn == LEFT)
      {
        drive(RIGHT, rover.speed);
      }
      else
      {
        drive(LEFT, rover.speed);
      }
    }
  }
  // no proximity alert from the rangefinder
  else
  {
    if(rover.isDriving)
    {
      // is rover roving or searching for a clear heading?
      if(rover.direction == LEFT || rover.direction == RIGHT)
      {
        stop();
        printf("\nfound a clear heading\n");
      }
      else
      {
        // rover is roving forward, do nothing
      }
    }
    // rover is stopped, resume roving
    else
    {
      drive(FORWARD, rover.speed);
      printf("\nproximity alert clear, driving forward\n");
    }
  }
}

// Print rover state to stdout
void toString()
{
  // print status to stdout
  char* mving=(char*)"stopped";
  char* dir=(char*)"unknown";
  if(rover.isDriving)
    mving = "driving";
  if(rover.direction==FORWARD)
    dir = "forward";
  if(rover.direction==REVERSE)
    dir = "reverse";
  if(rover.direction==LEFT)
    dir = "left";
  if(rover.direction==RIGHT)
    dir = "right";
  printf("\r%s direction:%s speed: %d, proximity alert: %d      ",
          mving, dir, rover.speed, rover.proximityAlert);
  fflush(stdout);
}

void init()
{
  // handle CNTRL+C signal
  signal(SIGINT, &signalHandler);

  // Setup wiringPi to use GPIO mode
  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode(PWM, PWM_OUTPUT);	// enable pwm output
  pinMode(SHIFTEN, OUTPUT);	// enable level shifter control
  pinMode(M1POL, OUTPUT);	// enable M1 polarity control
  pinMode(M2POL, OUTPUT);	// enable M2 polarity control
  digitalWrite(SHIFTEN, 1);	// enable level shifter
  if(initRangefinder() < 0)
    printf("failed to init rangefinder\n");
  stop();
  rover.isDriving = FALSE;
  rover.direction = NONE;
  rover.lastTurn = NONE;
  rover.speed = 1024;
  rover.proximityAlert = TRUE;
}

void drive(Direction direction, int speed)
{
  int m1p = LOW, m2p = LOW;
  switch(direction)
  {
    case FORWARD:
      m1p = LOW;
      m2p = HIGH;
      break;
    case REVERSE:
      m1p = HIGH;
      m2p = LOW;
      break;
    case LEFT:
      m1p = HIGH;
      m2p = HIGH;
      rover.lastTurn = LEFT;
      break;
    case RIGHT:
      m1p = LOW;
      m2p = LOW;
      rover.lastTurn = RIGHT;
      break;
    case NONE:
    default:
      return;
  }
  digitalWrite(M1POL, m1p);
  digitalWrite(M2POL, m2p);
  pwmWrite(PWM, speed);
  rover.isDriving = TRUE;
  rover.direction = direction;
}

void stop()
{
  pwmWrite(PWM, 0);
  rover.isDriving = FALSE;
}

// Handle OS signals
void signalHandler(int sig)
{
  printf("\nControl+C received, shutting down\n");
  stop();	// stop the rover
  exit(0);	// exit the program
}
