
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" and "fpga" 

fpga:
	mb-gcc -Os -c -DFPGA -Wall -I$(ASE_HOME)/include $(SOURCES)
	mb-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../fpga/libControl.a
tos: 
	avr-gcc -Os -c -DTINYOS -Wall -mmcu=atmega128 -I$(ASE_HOME)/include $(SOURCES)
	avr-ar -rcs libControl.a $(OBJECTS)
	mv libControl.a ../../tos/libControl.a 

ussr:
	gcc -c -DUSSR -DATRON -Wall -I$(ASE_HOME)/include $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
