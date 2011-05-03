CC=avr-gcc
TARGET=dynamixel-CM510
CFLAGS=-mmcu=atmega2561 -Wall -DDYNAMIXEL -DCM510 -I$(ASE_HOME)/include/
LIBFLAGS=-L. -L$(ASE_HOME)/lib/$(TARGET) -lControl -lASE -lm 
APPLICATION=-o appX.elf
FULLSOURCE=startDynamixel.c
SOURCES=$(notdir $(FULLSOURCE))
OBJECTS=$(SOURCES:.c=.o)


all: $(FULLSOURCE) $(OBJECTS)
	$(CC) $(CFLAGS) $(FULLSOURCE) $(LIBFLAGS) $(APPLICATION)
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature appX.elf appX.hex
	
	
.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
