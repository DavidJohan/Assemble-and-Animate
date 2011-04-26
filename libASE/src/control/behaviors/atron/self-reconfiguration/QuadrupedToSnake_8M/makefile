DCRC=../../../../../../../Dynarole/scripts/dycorc

TARGET=QuadrupedToSnake_8M

all:
	@echo Legal target is "dynarole"

dynarole: $(TARGET).dycor
	$(DCRC) --target==ansic $(TARGET).dycor
	cp $(TARGET)_gen.c $(TARGET).c

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
