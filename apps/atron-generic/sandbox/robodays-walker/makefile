DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	avr-gcc -Os -c -Wall -D TINYOS -D ATRON -mmcu=atmega128 -I../../../../include/atron-tinyos $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a

#SnakeToQuadruped_gen.c: SnakeToQuadruped.dycor
#	$(DCRC) --target==ansic SnakeToQuadruped.dycor

ussr: 
	gcc -c -Wall -D USSR -D ATRON -I../../../../include/atron-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
