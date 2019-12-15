CC=clang
CFLAGS=-I
DEPS=cpu.h fontset.h display.h

dist/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: dist/chip8.o dist/cpu.o dist/display.o
	$(CC) -o dist/chip8 dist/chip8.o dist/cpu.o dist/display.o
