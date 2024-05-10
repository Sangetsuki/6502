C_FLAGS = -c -O3 -I include -Wall
LINKER_FLAGS = -lraylib
ifdef DEBUG
	C_FLAGS += -g
endif

build/6502: build/main.o build/memory.o build/cpu.o build/rom.o build/nes.o
	gcc $^ $(LINKER_FLAGS) -o $@

build/memory.o: src/memory.c include/memory.h
	gcc $< $(C_FLAGS) -o $@

build/main.o: src/main.c
	gcc $< $(C_FLAGS) -o $@

build/cpu.o: src/cpu.c include/cpu.h
	gcc $< $(C_FLAGS) -o $@

build/rom.o: src/rom.c include/rom.h	
	gcc $< $(C_FLAGS) -o $@

build/nes.o: src/nes.c include/nes.h
	gcc $< $(C_FLAGS) -o $@
