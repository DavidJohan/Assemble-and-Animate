DCRC=../../../../../Dynarole/scripts/dycorc
SOURCES=controller.c SnakeShiftHead_gen.c SnakeGait.c StateSharing.c
OBJECTS=$(SOURCES:.c=.o)


all:
	@echo Legal targets are "ussr" and "tos" 

tos: 
	@echo Target tos not supported for MTRAN

SnakeShiftHead_gen.c: SnakeShiftHead.dycor
	$(DCRC) --target=ansic --platform=mtran SnakeShiftHead.dycor

ussr: SnakeShiftHead_gen.c
	gcc -DPRINTF -c -Wall -I../../../../include/mtran-ussr/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../ussr/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a *_gen.c
