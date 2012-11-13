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
#define PROX_ALERT	(25)

// Broadcom GPIO map
#define TXD     	(14)
#define RXD     	(15)
#define PWM     	(18)
#define COMPTRIG	(4)
#define M1POL		(27)
#define M2POL		(17)


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
  int		range;
  int		speed;
  bool		proximityAlert;
  short		heading;
} Rover;

// Function prototypes
void processMove(void);
void toString(void);
void init(void);
void drive(Direction direction, int speed);
void stop(void);
void signalHandler(int sig);
void testRange(void);
void testCompass(void);

// Globals
Rover		rover;
int 		minR=0, maxR=0, mint=0, maxt=0, minT=0, maxT=0, dif, range;
float		minF=0, maxF=0;
uint64_t 	start, end;
	
int main(int argc, char** argv)
{
  init();

  while(1)
  {
    testRange();
    testCompass();
    delay(25);
  }

/*
  // Main Loop - Rover
  while(1)
  {
    // get current heading
    rover.heading = (short)getHeading();

    while((rover.range = getRange()) < 0);
    rover.proximityAlert = rover.range <= PROX_ALERT;

    // process next move
    processMove();

    // print rover state to screen
    toString();

    // sleep thread a while
    delay(25);
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
  printf("\r%s: %s, speed: %d, proximity alert: %d, heading: %d       ",
          mving, dir, rover.speed, rover.proximityAlert, rover.heading);
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
  pinMode(M1POL, OUTPUT);	// enable M1 polarity control
  pinMode(M2POL, OUTPUT);	// enable M2 polarity control
  stop();
  if(initRangefinder() < 0)
    printf("failed to init rangefinder\n");
  rover.isDriving = FALSE;
  rover.direction = NONE;
  rover.lastTurn = NONE;
  rover.speed = 1024;
  rover.range = -1;
  rover.proximityAlert = TRUE;
  rover.heading = -1;
}

void testRange()
{
  start = getuSecs();
  range = getRange();
  end = getuSecs();
  dif = (int)(end-start);
  if(range < minR || minR==0) minR=range;
  if(range > maxR) maxR=range;
  if(dif < minT || minT==0) minT=dif;
  if(dif > maxT) maxT=dif;
  //printf("comptrig pin: %d\n", digitalRead(COMPTRIG));
  printf("range: %dcm min: %dcm max: %dcm\n", range, minR, maxR);
  printf("elapsed: %dus min: %dus max: %dus\n", dif, minT, maxT);
}

void testCompass()
{
  start = getuSecs();
  float heading = getHeading();
  end = getuSecs();
  if(heading > maxF) maxF = heading;
  if(heading < minF || minF == 0) minF = heading;
  dif = (int)(end-start);
  if(dif > maxt) maxt = dif;
  if(dif < mint || mint == 0) mint = dif;
  printf("heading: %3.2f,\tmin:%3.2f,\tmax:%3.2f\n", heading, minF, maxF);
  printf("elapsed: %4dus, \tmin:%dus,\tmax:%dus\n", dif, mint, maxt);
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
