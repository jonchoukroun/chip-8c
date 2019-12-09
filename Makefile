CC=clang
CFLAGS=-I
DEPS=chip8_cpu.h fontset.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: chip8.o chip8_cpu.o
	$(CC) -o chip8 chip8.o chip8_cpu.o
