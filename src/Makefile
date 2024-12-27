CC=gcc
CFLAGS=-Wall -Wextra -ggdb -std=c99 -pedantic -I/usr/include/freetype2 -O3
LIBS=-lX11 -lXft
DEPS=ui.h fuzzy.h
FILES=main.o ui.o fuzzy.o

all: $(FILES) $(DEPS)
	@$(CC) $(CFLAGS) $(LIBS) $^ -o out
	@echo CC $^

%.o: %.c Makefile $(DEPS)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $<

clean:
	rm *.o out

.PHONY: clean
