CC=gcc
TARGET=atron-ussr
CFLAGS=-Wall -I../../../include/$(TARGET)
LIBFLAGS=-L. -L../../../lib/$(TARGET) -lControl -lASE -lm -lpthread
APPLICATION=-o appX
FULLSOURCE=startAtron.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)


all: $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
