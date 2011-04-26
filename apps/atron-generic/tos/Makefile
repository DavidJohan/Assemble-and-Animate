APPLICATION=Controller
COMPONENT=StartATRONAppC
TARGET=atron-tinyos

#avr-gcc -mmcu=atmega128 -c -Wall controller.c -I../../../include/atron-tinyos

//CFLAGS+=controller.o 
CFLAGS+=-I../../../include/$(TARGET)

#CFLAGS += -DTOSH_DATA_LENGTH=40
#CFLAGS += -DPURE_IR_MULTI_CONNECTOR_TX

#CFLAGS += -DIR_POWER_PWM_HIGH_1_CONN=100
#CFLAGS += -DIR_POWER_PWM_HIGH_2_CONN=100
#CFLAGS += -DIR_POWER_PWM_HIGH_3_CONN=100
#CFLAGS += -DIR_POWER_PWM_HIGH_4_CONN=100

include $(MAKERULES)
LIBS+=-L. -L../../../lib/$(TARGET) -lControl -lASE -lm

