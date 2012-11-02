DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe
MODULES	= compass.c rangefinder.c serial.c time.c

LDFLAGS	= -L/usr/local/lib
LIBS    = -lwiringPi -lm


SRC	=	rover.c

OBJ	=	rover.o

all:		rover

rover:	rover.o
	@echo [link]
	$(CC) -o $@ rover.o $(MODULES) $(LDFLAGS) $(LIBS)

.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) *~ core tags rover

tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

depend:
	makedepend -Y $(SRC)
