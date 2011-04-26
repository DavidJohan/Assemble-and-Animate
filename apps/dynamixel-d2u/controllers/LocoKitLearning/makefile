SOURCES=controller.c RotationController.c KeyController.c PositionTracker.c
OBJECTS=$(SOURCES:.c=.o)


all:
	gcc -c -DDYNAMIXEL -DD2U  -Wall -I../../../../include/dynamixel-d2u/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../d2u/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
