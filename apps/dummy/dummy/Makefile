CC=gcc
TARGET=dummy
CFLAGS=-Wall -I../../../include/$(TARGET)
LIBFLAGS=-L. -L../../../lib/$(TARGET) -lControl -lASE -lm -lpthread
APPLICATION=-o appX
FULLSOURCE=startDummy.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)

.PHONY: clean print libcontrol_print dummy_target all libcontrol_printf libcontrol_knn

all: dummy_target $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)

knn: libcontrol_knn all

printf: libcontrol_printf all

libcontrol_printf:
	cd ../controllers/PrintfBuffered/;make clean;make

libcontrol_knn:
	cd ../controllers/kNN-test/;make clean;make

dummy_target: 
	cd ../../../src/;make TARGET=${TARGET}

clean:
	rm -f *.o *.out *.gch *.a
