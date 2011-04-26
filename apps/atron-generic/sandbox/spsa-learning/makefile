
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	avr-gcc -c -Wall -mmcu=atmega128 -I../../../../include/atron-tinyos $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a 

ussr:
	gcc -c -Wall -I../../../../include/atron-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a