
SOURCES=controller.c CM510Behaviors.c BeatDetector.c
OBJECTS=$(SOURCES:.c=.o)


all:
	avr-gcc -mmcu=atmega2561 -std=gnu99 -c -DDYNAMIXEL -DCM510 -Wall -I$(ASE_HOME)/include/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../cm510/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
