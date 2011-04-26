CC=gcc -g
TARGET=dynamixel-d2u
CFLAGS=-Wall -DDYNAMIXEL -I../../../include/$(TARGET)
LIBFLAGS=-L. -L../../../extlib/ftd2xx/ -L../../../lib/$(TARGET) -lControl -lASE -lm -lftd2xx
APPLICATION=-o appX
FULLSOURCE=startDynamixel.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)


all: $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
