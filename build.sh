#!/bin/sh
set -e

x86_64-w64-mingw32-clang iconake.c -o iconake.exe \
    -Os -flto -s \
    -ffreestanding -fno-builtin \
    -fno-stack-protector -mno-stack-arg-probe \
    -fno-asynchronous-unwind-tables -fno-ident \
    -ffunction-sections -fdata-sections \
    -nostdlib \
    -Wl,--entry,entry \
    -Wl,--subsystem,windows \
    -Wl,--gc-sections \
    -lkernel32 -luser32 -lole32 -lwinmm