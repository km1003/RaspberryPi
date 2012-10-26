#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "wiringPi.h"

int main(int argc, char** argv)
{
  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  return 0;
}
