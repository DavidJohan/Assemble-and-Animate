
SOURCES=controller.c
OBJECTS=$(SOURCES:.c=.o)


all:
	gcc -c -DDUMMY -Wall -I../../../../include/dummy/ $(SOURCES) 
	ar -rcs libControl.a $(OBJECTS) 
	mv libControl.a ../../dummy/libControl.a 

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
