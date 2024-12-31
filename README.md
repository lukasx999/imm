# XMenu

**A suckless-style run launcher**

## Build instructions

### Requirements

- `gcc` (or any other C99 compiler)
- `make`
- `pkg-config`
- `libX11`
- `libXft`

### Building

- `cd src`
- `make` (to generate a binary in the current directory)
- `make install` (to install to the system)

> **NOTE:** `make install` will install `xmenu`, as well as `xmenu_run` to the system.

### Usage

XMenu works similar to other run-launchers like fzf and dmenu.\
It takes input as newline seperated strings from `stdin`, gives you a prompt, then writes the selected entry back to `stdout`.

**Examples:**

- `echo "foo\nbar\nbaz" | xmenu`
- `xmenu < somefile.txt`
```sh
xmenu
foo
bar
baz
^D
```

This gives it a lot of potential for usage with shell scripting.\\
A script for using XMenu as a launcher for desktop applicatoins can be found at `src/xmenu_run`.\\

## Keybindings

- Move cursor up: `<C-p>, <C-k>, <Up>`
- Move cursor down: `<C-n>, <C-j>, <Down>`
- Clear selection: `<C-u>`
- Quit: `<ESC>, <C-c>`
- Select entry: `<CR>`
