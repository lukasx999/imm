# imm

## A suckless-style run launcher

### Requirements

- `gcc`
- `make`
- `pkg-config`
- `libX11`
- `libXft`

### Building

- `cd src`
- Modify `config.mk` to fit your enviroment (you probably want to change `INSTALL_DIR` from `~/.opt` to `/usr/local/bin` or `/usr/bin`
- `make` (to generate a binary in the current directory)\
**or:**
- `make install` (to install to `INSTALL_DIR`)

### Usage

imm works similar to other run-launchers like fzf and dmenu.\
It takes newline delimited strings from `stdin` as input, gives you a prompt, then writes the selected entry back to `stdout`.

**Examples:**

`echo "foo\nbar\nbaz" | imm`

`imm < somefile.txt`

```sh
imm
foo
bar
baz
^D
```

## Configuration

Configuration is done by editing `src/config.h`, and rebuilding the source code.

## Keybindings

The keybindings are generally inspired by GNU readline and Vim, aiming to satisfy both kinds of users.

- Move cursor up: `<C-p>`, `<C-k>`, `<Up>`
- Move cursor down: `<C-n>`, `<C-j>`, `<Down>`
- Clear selection: `<C-u>`
- Quit: `<ESC>`, `<C-c>`
- Select entry: `<CR>`

## Command Line Options

- `-h`: print usage (**h**elp)
- `-v`: print **v**ersion

## Exit Code

Exit code is 1, if no string was matched, and the current query is returned
