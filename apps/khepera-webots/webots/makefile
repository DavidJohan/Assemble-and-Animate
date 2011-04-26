CC=gcc
TARGET=khepera-webots
CFLAGS=-Wall -I../../../include/$(TARGET)
LIBFLAGS=-L. -L../../../lib/$(TARGET) -lControl -lASE -lm -lpthread
APPLICATION=-o appX
FULLSOURCE=startKhepera.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)


all: $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
