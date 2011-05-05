
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	gcc -c -DDUMMY -Wall -I$(ASE_HOME)/include/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../dummy/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
