CC = clang
CFLAGS = -Wall
DEPS =  cpu.h fontset.h display.h keyboard.h
OBJ = dist/main.o dist/cpu.o dist/display.o dist/keyboard.o

dist/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

chip8: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
