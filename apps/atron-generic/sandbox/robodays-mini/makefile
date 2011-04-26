DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c robodays_mini_gen.c #StateSharing.c #SnakeGait.c 
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	avr-gcc -Os -c -Wall -mmcu=atmega128 -I../../../../include/atron-tinyos $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a 

robodays_mini_gen.c: robodays_mini.dcr
	$(DCRC) --target=ansic -loop --platform=atron robodays_mini.dcr

ussr: robodays_mini_gen.c
	gcc -DLOOP -DPENDING_STATES -c -Wall -I../../../../include/atron-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a *_gen.c
