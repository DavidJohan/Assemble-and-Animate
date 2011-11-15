
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	gcc -std=gnu99 -c -DDYNAMIXEL -DCM510 -Wall -I$(ASE_HOME)/include/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../local/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
