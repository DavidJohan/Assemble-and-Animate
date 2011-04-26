CC=gcc
TARGET=odin-ussr
CFLAGS=-Wall -I../../../include/$(TARGET)
LIBFLAGS=-L. -lm -L../../../lib/$(TARGET) -lASE -lpthread
APPLICATION=-o appX
FULLSOURCE=odin-cpg.c controller.c

SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)


all: $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
