CC=gcc
TARGET=dummy
CFLAGS=-Wall -I$(ASE_HOME)/include/
LIBFLAGS=-L. -L$(ASE_HOME)/lib/$(TARGET) -lControl -lASE -lm -lpthread
APPLICATION=-o appX
FULLSOURCE=startDummy.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)

.PHONY: clean print libcontrol_print dummy_target all libcontrol_printf libcontrol_knn

all: $(FULLSOURCE) $(OBJECTS)
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
