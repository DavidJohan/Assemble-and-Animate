DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c completeTest_gen.c 
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos"

tos:
	$(DCRC) --target=ansic -loop --platform=atron completeTest.dcr 
	avr-gcc -Os -c -DTINYOS -DATRON -Wall -mmcu=atmega128 -I$(ASE_HOME)/include $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a

completeTest_gen.c: completeTest.dcr
	$(DCRC) --target=ansic -loop --platform=atron completeTest.dcr

ussr: completeTest_gen.c
	gcc -DLOOP -DPENDING_STATES -DATRON -DUSSR -c -Wall -I$(ASE_HOME)/include $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a *_gen.c
