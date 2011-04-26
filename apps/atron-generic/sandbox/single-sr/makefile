DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c SnakeToQuadruped_gen.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	avr-gcc -Os -c -Wall -mmcu=atmega128 -I../../../../include/atron-tinyos $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a

SnakeToQuadruped_gen.c: SnakeToQuadruped.dycor
	$(DCRC) SnakeToQuadruped.dycor

ussr: SnakeToQuadruped_gen.c
	gcc -c -Wall -I../../../../include/atron-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
