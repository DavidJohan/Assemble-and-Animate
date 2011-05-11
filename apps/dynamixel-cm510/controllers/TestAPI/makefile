
SOURCES=TestApi.c
OBJECTS=$(SOURCES:.c=.o)


all:
	avr-gcc -std=gnu99 -mmcu=atmega2561 -c -DDYNAMIXEL -DCM510 -Wall -I$(ASE_HOME)/include/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../cm510/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
