CC=gcc
CFLAGS=-Wall -Wextra -ggdb -std=c99 -pedantic `pkg-config --cflags xft` -O3
LIBS=-lX11 `pkg-config --libs xft` -lm -lXext
DEPS=ui.h sort.h config.h
FILES=main.o ui.o sort.o
BIN=xmenu

all: xmenu

xmenu: $(FILES) $(DEPS)
	@$(CC) $(CFLAGS) $(LIBS) $^ -o $(BIN)
	@echo CC $^

%.o: %.c Makefile $(DEPS)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $<

clean:
	rm *.o $(BIN)

install: all
	cp -f xmenu ~/.opt
	cp -f xmenu_run.sh ~/.opt/xmenu_run

uninstall:
	rm -f ~/.opt/xmenu ~/.opt/xmenu_run

.PHONY: clean install uninstall
