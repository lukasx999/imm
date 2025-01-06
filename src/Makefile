include config.mk

CFLAGS=-Wall -Wextra -ggdb -std=c99 -pedantic -O3 `pkg-config --cflags x11 xft`
LIBS=-lm `pkg-config --libs x11 xft`
DEPS=ui.h sort.h config.h
FILES=main.o ui.o sort.o
BIN=imm

all: imm

imm: $(FILES) $(DEPS)
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BIN)
	@echo CC $^

%.o: %.c Makefile $(DEPS)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $<

clean:
	rm *.o $(BIN)

install: all
	cp -f imm $(INSTALL_DIR)

uninstall:
	rm -f $(INSTALL_DIR)/imm

.PHONY: clean install uninstall
