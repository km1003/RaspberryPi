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
#define FALSE   (0)
#define TRUE    (1)

// Broadcom GPIO map
#define SHIFTEN (4)
#define TXD     (14)
#define RXD     (15)
#define M1POL   (17)
#define PWM     (18)
#define M2POL   (27)

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
  int		range;
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
    printf("range: %dcm min: %dcm max: %dcm\n", range, minR, maxR);
    printf("elapsed: %dus min: %dus max: %dus\n", dif, minT, maxT);
    delay(100);
  }
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
  // Main Loop
  while(1)
  {
    // get range infront of the rover
    rover.range = getRange();

    // process next move
    processMove();

    // print rover state to screen
    toString();

    // sleep thread a while
    delay(50);
  }
*/
  return 0;
}

// Process the next move for the rover to make
void processMove()
{
  // an is obsticle near
  if(rover.range < 25)
  {
    // rover is driving, stop to avoid a collision
    if(rover.isDriving && rover.direction == FORWARD)
    {
      stop();
      printf("\nobsticle near, stopping\n");
    }
    // rover is stopped, rotate right or left to look for a clear path
    if(!rover.isDriving)
    {
      // enough room to rotate
      if(rover.range > 15)
      {
        if(rover.lastTurn == LEFT) drive(RIGHT, rover.speed);
        else if(rover.lastTurn == RIGHT) drive(LEFT, rover.speed);
        else drive(LEFT, rover.speed);
        printf("\nrotating to a new heading\n");
      }
      // not enough room to rotate so reverse
      else
      {
        drive(REVERSE, rover.speed);
        printf("\nobsticle very close, reversing\n");
      }
    }
  }
  
  // resume driving when obsticle is clear
  if(rover.range > 35)
  {
    if(!rover.isDriving)
    {
      drive(FORWARD, rover.speed);
      printf("\nresume driving forward\n");
    }
    if(rover.isDriving)
    {
      if(rover.direction == RIGHT || rover.direction == LEFT)
      {
        stop();
        printf("\nfound a clear heading\n");
      }
      if(rover.direction == REVERSE)
      {
        stop();
        printf("\nreversed to a safe distance\n");
      }
    }
  }
}

// Print rover state to stdout
void toString()
{
  // print status to stdout
  char* mving=(char*)"stopped";
  char* dir=(char*)" ";
  if(rover.isDriving)
  {
    mving = "driving";
    if(rover.direction==FORWARD)
      dir = "forward";
    if(rover.direction==REVERSE)
      dir = "reverse";
    if(rover.direction==LEFT)
      dir = "left";
    if(rover.direction==RIGHT)
      dir = "right";
  }
  printf("\r%s %s at %d, range: %d                               ",
          mving, dir, rover.speed, rover.range);
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
  pwmWrite(PWM, 0);		// zero pwm output
  if(initRangefinder() < 0)
    printf("failed to init rangefinder\n");
  stop();
  rover.isDriving = FALSE;
  rover.direction = NONE;
  rover.lastTurn = NONE;
  rover.speed = 512;
  rover.range = 0;
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
