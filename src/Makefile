CC=gcc
CFLAGS=-Wall -Wextra -ggdb -std=c99 -pedantic -I/usr/include/freetype2
LIBS=-lX11 -lXft
DEPS=ui.h
FILES=main.o ui.o

all: $(FILES) $(DEPS)
	@$(CC) $(CFLAGS) $(LIBS) $^ -o out
	@echo CC $^

%.o: %.c Makefile $(DEPS)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $<

clean:
	rm *.o out

.PHONY: clean
