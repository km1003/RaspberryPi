#include <wiringSerial.h>

void writeSerial(unsigned char* buf, int len, int uart);
int readSerial(unsigned char* buf, int len, int uart);
