#!bin/bash
arm-none-eabi-gcc crt0.s main.c -mcpu=arm7tdmi -nostartfiles -Tlnkscript
