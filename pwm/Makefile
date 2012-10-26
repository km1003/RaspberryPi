DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LIBS    = -lwiringPi


SRC	=	pwm.c

OBJ	=	pwm.o

all:		pwm

pwm:	pwm.o
	@echo [link]
	$(CC) -o $@ pwm.o $(LDFLAGS) $(LIBS)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) *~ core tags pwm

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)
