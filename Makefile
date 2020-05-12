INCDIR = include
LDIR = lib
OBJDIR = dist

CC = clang
WARNINGS = -Wall -Wextra -pedantic
CFLAGS = $(WARNINGS) -I$(INCDIR)

LIBS = -lSDL2

_DEPS = types.h constants.h input.h cpu.h output.h cycles.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = input.o output.o cpu.o cycles.o main.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chip8: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(OBJDIR)/*
