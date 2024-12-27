CC=gcc
CFLAGS=-Wall -Wextra -ggdb -std=c99 -pedantic -I/usr/include/freetype2 -O3
LIBS=-lX11 -lXft
DEPS=ui.h sort.h
FILES=main.o ui.o sort.o

all: $(FILES) $(DEPS)
	@$(CC) $(CFLAGS) $(LIBS) $^ -o out
	@echo CC $^

%.o: %.c Makefile $(DEPS)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $<

clean:
	rm *.o out

.PHONY: clean
