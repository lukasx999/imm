#!/usr/bin/env bash
set -euxo pipefail

cc main.c -Wall -Wextra -std=c99 -pedantic -ggdb -o out -lX11
