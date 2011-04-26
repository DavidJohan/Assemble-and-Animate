
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "webots" and "tos"

tos: 
	avr-gcc -c -Wall -mmcu=atmega128 -I../../../../include/khepera-tinyos $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a 

webots:
	gcc -c -Wall -I../../../../include/khepera-webots/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../webots/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a