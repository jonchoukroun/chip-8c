CC = clang
CFLAGS = -Wall
LDFLAGS = -lncurses
DEPS = chip8_io.h cpu.h fontset.h
OBJ = dist/chip8_io.o dist/main.o dist/cpu.o

dist/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

chip8: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
