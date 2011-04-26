DCRC=../../../../../../../Dynarole/scripts/dycorc

all:
	@echo Legal target is "dynarole"

dynarole: SnakeToQuadruped_8M.dycor
	$(DCRC) --target==ansic SnakeToQuadruped_8M.dycor
	cp SnakeToQuadruped_8M_gen.c SnakeToQuadruped_8M.c

.PHONY: clean
clean:
	rm -f *.o *.out *.gch *.a
