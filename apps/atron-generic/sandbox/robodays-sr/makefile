DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c robodays_sculpture_gen.c #StateSharing.c #SnakeGait.c 
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	@echo Target tos not supported for in this makefile, sorry

robodays_sculpture_gen.c: robodays_sculpture.dycor
	$(DCRC) --target=ansic -loop --platform=atron robodays_sculpture.dycor

ussr: robodays_sculpture_gen.c
	gcc -DLOOP -DPENDING_STATES -c -Wall -I../../../../include/atron-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a *_gen.c
